#ifndef TOP_H
#define TOP_H

#include <systemc>
#include "fetch.h"
#include "decode.h"
#include "execute.h"
#include "../memory/memory.h"
#include "../register/register_file.h"

namespace riscv {

class RiscvProcessor : public sc_core::sc_module {
public:
    // Ports
    sc_core::sc_in<bool> clk;
    sc_core::sc_in<bool> reset;
    
    // Constructor
    SC_HAS_PROCESS(RiscvProcessor);
    RiscvProcessor(sc_core::sc_module_name name);
    
    // Load program
    bool load_program(const std::string& filename) {
        return memory.load_program(filename);
    }
    
private:
    // 内部信号
    WordSignal pc_fetch_to_decode;
    WordSignal instruction_fetch_to_decode;
    WordSignal branch_target_execute_to_fetch;
    BoolSignal branch_taken_execute_to_fetch;
    BoolSignal stall;
    
    // 寄存器文件信号
    RegAddrSignal rs1_addr_decode_to_regfile;
    RegAddrSignal rs2_addr_decode_to_regfile;
    RegAddrSignal rd_addr_decode_to_regfile;
    WordSignal rs1_data_regfile_to_decode;
    WordSignal rs2_data_regfile_to_decode;
    BoolSignal reg_write_decode_to_regfile;
    
    // ALU信号
    sc_core::sc_signal<sc_dt::sc_uint<4>> alu_op_decode_to_execute;
    WordSignal operand1_decode_to_execute;
    WordSignal operand2_decode_to_execute;
    WordSignal imm_value_decode_to_execute;
    BoolSignal is_branch_decode_to_execute;
    WordSignal alu_result_execute_to_memory;
    
    // 内存信号
    WordSignal mem_addr;
    WordSignal mem_data_read;
    WordSignal mem_data_write;
    BoolSignal mem_read_enable;
    BoolSignal mem_write_enable;
    
    // 模块实例
    Fetch fetch;
    Decode decode;
    Execute execute;
    RegisterFile reg_file;
    Memory memory;
    
    // 过程
    void process();
};

} // namespace riscv

#endif // TOP_H
