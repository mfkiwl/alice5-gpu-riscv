#include <algorithm>
#include <cstdint>

struct GPUCore
{
    // Loosely modeled on RISC-V RVI, RVA, RVM, RVF

    uint32_t x[32]; // but 0 not accessed because it's readonly 0
    uint32_t pc;
    // XXX CSRs
    // XXX FCSRs

    GPUCore()
    {
        std::fill(x, x + 32, 0);
        pc = 0;
    }

    enum Status {RUNNING, BREAK, UNIMPLEMENTED_OPCODE};

    // Expected memory functions
    // uint8_t read8(uint32_t addr);
    // uint16_t read16(uint32_t addr);
    // uint32_t read32(uint32_t addr);
    // void write8(uint32_t addr, uint8_t v);
    // void write16(uint32_t addr, uint16_t v);
    // void write32(uint32_t addr, uint32_t v);
    
    template <class T>
    Status step(T& memory);

    template <class T>
    Status stepUntilException(T& memory)
    {
        Status status;
        while((status = step(memory)) == RUNNING);
        return status;
    }
};

uint32_t extendSign(uint32_t v, int width)
{
    uint32_t sign = v & (1 << (width - 1));
    uint32_t extended = 0 - sign;
    return v | extended;
}

uint32_t getBits(uint32_t v, int hi, int lo)
{
    return (v >> lo) & ((1u << (hi + 1 - lo)) - 1);
}

constexpr uint32_t makeOpcode(uint32_t bits14_12, uint32_t bits6_2, uint32_t bits1_0)
{
    return (bits14_12 << 12) | (bits6_2 << 2) | bits1_0;
}

const bool dump = false;

template <class T>
GPUCore::Status GPUCore::step(T& memory)
{
    uint32_t insn = memory.read32(pc);

    Status status = RUNNING;

    uint32_t rd = getBits(insn, 11, 7);
    uint32_t funct3 = getBits(insn, 14, 12);
    // uint32_t funct7 = getBits(insn, 31, 25);
    uint32_t rs1 = getBits(insn, 19, 15);
    uint32_t rs2 = getBits(insn, 24, 20);
    uint32_t immI = extendSign(getBits(insn, 31, 20), 12);
    uint32_t shamt = getBits(insn, 34, 20);
    uint32_t immS = extendSign(
        (getBits(insn, 31, 25) << 5) |
        getBits(insn, 11, 7),
        12);
    uint32_t immSB = extendSign((getBits(insn, 31, 31) << 12) |
        (getBits(insn, 7, 7) << 11) | 
        (getBits(insn, 30, 25) << 5) | 
        (getBits(insn, 11, 8) << 1),
        12);
    uint32_t immU = getBits(insn, 31, 12) << 12;
    uint32_t immUJ = extendSign((getBits(insn, 31, 31) << 20) |
        (getBits(insn, 19, 12) << 12) | 
        (getBits(insn, 20, 20) << 11) | 
        (getBits(insn, 30, 21) << 1),
        21);

    std::function<void(void)> unimpl = [&]() {
        std::cerr << "unimplemented instruction " << std::hex << std::setfill('0') << std::setw(8) << insn;
        std::cerr << " with 14..12=" << std::dec << std::setfill('0') << std::setw(1) << ((insn & 0x7000) >> 12);
        std::cerr << " and 6..2=0x" << std::hex << std::setfill('0') << std::setw(2) << ((insn & 0x7F) >> 2) << std::dec << '\n';
        status = UNIMPLEMENTED_OPCODE;
    };

    switch(insn & 0x707F) {
        case makeOpcode(0, 0x1C, 3): { // ebreak
            if(dump) std::cout << "ebreak\n";
            if(insn == 0x00100073) {
                status = BREAK;
            } else {
                unimpl();
            }
            break;
        }

        case makeOpcode(0, 0x08, 3):
        case makeOpcode(1, 0x08, 3):
        case makeOpcode(2, 0x08, 3):
        { // sb, sh, sw
            if(dump) std::cout << "sw\n";
            switch(funct3) {
                case 0: memory.write8(x[rs1] + immS, x[rs2] & 0xFF); break;
                case 1: memory.write16(x[rs1] + immS, x[rs2] & 0xFFFF); break;
                case 2: memory.write32(x[rs1] + immS, x[rs2]); break;
            }
            pc += 4;
            break;
        }

        case makeOpcode(0, 0x00, 3):
        case makeOpcode(1, 0x00, 3):
        case makeOpcode(2, 0x00, 3):
        case makeOpcode(4, 0x00, 3):
        case makeOpcode(5, 0x00, 3):
        { // lb, lh, lw, lbu, lhw
            if(dump) std::cout << "load\n";
            if(rd != 0) {
                switch(funct3) {
                    case 0: x[rd] = extendSign(memory.read8(x[rs1] + immI), 8); break;
                    case 1: x[rd] = extendSign(memory.read16(x[rs1] + immI), 16); break;
                    case 2: x[rd] = memory.read32(x[rs1] + immI); break;
                    case 4: x[rd] = memory.read8(x[rs1] + immI); break;
                    case 5: x[rd] = memory.read16(x[rs1] + immI); break;
                    default: unimpl();
                }
            }
            pc += 4;
            break;
        }

        case makeOpcode(0, 0x0D, 3):
        case makeOpcode(1, 0x0D, 3):
        case makeOpcode(2, 0x0D, 3):
        case makeOpcode(3, 0x0D, 3):
        case makeOpcode(4, 0x0D, 3):
        case makeOpcode(5, 0x0D, 3):
        case makeOpcode(6, 0x0D, 3):
        case makeOpcode(7, 0x0D, 3):
        { // lui
            if(dump) std::cout << "lui\n";
            if(rd > 0) {
                x[rd] = immU;
            }
            pc += 4;
            break;
        }
 
        case makeOpcode(0, 0x18, 3):
        case makeOpcode(1, 0x18, 3):
        case makeOpcode(4, 0x18, 3):
        case makeOpcode(5, 0x18, 3):
        case makeOpcode(6, 0x18, 3):
        case makeOpcode(7, 0x18, 3):
        { // beq, bne, blt, bge, bltu, bgeu
            if(dump) std::cout << "bge\n";
            switch(funct3) {
                case 0: pc += (x[rs1] == x[rs2]) ? immSB : 4; break;
                case 1: pc += (x[rs1] != x[rs2]) ? immSB : 4; break;
                case 4: pc += (static_cast<int32_t>(x[rs1]) < static_cast<int32_t>(x[rs2])) ? immSB : 4; break;
                case 5: pc += (static_cast<int32_t>(x[rs1]) >= static_cast<int32_t>(x[rs2])) ? immSB : 4; break;
                case 6: pc += (x[rs1] < x[rs2]) ? immSB : 4; break;
                case 7: pc += (x[rs1] >= x[rs2]) ? immSB : 4; break;
                default:
                    unimpl();
            }
            break;
        }

        case makeOpcode(0, 0x0C, 3): { // add
            if(dump) std::cout << "add\n";
            if(rd > 0) {
                x[rd] = x[rs1] + x[rs2];
            }
            pc += 4;
            break;
        }

        case makeOpcode(0, 0x1b, 3): { // jal
            if(dump) std::cout << "jal\n";
            if(rd > 0) {
                x[rd] = pc + 4;
            }
            pc += immUJ;
            break;
        }

        case makeOpcode(0, 0x04, 3):
        case makeOpcode(1, 0x04, 3):
        case makeOpcode(7, 0x04, 3):
        { // addi
            if(dump) std::cout << "addi\n";
            if(rd > 0) {
                switch(funct3) {
                    case 0: x[rd] = x[rs1] + immI; break;
                    case 1: x[rd] = x[rs1] << shamt; break;
                    case 4: x[rd] = x[rs1] ^ immI; break;
                    case 6: x[rd] = x[rs1] | immI; break;
                    case 7: x[rd] = x[rs1] & immI; break;
                }
            }
            pc += 4;
            break;
        }

        default: {
            unimpl();
        }
    }
    return status;
}

struct RunHeader
{
    // Little-endian
    uint32_t magic = 0x30354c41;        // 'AL50', version 0 of Alice 5 header
    uint32_t initialPC;                 // Initial value PC is set to
    uint32_t gl_FragCoordAddress;       // address of vec4 gl_FragCoord input
    uint32_t colorAddress;              // address of vec4 color output
    uint32_t iTimeAddress;              // address of float uniform
    uint32_t iMouseAddress;             // address of ivec4 uniform
    uint32_t iResolution;               // address of vec2 iResolution uniform
    // Bytes to follow are loaded at 0
};
