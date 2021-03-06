#ifndef PROGRAM_H
#define PROGRAM_H

#include <string>
#include <map>
#include <set>
#include <memory>
#include <vector>

#include <StandAlone/ResourceLimits.h>
#include <glslang/MachineIndependent/localintermediate.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/Include/intermediate.h>
#include <SPIRV/GlslangToSpv.h>
#include <SPIRV/disassemble.h>
#include <spirv-tools/libspirv.h>
#include <spirv-tools/optimizer.hpp>
#include "spirv.h"
#include "GLSL.std.450.h"

#include "basic_types.h"
#include "image.h"
#include "timer.h"

// List of shared instruction pointers.
#include "opcode_structs.h"

const uint32_t NO_MEMORY_ACCESS_SEMANTIC = 0xFFFFFFFF;

const uint32_t SOURCE_NO_FILE = 0xFFFFFFFF;
const uint32_t NO_INITIALIZER = 0xFFFFFFFF;
const uint32_t NO_ACCESS_QUALIFIER = 0xFFFFFFFF;
const uint32_t NO_RETURN_REGISTER = 0xFFFFFFFF;
const uint32_t NO_FUNCTION = 0xFFFFFFFF;

extern std::map<uint32_t, std::string> OpcodeToString;

struct VariableInfo
{
    uint32_t address;       // The address of this variable within memory
    size_t size;            // Size in bytes of this element for type checking
};

// Section of memory for a specific use.
struct MemoryRegion
{
    // Offset within the "memory" array.
    size_t base;

    // Size of the region.
    size_t size;

    // Offset within the "memory" array of next allocation.
    size_t top;

    MemoryRegion() :
        base(0),
        size(0),
        top(0)
    {}
    MemoryRegion(size_t base_, size_t size_) :
        base(base_),
        size(size_),
        top(base_)
    {}
};

// helper type for visitor
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

// The static state of the program.
struct Program 
{
    bool throwOnUnimplemented;
    bool hasUnimplemented;
    bool verbose;
    std::set<uint32_t> capabilities;

    LineInfo currentLine;

    // main id-to-thingie map containing extinstsets, types, variables, etc
    // secondary maps of entryPoint, decorations, names, etc

    std::map<uint32_t, std::string> extInstSets;
    uint32_t ExtInstGLSL_std_450_id;
    std::map<uint32_t, std::string> strings;
    uint32_t memoryModel;
    uint32_t addressingModel;
    std::map<uint32_t, EntryPoint> entryPoints;

    std::map<uint32_t, std::string> names;
    std::map<uint32_t, std::map<uint32_t, std::string> > memberNames;

    // decorations is indexed by id, then decoration type, yielding operands.
    // e.g. offset = decorations[variable_fragCoord][SpvDecorationLocation][0];
    std::map<uint32_t, Decorations > decorations;

    // memberDecorations is indexed by id, then member number,
    // then decoration type, yielding operands.
    // e.g. offset = decorations[var_params][mem_iTime][SpvDecorationOffset][0];
    std::map<uint32_t, std::map<uint32_t, Decorations> > memberDecorations;

    std::vector<Source> sources;
    std::vector<std::string> processes;
    std::map<uint32_t, std::shared_ptr<Type>> types;
    std::map<uint32_t, size_t> typeSizes; // XXX put into Type
    std::map<uint32_t, Variable> variables;
    // Map from function ID to Function object.
    std::map<uint32_t, std::shared_ptr<Function>> functions;
    // Map from virtual register ID to its type. Only used for results of instructions.
    std::map<uint32_t, uint32_t> resultTypes;
    std::map<uint32_t, Register> constants;
    std::map<std::string, VariableInfo> namedVariables;

    // For expanding vectors to scalars:
    uint32_t nextReg = 10000; // XXX make sure this doesn't conflict with actual registers.
    using RegIndex = std::pair<uint32_t,int>;
    std::map<RegIndex,uint32_t> vec2scalar;

    SampledImage sampledImages[16];

    // Only valid while parsing:
    std::shared_ptr<Function> currentFunction;
    std::shared_ptr<Block> currentBlock;

    // ID of main function, or NO_FUNCTION if not set.
    uint32_t mainFunctionId;

    size_t memorySize;

    std::map<uint32_t, MemoryRegion> memoryRegions;

    // Returns the type as the specific subtype. Does not check to see
    // whether the object is of the specific subtype.
    template <class T>
    const T *type(uint32_t typeId) const {
        return dynamic_cast<const T *>(types.at(typeId).get());
    }

    Register& allocConstantObject(uint32_t id, uint32_t type)
    {
        constants[id] = Register {type, typeSizes[type]};
        constants[id].initialized = true;
        return constants[id];
    }

    Program(bool throwOnUnimplemented_, bool verbose_);

    // Get the type ID of the entity. Returns 0 if not found.
    uint32_t typeIdOf(uint32_t id) const {
        // Look for operation results.
        auto itr1 = resultTypes.find(id);
        if (itr1 != resultTypes.end()) {
            return itr1->second;
        }

        // Look for constants.
        auto itr2 = constants.find(id);
        if (itr2 != constants.end()) {
            return itr2->second.type;
        }

        // Not found. Should probably look in variables, etc.
        return 0;
    }

    // Returns whether this register is a constant.
    bool isConstant(uint32_t regId) const {
        return constants.find(regId) != constants.end();
    }

    size_t allocate(SpvStorageClass clss, uint32_t type)
    {
        MemoryRegion& reg = memoryRegions[clss];
        if(false) {
            std::cout << "allocate from " << clss << " type " << type << "\n";
            std::cout << "region at " << reg.base << " size " << reg.size << " top " << reg.top << "\n";
            std::cout << "object is size " << typeSizes[type] << "\n";
        }
        assert(reg.top + typeSizes[type] <= reg.base + reg.size);
        size_t address = reg.top;
        reg.top += typeSizes[type];
        return address;
    }
    size_t allocate(uint32_t clss, uint32_t type)
    {
        return allocate(static_cast<SpvStorageClass>(clss), type);
    }

    // Returns the type of and byte offset to the member of "t" at
    // index "i". For structs, members are zero-indexed.
    ConstituentInfo getConstituentInfo(uint32_t t, int i) const
    {
        return types.at(t)->getConstituentInfo(i);
    }

    // If the type ID refers to a TypeVector, returns it. Otherwise returns null.
    const TypeVector *getTypeAsVector(int typeId) const {
        const Type *type = types.at(typeId).get();
        return type->op() == SpvOpTypeVector ? dynamic_cast<const TypeVector *>(type) : nullptr;
    }

    // If the type ID refers to a TypeMatrix, returns it. Otherwise returns null.
    const TypeMatrix *getTypeAsMatrix(int typeId) const {
        const Type *type = types.at(typeId).get();
        return type->op() == SpvOpTypeMatrix ? dynamic_cast<const TypeMatrix *>(type) : nullptr;
    }

    // Return the SPIR-V operator for the type (SpvOpTypeInt, SpvOpTypePointer, SpvOpTypeBool,
    // SpvOpTypeFloat, etc.).
    uint32_t getTypeOp(uint32_t typeId) const {
        return types.at(typeId)->op();
    }

    // Returns true if a type is a float, false if it's an integer, pointer, or bool,
    // otherwise asserts.
    bool isTypeFloat(uint32_t typeId) const {
        uint32_t op = getTypeOp(typeId);
        if (op == SpvOpTypeInt
                || op == SpvOpTypePointer
                || op == SpvOpTypeBool) {

            return false;
        } else if (op == SpvOpTypeFloat) {
            return true;
        } else {
            std::cerr << "Error: Type " << typeId << " is neither int nor float.\n";
            assert(false);
        }
    }

    static spv_result_t handleHeader(void* user_data, spv_endianness_t endian,
                               uint32_t /* magic */, uint32_t version,
                               uint32_t generator, uint32_t id_bound,
                               uint32_t schema);

    static spv_result_t handleInstruction(void* user_data, const spv_parsed_instruction_t* insn);

    void storeNamedVariableInfo(const std::string& name, uint32_t typeId, uint32_t address)
    {
        const Type *type = types[typeId].get();

        switch (type->op()) {
            case SpvOpTypeStruct: {
                const TypeStruct *typeStruct = dynamic_cast<const TypeStruct *>(type);

                for(size_t i = 0; i < typeStruct->memberTypes.size(); i++) {
                    uint32_t membertype = typeStruct->memberTypeIds[i];
                    std::string fullname = ((name == "") ? "" : (name + ".")) + memberNames[typeId][i];
                    storeNamedVariableInfo(fullname, membertype, address + memberDecorations[typeId][i][SpvDecorationOffset][0]);
                }
                break;
            }

            case SpvOpTypeArray: {
                const TypeArray *typeArray = dynamic_cast<const TypeArray *>(type);

                for(size_t i = 0; i < typeArray->count; i++) {
                    uint32_t membertype = typeArray->type;
                    std::string fullname = name + "[" + std::to_string(i) + "]";
                    storeNamedVariableInfo(fullname, membertype, address + i * typeSizes.at(membertype));
                }
                break;
            }

            case SpvOpTypeVector:
            case SpvOpTypeFloat:
            case SpvOpTypeInt:
            case SpvOpTypeSampledImage:
            case SpvOpTypeBool:
            case SpvOpTypeMatrix: {
                namedVariables[name] = {address, typeSizes.at(typeId)};
                break;
            }

            default:
                std::cout << "Unhandled type for finding uniform variable offset and size\n";
                break;
        }
    }

    // Post-parsing work.
    void postParse();

    // Create data structures that compiler will use.
    void prepareForCompile();

    // Return the scalar for the vector register's index.
    uint32_t scalarize(uint32_t vreg, int i, uint32_t subtype, uint32_t scalarReg = 0,
            bool mustExist = false);

    // If typeVector is null, just returns vreg. Else calls scalarize() with the
    // typeVector's subtype.
    uint32_t scalarize(uint32_t vreg, int i, const TypeVector *typeVector);

    // Replace the SPIR-V Phi instructions with ours.
    void replacePhi();
    void replacePhiInFunction(Function *function);
    void replacePhiInBlock(Block *block);

    // Transform vector instructions to scalar instructions.
    void expandVectors();
    void expandVectorsInFunction(Function *function);
    void expandVectorsInBlockTree(Block *block);
    void expandVectorsInBlock(Block *block);

    // Compute livein and liveout registers for each line.
    void computeLiveness();

    // Expand a unary operator, such as FNegate, from vector to scalar if necessary.
    template <class T>
    void expandVectorsUniOp(Instruction *instruction, InstructionList &newList, bool &replaced) {
        T *insn = dynamic_cast<T *>(instruction);
        const TypeVector *typeVector = getTypeAsVector(typeIdOf(insn->resultId()));
        if (typeVector != nullptr) {
            for (uint32_t i = 0; i < typeVector->count; i++) {
                auto [subtype, offset] = getConstituentInfo(insn->type, i);
                newList.push_back(std::make_shared<T>(insn->lineInfo,
                            subtype,
                            scalarize(insn->resultId(), i, subtype),
                            scalarize(insn->argIdList[0], i, subtype)));
            }
            replaced = true;
        }
    }

    // Expand a binary operator, such as FMul, from vector to scalar if necessary.
    template <class T>
    void expandVectorsBinOp(Instruction *instruction, InstructionList &newList, bool &replaced) {
        T *insn = dynamic_cast<T *>(instruction);
        const TypeVector *typeVector = getTypeAsVector(typeIdOf(insn->resultId()));
        if (typeVector != nullptr) {
            const TypeVector *typeVector0 = getTypeAsVector(typeIdOf(insn->argIdList[0]));
            const TypeVector *typeVector1 = getTypeAsVector(typeIdOf(insn->argIdList[1]));
            uint32_t arg0Subtype = typeVector0->type;
            uint32_t arg1Subtype = typeVector1->type;

            for (uint32_t i = 0; i < typeVector->count; i++) {
                auto [subtype, offset] = getConstituentInfo(insn->type, i);
                newList.push_back(std::make_shared<T>(insn->lineInfo,
                            subtype,
                            scalarize(insn->resultId(), i, subtype),
                            scalarize(insn->argIdList[0], i, arg0Subtype),
                            scalarize(insn->argIdList[1], i, arg1Subtype)));
            }
            replaced = true;
        }
    }

    // Expand a ternary operator, such as FClamp, from vector to scalar if necessary.
    template <class T>
    void expandVectorsTerOp(Instruction *instruction, InstructionList &newList, bool &replaced) {
        T *insn = dynamic_cast<T *>(instruction);
        const TypeVector *typeVector = getTypeAsVector(typeIdOf(insn->resultId()));
        if (typeVector != nullptr) {
            for (uint32_t i = 0; i < typeVector->count; i++) {
                auto [subtype, offset] = getConstituentInfo(insn->type, i);
                newList.push_back(std::make_shared<T>(insn->lineInfo,
                            subtype,
                            scalarize(insn->resultId(), i, subtype),
                            scalarize(insn->argIdList[0], i, subtype),
                            scalarize(insn->argIdList[1], i, subtype),
                            scalarize(insn->argIdList[2], i, subtype)));
            }
            replaced = true;
        }
    }
};

#endif /* PROGRAM_H */
