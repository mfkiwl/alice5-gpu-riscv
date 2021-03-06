#ifndef BASIC_TYPES_H
#define BASIC_TYPES_H

#include <assert.h>
#include <memory>
#include <algorithm>
#include <iostream>
#include <map>
#include <array>
#include <vector>
#include <set>

#include "spirv.h"

typedef std::array<float,1> v1float;
typedef std::array<uint32_t,1> v1uint;
typedef std::array<int32_t,1> v1int;
typedef std::array<float,2> v2float;
typedef std::array<uint32_t,2> v2uint;
typedef std::array<int32_t,2> v2int;
typedef std::array<float,3> v3float;
typedef std::array<uint32_t,3> v3uint;
typedef std::array<int32_t,3> v3int;
typedef std::array<float,4> v4float;
typedef std::array<uint32_t,4> v4uint;
typedef std::array<int32_t,4> v4int;

constexpr bool PRINT_TIMER_RESULTS = false;

const uint32_t NO_LINE = 0xFFFFFFFF;
const uint32_t NO_FILE = 0xFFFFFFFF;
const uint32_t NO_COLUMN = 0xFFFFFFFF;
// Information about which file, line, and column an instruction is from
struct LineInfo
{
    uint32_t fileId;
    uint32_t line;
    uint32_t column;
    LineInfo() :
        fileId(NO_FILE),
        line(NO_LINE),
        column(NO_COLUMN)
        {}
    LineInfo(uint32_t fileId_, uint32_t line_, uint32_t column_) :
        fileId(fileId_),
        line(line_),
        column(column_)
        {}
};


const uint32_t NO_REGISTER = 0xFFFFFFFF;
const uint32_t NO_BLOCK_ID = 0xFFFFFFFF;

// A variable in memory, either global or within a function's frame.
struct Variable
{
    // Type of variable (not the pointer to it). Key into the "types" map.
    uint32_t type;

    // One of the values of SpvStorageClass (input, output, uniform, function, etc.).
    uint32_t storageClass;

    // XXX Not sure, or NO_INITIALIZER.
    uint32_t initializer;

    // Address in memory array
    size_t address;
};

// Entry point for the shader.
struct EntryPoint
{
    // What kind of shader. Must be SpvExecutionModelFragment.
    uint32_t executionModel;

    // Name of the function to call. This is typically "main".
    std::string name;

    // XXX Don't know.
    std::vector<uint32_t> interfaceIds;

    // XXX Don't know what an execution mode is.
    std::map<uint32_t, std::vector<uint32_t>> executionModesToOperands;
};

// Information about the source code for the shader.
struct Source
{
    // What language the shader was written in.
    uint32_t language;

    // XXX Version of the shading language.
    uint32_t version;

    // XXX Dunno.
    uint32_t file;

    // XXX Filename of the shader source?
    std::string source;
};

// A decoration tells us something about a type or variable, like 
// how to access it from outside the shader (OpDecorationBinding),
// the offset of fields within a struct (OpDecorationOffset), or
// the stride of elements with in an array (OpDecorationArrayStride,
// or whether a scalar can have relaxed precision (OpDecorateRelaxedPrecision).
// See the SPIR-V spec for "OpDecorate" for more details on the
// available decorations.
//
// This type represents a list of decoration ids (e.g. OpDecorationLocation)
// mapped to the decoration's operands.  At the time of SPIR-V 1.2, all
// decorations have 0 or 1 operands except OpDecorationLinkageAttributes,
// which has 2.  So the operands vector almost always has 0 or 1 operands.
typedef std::map<uint32_t, std::vector<uint32_t> > Decorations;

// XXX Unused.
struct MemberName
{
    uint32_t type;
    uint32_t member;
    std::string name;
};

// Component of an aggregate type.
struct ConstituentInfo {
    // Type of the sub-element.
    uint32_t subtype;

    // Offset in bytes of the sub-element.
    size_t offset;
};

// Template to case generate data to a specific type.
template <class T>
T& objectAt(unsigned char* data)
{
    return *reinterpret_cast<T*>(data);
}

// Base class for types.
struct Type {
    Type(uint32_t size) : size(size) {}
    virtual ~Type() {
        // Nothing.
    }

    // Number of bytes.
    uint32_t size;

    virtual uint32_t op() const = 0;
    virtual ConstituentInfo getConstituentInfo(int i) const {
        // Most types don't have this.
        std::cerr << "No constituent info for op " << op() << "\n";
        assert(false);
    }
    virtual uint32_t getSubtypeSize() const {
        // Most types don't have this.
        std::cerr << "No subtype size for op " << op() << "\n";
        assert(false);
    }
    virtual void dump(unsigned char *data) const = 0;
};

// Represents a "void" type.
struct TypeVoid : public Type
{
    TypeVoid() : Type(0) {}
    virtual uint32_t op() const { return SpvOpTypeVoid; }
    virtual void dump(unsigned char *data) const {
        std::cout << "{}";
    }
};

// Represents an "bool" type.
struct TypeBool : public Type
{
    TypeBool() : Type(sizeof(bool)) {}
    virtual uint32_t op() const { return SpvOpTypeBool; }
    virtual void dump(unsigned char *data) const {
        std::cout << objectAt<bool>(data);
    }
};

// Represents an "int" type.
struct TypeInt : public Type
{
    // Number of bits in the int.
    uint32_t width;

    // Whether signed.
    uint32_t signedness;

    TypeInt(uint32_t width, uint32_t signedness)
        : Type(sizeof(uint32_t)), width(width), signedness(signedness) {}
    virtual uint32_t op() const { return SpvOpTypeInt; }
    virtual void dump(unsigned char *data) const {
        if(signedness) {
            std::cout << objectAt<int32_t>(data);
        } else {
            std::cout << objectAt<uint32_t>(data);
        }
    }
};

// Represents a "float" type.
struct TypeFloat : public Type
{
    // Number of bits in the float.
    uint32_t width;

    TypeFloat(uint32_t width)
        : Type(sizeof(float)), width(width) {}
    virtual uint32_t op() const { return SpvOpTypeFloat; }
    virtual void dump(unsigned char *data) const {
        std::cout << objectAt<float>(data);
    }
};

// Represents a vector type.
struct TypeVector : public Type
{
    // Type of each element.
    std::shared_ptr<Type> subtype;

    // Type of the element. This is a key in the "types" map.
    uint32_t type;

    // Number of elements.
    uint32_t count;

    TypeVector(std::shared_ptr<Type> subtype, uint32_t type, uint32_t count)
        : Type(subtype->size*count), subtype(subtype), type(type), count(count) {}
    virtual uint32_t op() const { return SpvOpTypeVector; }
    virtual ConstituentInfo getConstituentInfo(int i) const {
        return { type, i*subtype->size };
    }
    virtual uint32_t getSubtypeSize() const {
        return subtype->size;
    }
    virtual void dump(unsigned char *data) const {
        std::cout << "<";
        for(uint32_t i = 0; i < count; i++) {
            subtype->dump(data);
            data += subtype->size;
            if(i < count - 1) {
                std::cout << ", ";
            }
        }
        std::cout << ">";
    }
};

// Represents a vector type.
struct TypeArray : public Type
{
    // Type of each element.
    std::shared_ptr<Type> subtype;

    // Type of the element. This is a key in the "types" map.
    uint32_t type;

    // Number of elements.
    uint32_t count;

    TypeArray(std::shared_ptr<Type> subtype, uint32_t type, uint32_t count)
        : Type(subtype->size*count), type(type), count(count) {}
    virtual uint32_t op() const { return SpvOpTypeArray; }
    virtual ConstituentInfo getConstituentInfo(int i) const {
        return { type, i*subtype->size };
    }
    virtual uint32_t getSubtypeSize() const {
        return subtype->size;
    }
    virtual void dump(unsigned char *data) const {
        std::cout << "[";
        for(uint32_t i = 0; i < count; i++) {
            subtype->dump(data);
            data += subtype->size;
            if(i < count - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "]";
    }
};

// Represents a matrix type. Each matrix is a sequence of column vectors.
// Matrix data is stored starting at the upper-left, going down the first
// column, then the next column, etc.
struct TypeMatrix : public Type
{
    // Type of each column.
    std::shared_ptr<Type> subtype;

    // Type of the column vector. This is a key in the "types" map.
    uint32_t columnType;

    // Number of columns.
    uint32_t columnCount;

    TypeMatrix(std::shared_ptr<Type> subtype, uint32_t columnType, uint32_t columnCount)
        : Type(subtype->size*columnCount), subtype(subtype),
        columnType(columnType), columnCount(columnCount) {}
    virtual uint32_t op() const { return SpvOpTypeMatrix; }
    virtual ConstituentInfo getConstituentInfo(int i) const {
        return { columnType, subtype->size*i };
    }
    virtual uint32_t getSubtypeSize() const {
        return subtype->size;
    }
    virtual void dump(unsigned char *data) const {
        std::cout << "<";
        for(uint32_t i = 0; i < columnCount; i++) {
            subtype->dump(data);
            data += subtype->size;
            if(i < columnCount - 1) {
                std::cout << ", ";
            }
        }
        std::cout << ">";
    }

    // Converts a row,col pair to a single index of the element.
    int getIndex(int row, int col, const TypeVector *typeVector) const {
        // The math here is arbitrary, we just need to do it consistently throughout.
        return col*typeVector->count + row;
    }

    // Converts an index to a row,col pair.
    void getRowAndCol(int index, int &row, int &col, const TypeVector *typeVector) const {
        // The math here is arbitrary, we just need to do it consistently throughout.
        row = index % typeVector->count;
        col = index / typeVector->count;
    }
};

// Represents a function type.
struct TypeFunction : public Type
{
    // Type of the return value. This is a key in the "types" map.
    uint32_t returnType;

    // Type of each parameter, in order from left to right. These are keys in the "types" map.
    std::vector<uint32_t> parameterTypes;

    TypeFunction(uint32_t returnType, const std::vector<uint32_t> &parameterTypes)
        : Type(4 /* XXX */), returnType(returnType), parameterTypes(parameterTypes) {}
    virtual uint32_t op() const { return SpvOpTypeFunction; }
    virtual void dump(unsigned char *data) const {
        std::cout << "function";
    }
};

// Represents a struct type.
struct TypeStruct : public Type
{
    // Type of each element, in order.
    std::vector<std::shared_ptr<Type>> memberTypes;

    // Type of each element, in order. These are keys in the "types" map.
    std::vector<uint32_t> memberTypeIds;

    TypeStruct(const std::vector<std::shared_ptr<Type>> &memberTypes,
            const std::vector<uint32_t> &memberTypeIds)
        : Type(computeSize(memberTypes)), memberTypes(memberTypes), memberTypeIds(memberTypeIds) {}
    virtual uint32_t op() const { return SpvOpTypeStruct; }
    virtual ConstituentInfo getConstituentInfo(int i) const {
        size_t offset = 0;
        for(int j = 0; j < i; j++) {
            offset += memberTypes[j]->size;
        }
        return { memberTypeIds[i], offset };
    }
    virtual void dump(unsigned char *data) const {
        std::cout << "{";
        for(size_t i = 0; i < memberTypes.size(); i++) {
            memberTypes[i]->dump(data);
            data += memberTypes[i]->size;
            if(i < memberTypes.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "}";
    }
    static size_t computeSize(const std::vector<std::shared_ptr<Type>> &memberTypes) {
        size_t size = 0;
        for(auto &t: memberTypes) {
            size += t->size;
        }
        return size;
    }
};

// Represents a pointer type.
struct TypePointer : public Type
{
    // Type pointer is pointing to.
    std::shared_ptr<Type> subtype;

    // Type of the data being pointed to. This is a key in the "types" map.
    uint32_t type;

    // Storage class of the data. One of the values of SpvStorageClass (input,
    // output, uniform, function, etc.).
    uint32_t storageClass;

    TypePointer(std::shared_ptr<Type> subtype, uint32_t type, uint32_t storageClass)
        : Type(sizeof(uint32_t)), subtype(subtype), type(type), storageClass(storageClass) {}
    virtual uint32_t op() const { return SpvOpTypePointer; }
    virtual void dump(unsigned char *data) const {
        std::cout << "(ptr)" << objectAt<uint32_t>(data);
    }
};

// Represents an image type.
struct TypeImage : public Type
{
    // XXX document these.
    uint32_t sampledType;
    uint32_t dim;
    uint32_t depth;
    uint32_t arrayed;
    uint32_t ms;
    uint32_t sampled;
    uint32_t imageFormat;
    uint32_t accessQualifier;

    TypeImage(uint32_t sampledType, uint32_t dim, uint32_t depth, uint32_t arrayed, uint32_t ms, uint32_t sampled, uint32_t imageFormat, uint32_t accessQualifier)
        : Type(sizeof(uint32_t) /* XXX */),
        sampledType(sampledType),
        dim(dim),
        depth(depth),
        arrayed(arrayed),
        ms(ms),
        sampled(sampled),
        imageFormat(imageFormat),
        accessQualifier(accessQualifier) {}
    virtual uint32_t op() const { return SpvOpTypeImage; }
    virtual void dump(unsigned char *data) const {
        std::cout << "image";
    }
};

// XXX Dunno.
struct TypeSampledImage : public Type
{
    // Type of the image. This is a key in the "types" map. XXX yes?
    uint32_t imageType;

    TypeSampledImage(uint32_t imageType)
        : Type(sizeof(uint32_t)), imageType(imageType) {}
    virtual uint32_t op() const { return SpvOpTypeSampledImage; }
    virtual void dump(unsigned char *data) const {
        std::cout << "sampledimage";
    }
};

// A function parameter. This is on the receiving side, to set aside an SSA slot
// for the value received from the caller.
struct FunctionParameter
{
    // Type of the parameter. This is a key in the "types" map.
    uint32_t type;

    // Name of the parameter. This is a key in the "names" array.
    uint32_t id;
};

struct Pointer
{
    uint32_t type;
    uint32_t storageClass;
    size_t address; // address in memory
};

// SSA (virtual) register.
struct Register
{
    uint32_t type;
    size_t size;
    unsigned char *data;
    bool initialized;

    // For ConstantComposite, the IDs of the sub-elements.
    std::vector<uint32_t> subelements;

    Register(const Register &other)
    {
        if(false)std::cout << "move ctor allocated to " << this << " as " << &data << "\n";
        type = other.type;
        size = other.size;
        data = new unsigned char[size];
        std::copy(other.data, other.data + size, data);
        initialized = other.initialized;
        subelements = other.subelements;
    }

    Register(uint32_t type_, size_t size_) :
        type(type_),
        size(size_),
        data(new unsigned char[size_]),
        initialized(false)
    {
        if(false)std::cout << "ctor allocated to " << this << " as " << &data << "\n";
    }

    Register() :
        type(0xFFFFFFFF),
        size(0),
        data(nullptr),
        initialized(false)
    {
        if(false)std::cout << "ctor empty Register " << this << " \n";
    }

    ~Register()
    {
        if(false)std::cout << "dtor deleting from " << this << " as " << &data << "\n";
        delete[] data;
    }

    Register& operator=(const Register &other)
    {
        if(false)std::cout << "operator=\n";
        if(this != &other) {
            if(false)std::cout << "op= deleting from " << this << " as " << &data << "\n";
            delete[] data;
            type = other.type;
            size = other.size;
            data = new unsigned char[size];
            if(false)std::cout << "op= allocated to " << this << " as " << &data << "\n";
            std::copy(other.data, other.data + size, data);
            initialized = other.initialized;
            subelements = other.subelements;
        }
        return *this;
    }
};

struct Interpreter;
struct Compiler;
struct InstructionList;
struct Block;
struct Program;
struct Function;
struct RiscVPhi;

// Base class for individual instructions.
struct Instruction {
    Instruction(const LineInfo& lineInfo)
        : list(nullptr), lineInfo(lineInfo), needLiveness(false) {

        // Nothing.
    }
    virtual ~Instruction() {}

    // Back pointer.
    InstructionList *list;

    // Linked list pointers.
    std::shared_ptr<Instruction> next;
    std::shared_ptr<Instruction> prev;

    // Source line information
    LineInfo lineInfo;

    // Set of registers affected by instruction.
    std::set<uint32_t> resIdSet;

    // List of registers affected by instruction.
    std::vector<uint32_t> resIdList;

    // Set of registers that are inputs to the instruction.
    std::set<uint32_t> argIdSet;

    // List of registers that are inputs to the instruction.
    std::vector<uint32_t> argIdList;

    // Label IDs we might branch to.
    std::set<uint32_t> targetLabelIds;

    // Registers that are live going into this instruction. The key is
    // the block it's coming from, or 0 to mean "any".
    std::map<uint32_t,std::set<uint32_t>> livein;

    // Registers that are live leaving this instruction.
    std::set<uint32_t> liveout;

    // Whether we need to recompute liveness for this instruction.
    bool needLiveness;

    // Step the interpreter forward one instruction.
    virtual void step(Interpreter *interpreter) = 0;

    // Emit compiler output for this instruction.
    virtual void emit(Compiler *compiler);

    // The opcode of this instruction (e.g., SpvOpFMul).
    virtual uint32_t opcode() const = 0;

    // The name of this instruction (e.g., "OpFMul").
    virtual std::string name() const = 0;

    // Whether this is a branch instruction (OpBranch, OpBranchConditional,
    // OpSwitch, OpReturn, or OpReturnValue).
    virtual bool isBranch() const { return false; }

    // Whether this is a termination instruction (branch instruction, OpKill,
    // OpUnreachable).
    virtual bool isTermination() const { return false; }

    // Which block this instruction is in.
    uint32_t blockId() const;

    // Add a result to the instruction.
    void addResult(uint32_t id) {
        resIdSet.insert(id);
        resIdList.push_back(id);
    }

    // Add an input parameter to the instruction.
    void addParameter(uint32_t id) {
        argIdSet.insert(id);
        argIdList.push_back(id);
    }

    std::vector<Instruction *> succ() const;
    std::vector<Instruction *> pred() const;

    // String representation of this instruction (block and offset).
    std::string pos() const;

    // Whether the instruction modifies the register.
    bool affectsRegister(uint32_t regId) const {
        return resIdSet.find(regId) != resIdSet.end();
    }

    // Whether the instruction uses the register as a parameter.
    bool usesRegister(uint32_t regId) const {
        return argIdSet.find(regId) != argIdSet.end();
    }

    // Change the use of the argument. Asserts if the oldRegId
    // isn't currently being used.
    void changeArg(uint32_t oldRegId, uint32_t newRegId) {
        assert(usesRegister(oldRegId));
        argIdSet.erase(oldRegId);
        argIdSet.insert(newRegId);

        int foundCount = 0;
        for (auto itr = argIdList.begin(); itr != argIdList.end(); ++itr) {
            if (*itr == oldRegId) {
                *itr = newRegId;
                foundCount++;
            }
        }
        assert(foundCount > 0);
    }

    // Dump a rough disassembly to stdout.
    void dump(std::ostream &out) const;
};

// A doubly-linked list of Instruction objects.
struct InstructionList {
    Block *block;
    std::shared_ptr<Instruction> head;
    std::shared_ptr<Instruction> tail;

    InstructionList(Block *block) : block(block) {
        // Nothing.
    }

    ~InstructionList() {
        while (head) {
            erase(head);
        }
    }

    // The number of instructions in the list. Takes O(n).
    // XXX Not tested.
    size_t size() const {
        size_t s = 0;

        for (auto inst = head; inst; inst = inst->next) {
            s++;
        }

        // Sanity check.
        size_t s_backward = 0;
        for (auto inst = tail; inst; inst = inst->prev) {
            s_backward++;
        }
        assert(s == s_backward);

        return s;
    }

    // Add an instruction to the end of this list.
    void push_back(std::shared_ptr<Instruction> instruction) {
        prepareForAdd(instruction);

        if (head) {
            // Add to end of list.
            tail->next = instruction;
            instruction->prev = tail;
        } else {
            // Starting with empty list.
            head = instruction;
        }

        tail = instruction;
    }

    // Add an instruction before the other instruction.
    void insert(std::shared_ptr<Instruction> instruction, std::shared_ptr<Instruction> other) {
        prepareForAdd(instruction);

        // If the list is not empty, must specify item to insert in front of. Otherwise
        // it's okay to pass an empty element (like head to insert at the front).
        assert(!other == !head);

        if (other) {
            // Other item must be in this list.
            assert(other->list == this);

            // Keep track of previous item.
            std::shared_ptr<Instruction> prevItem = other->prev;

            // Pair between instruction and other.
            other->prev = instruction;
            instruction->next = other;

            // Pair between instruction and previous.
            instruction->prev = prevItem;
            if (prevItem) {
                prevItem->next = instruction;
            } else {
                assert(head == other);
                head = instruction;
            }
        } else {
            // Starting with empty list.
            head = instruction;
            tail = instruction;
            instruction->prev.reset();
            instruction->next.reset();
        }
    }

    void erase(std::shared_ptr<Instruction> instruction) {
        assert(instruction->list == this);
        if (instruction->prev) {
            instruction->prev->next = instruction->next;
        } else {
            assert(head == instruction);
            head = instruction->next;
        }
        if (instruction->next) {
            instruction->next->prev = instruction->prev;
        } else {
            assert(tail == instruction);
            tail = instruction->prev;
        }
        instruction->prev.reset();
        instruction->next.reset();
        instruction->list = nullptr;
    }

    // Swap the two instruction lists.
    void swap(InstructionList &other) {
        // Don't swap the "block" pointer, that stays fixed with whatever
        // block we're a part of.
        std::swap(head, other.head);
        std::swap(tail, other.tail);

        fixUpBackPointers();
        other.fixUpBackPointers();
    }

    // Set the instructions' "list" pointer back to us.
    void fixUpBackPointers() {
        for (auto inst = head; inst; inst = inst->next) {
            inst->list = this;
        }
    }

private:
    // Prepare an item for adding (insertion or appending).
    void prepareForAdd(std::shared_ptr<Instruction> instruction) {
        if (instruction->list != nullptr) {
            instruction->list->erase(instruction);
        }

        assert(instruction->list == nullptr);
        assert(!instruction->prev);
        assert(!instruction->next);

        // Set up back pointer.
        instruction->list = this;
    }
};

// A block is a sequence of instructions that has one entry point
// (the first instruction) and one exit point (the last instruction).
// The last instruction must be a variant of a branch.
struct Block {
    Block(uint32_t blockId, Function *function)
        : instructions(this), function(function), blockId(blockId), idom(NO_BLOCK_ID) {
        // Nothing.
    }

    // Instructions in this block.
    InstructionList instructions;

    // Back pointer.
    Function *function;

    // ID of label that points to first instruction.
    uint32_t blockId;

    // Predecessor blocks. This is only empty for the first block in each function.
    std::set<uint32_t> pred;

    // Successor blocks.
    std::set<uint32_t> succ;

    // Block IDs that dominate this block.
    std::set<uint32_t> dom;

    // Immediate dominator of this block, or NO_BLOCK_ID if this is the start block.
    uint32_t idom;

    // Children in idom tree.
    std::vector<std::shared_ptr<Block>> idomChildren;

    // Whether this block is dominated by the other block.
    bool isDominatedBy(uint32_t other) const {
        return dom.find(other) != dom.end();
    }
};

struct CommandLineParameters
{
    int outputWidth;
    int outputHeight;
    bool beVerbose;
    bool throwOnUnimplemented;
};

std::string readFileContents(std::string shaderFileName);

#endif // BASIC_TYPES_H
