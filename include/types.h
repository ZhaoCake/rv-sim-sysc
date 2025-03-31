#ifndef TYPES_H
#define TYPES_H

#include <systemc>
#include <cstdint>

namespace riscv {

// Word type definition (32-bit)
using Word = sc_dt::sc_uint<32>;

// Register address type (5-bit)
using RegAddr = sc_dt::sc_uint<5>;

// Signal types for easier reference
using WordSignal = sc_core::sc_signal<Word>;
using BoolSignal = sc_core::sc_signal<bool>;
using RegAddrSignal = sc_core::sc_signal<RegAddr>;

// Instruction formats as per RISC-V spec
struct RType {
    uint32_t opcode : 7;
    uint32_t rd : 5;
    uint32_t funct3 : 3;
    uint32_t rs1 : 5;
    uint32_t rs2 : 5;
    uint32_t funct7 : 7;
};

struct IType {
    uint32_t opcode : 7;
    uint32_t rd : 5;
    uint32_t funct3 : 3;
    uint32_t rs1 : 5;
    uint32_t imm : 12;
};

struct SType {
    uint32_t opcode : 7;
    uint32_t imm0 : 5;
    uint32_t funct3 : 3;
    uint32_t rs1 : 5;
    uint32_t rs2 : 5;
    uint32_t imm1 : 7;
};

struct BType {
    uint32_t opcode : 7;
    uint32_t imm0 : 5;
    uint32_t funct3 : 3;
    uint32_t rs1 : 5;
    uint32_t rs2 : 5;
    uint32_t imm1 : 7;
};

struct UType {
    uint32_t opcode : 7;
    uint32_t rd : 5;
    uint32_t imm : 20;
};

struct JType {
    uint32_t opcode : 7;
    uint32_t rd : 5;
    uint32_t imm : 20;
};

// Union for easier access to different instruction formats
union Instruction {
    uint32_t value;
    RType r;
    IType i;
    SType s;
    BType b;
    UType u;
    JType j;
};

} // namespace riscv

#endif // TYPES_H
