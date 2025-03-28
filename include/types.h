#ifndef TYPES_H
#define TYPES_H

#include <systemc>
#include <cstdint>

namespace riscv {
    // 基本数据类型 - 使用SystemC数据类型
    using Byte = sc_dt::sc_uint<8>;
    using Word = sc_dt::sc_uint<32>;
    using RegAddr = sc_dt::sc_uint<5>;  // 正确使用5-bit寄存器地址
    
    // 标准C++类型 - 用于联合体
    using CppByte = uint8_t;
    using CppWord = uint32_t;
    
    // SystemC信号类型
    using WordSignal = sc_core::sc_signal<Word>;
    using ByteSignal = sc_core::sc_signal<Byte>;
    using BoolSignal = sc_core::sc_signal<bool>;
    using RegAddrSignal = sc_core::sc_signal<RegAddr>;
    
    // 指令格式 - 保留位域结构，方便指令解码
    struct RTypeInst {
        unsigned int opcode : 7;
        unsigned int rd     : 5;
        unsigned int funct3 : 3;
        unsigned int rs1    : 5;
        unsigned int rs2    : 5;
        unsigned int funct7 : 7;
    };
    
    struct ITypeInst {
        unsigned int opcode : 7;
        unsigned int rd     : 5;
        unsigned int funct3 : 3;
        unsigned int rs1    : 5;
        unsigned int imm    : 12;
    };
    
    struct STypeInst {
        unsigned int opcode : 7;
        unsigned int imm1   : 5;
        unsigned int funct3 : 3;
        unsigned int rs1    : 5;
        unsigned int rs2    : 5;
        unsigned int imm2   : 7;
    };
    
    struct UTypeInst {
        unsigned int opcode : 7;
        unsigned int rd     : 5;
        unsigned int imm    : 20;
    };
    
    // 指令联合体，使用标准C++类型避免构造/析构函数问题
    union Instruction {
        CppWord value;
        RTypeInst r;
        ITypeInst i;
        STypeInst s;
        UTypeInst u;
    };
}

#endif // TYPES_H
