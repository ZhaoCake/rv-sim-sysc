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
    
    // Add trace method for waveform generation
    void trace(sc_core::sc_trace_file* tf) {
        if (!tf) return;
        
        // CPU signals
        sc_core::sc_trace(tf, pc_fetch_to_decode, "pc");
        sc_core::sc_trace(tf, instruction_fetch_to_decode, "instruction");
        sc_core::sc_trace(tf, branch_taken_execute_to_fetch, "branch_taken");
        sc_core::sc_trace(tf, branch_target_execute_to_fetch, "branch_target");
        sc_core::sc_trace(tf, stall, "stall");
        
        // Register access signals
        sc_core::sc_trace(tf, rs1_addr_decode_to_regfile, "rs1_addr");
        sc_core::sc_trace(tf, rs2_addr_decode_to_regfile, "rs2_addr");
        sc_core::sc_trace(tf, rd_addr_decode_to_regfile, "rd_addr");
        sc_core::sc_trace(tf, rs1_data_regfile_to_decode, "rs1_data");
        sc_core::sc_trace(tf, rs2_data_regfile_to_decode, "rs2_data");
        sc_core::sc_trace(tf, reg_write_decode_to_regfile, "reg_write_en");
        sc_core::sc_trace(tf, alu_result_execute_to_memory, "reg_write_data");
        
        // ALU signals
        sc_core::sc_trace(tf, alu_op_decode_to_execute, "alu_op");
        sc_core::sc_trace(tf, operand1_decode_to_execute, "alu_operand1");
        sc_core::sc_trace(tf, operand2_decode_to_execute, "alu_operand2");
        
        // Memory access signals
        sc_core::sc_trace(tf, mem_addr, "mem_addr");
        sc_core::sc_trace(tf, mem_data_read, "mem_data_read");
        sc_core::sc_trace(tf, mem_data_write, "mem_data_write");
        sc_core::sc_trace(tf, mem_read_enable, "mem_read_en");
        sc_core::sc_trace(tf, mem_write_enable, "mem_write_en");
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
    
    // 分离指令获取和数据访问的内存读信号
    BoolSignal mem_instr_read_enable;  // 新增：指令内存读信号
    BoolSignal mem_data_read_enable;   // 新增：数据内存读信号
    WordSignal mem_instr_addr;         // 新增：指令内存地址
    WordSignal mem_data_addr;          // 新增：数据内存地址
    
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
