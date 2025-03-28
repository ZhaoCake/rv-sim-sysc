#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace riscv {
    // RISC-V操作码定义
    constexpr unsigned int OP_LUI    = 0b0110111;
    constexpr unsigned int OP_AUIPC  = 0b0010111;
    constexpr unsigned int OP_JAL    = 0b1101111;
    constexpr unsigned int OP_JALR   = 0b1100111;
    constexpr unsigned int OP_BRANCH = 0b1100011;
    constexpr unsigned int OP_LOAD   = 0b0000011;
    constexpr unsigned int OP_STORE  = 0b0100011;
    constexpr unsigned int OP_IMM    = 0b0010011;
    constexpr unsigned int OP_REG    = 0b0110011;
    
    // 指令功能码定义
    constexpr unsigned int FUNCT3_BEQ  = 0b000;
    constexpr unsigned int FUNCT3_BNE  = 0b001;
    constexpr unsigned int FUNCT3_BLT  = 0b100;
    constexpr unsigned int FUNCT3_BGE  = 0b101;
    constexpr unsigned int FUNCT3_BLTU = 0b110;
    constexpr unsigned int FUNCT3_BGEU = 0b111;
    
    // ALU操作
    constexpr unsigned int ALU_ADD  = 0;
    constexpr unsigned int ALU_SUB  = 1;
    constexpr unsigned int ALU_AND  = 2;
    constexpr unsigned int ALU_OR   = 3;
    constexpr unsigned int ALU_XOR  = 4;
    constexpr unsigned int ALU_SLL  = 5;
    constexpr unsigned int ALU_SRL  = 6;
    constexpr unsigned int ALU_SRA  = 7;
    constexpr unsigned int ALU_SLT  = 8;
    constexpr unsigned int ALU_SLTU = 9;
    
    // 存储器大小
    constexpr unsigned int MEM_SIZE = 1024 * 1024; // 1MB
    constexpr unsigned int REG_COUNT = 32;
}

#endif // CONSTANTS_H
