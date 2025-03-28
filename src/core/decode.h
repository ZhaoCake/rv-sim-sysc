#ifndef DECODE_H
#define DECODE_H

#include <systemc>
#include "../../include/types.h"
#include "../../include/constants.h"

namespace riscv {

class Decode : public sc_core::sc_module {
public:
    // Ports
    sc_core::sc_in<bool> clk;
    sc_core::sc_in<bool> reset;
    sc_core::sc_in<Word> instruction;
    sc_core::sc_in<Word> pc;
    
    // 寄存器文件接口，使用RegAddr类型
    sc_core::sc_out<RegAddr> rs1_addr;
    sc_core::sc_out<RegAddr> rs2_addr;
    sc_core::sc_in<Word> rs1_data;
    sc_core::sc_in<Word> rs2_data;
    
    // 控制信号输出，使用RegAddr类型
    sc_core::sc_out<RegAddr> rd_addr;
    sc_core::sc_out<sc_dt::sc_uint<4>> alu_op; // 4位足够表示ALU操作
    sc_core::sc_out<Word> operand1;
    sc_core::sc_out<Word> operand2;
    sc_core::sc_out<bool> mem_read;
    sc_core::sc_out<bool> mem_write;
    sc_core::sc_out<bool> reg_write;
    sc_core::sc_out<bool> is_branch;
    sc_core::sc_out<Word> imm_value;
    
    // Constructor
    SC_HAS_PROCESS(Decode);
    Decode(sc_core::sc_module_name name);
    
private:
    void decode_process();
    Word sign_extend(Word value, int bits);
};

} // namespace riscv

#endif // DECODE_H
