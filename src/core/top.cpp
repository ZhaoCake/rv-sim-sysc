#include "top.h"
#include <iostream>

namespace riscv {

RiscvProcessor::RiscvProcessor(sc_core::sc_module_name name) : 
    sc_core::sc_module(name),
    stall("stall", false),
    fetch("fetch"),
    decode("decode"),
    execute("execute"),
    reg_file("reg_file"),
    memory("memory")
{
    // 连接Fetch模块
    fetch.clk(clk);
    fetch.reset(reset);
    fetch.stall(stall);
    fetch.branch_taken(branch_taken_execute_to_fetch);
    fetch.branch_target(branch_target_execute_to_fetch);
    fetch.pc_out(pc_fetch_to_decode);
    fetch.instruction_out(instruction_fetch_to_decode);
    fetch.mem_data(mem_data_read);
    fetch.mem_addr(mem_instr_addr);       // 连接到指令地址
    fetch.mem_read(mem_instr_read_enable); // 连接到指令读信号
    
    // 连接Decode模块
    decode.clk(clk);
    decode.reset(reset);
    decode.instruction(instruction_fetch_to_decode);
    decode.pc(pc_fetch_to_decode);
    decode.rs1_addr(rs1_addr_decode_to_regfile);
    decode.rs2_addr(rs2_addr_decode_to_regfile);
    decode.rs1_data(rs1_data_regfile_to_decode);
    decode.rs2_data(rs2_data_regfile_to_decode);
    decode.rd_addr(rd_addr_decode_to_regfile);
    decode.alu_op(alu_op_decode_to_execute);
    decode.operand1(operand1_decode_to_execute);
    decode.operand2(operand2_decode_to_execute);
    decode.mem_read(mem_data_read_enable);  // 连接到数据读信号
    decode.mem_write(mem_write_enable);
    decode.reg_write(reg_write_decode_to_regfile);
    decode.is_branch(is_branch_decode_to_execute);
    decode.imm_value(imm_value_decode_to_execute);
    
    // 连接Execute模块
    execute.clk(clk);
    execute.reset(reset);
    execute.alu_op(alu_op_decode_to_execute);
    execute.operand1(operand1_decode_to_execute);
    execute.operand2(operand2_decode_to_execute);
    execute.pc(pc_fetch_to_decode);
    execute.imm_value(imm_value_decode_to_execute);
    execute.is_branch(is_branch_decode_to_execute);
    execute.alu_result(alu_result_execute_to_memory);
    execute.branch_taken(branch_taken_execute_to_fetch);
    execute.branch_target(branch_target_execute_to_fetch);
    
    // 连接RegisterFile模块
    reg_file.clk(clk);
    reg_file.reset(reset);
    reg_file.rs1_addr(rs1_addr_decode_to_regfile);
    reg_file.rs2_addr(rs2_addr_decode_to_regfile);
    reg_file.rs1_data(rs1_data_regfile_to_decode);
    reg_file.rs2_data(rs2_data_regfile_to_decode);
    reg_file.write_enable(reg_write_decode_to_regfile);
    reg_file.rd_addr(rd_addr_decode_to_regfile);
    reg_file.write_data(alu_result_execute_to_memory);
    
    // 连接Memory模块
    memory.clk(clk);
    memory.reset(reset);
    memory.address(mem_addr);
    memory.write_data(mem_data_write);
    memory.read_enable(mem_read_enable);
    memory.write_enable(mem_write_enable);
    memory.read_data(mem_data_read);
    
    SC_METHOD(process);
    sensitive << clk.pos();
    sensitive << mem_instr_read_enable << mem_data_read_enable;
    sensitive << mem_instr_addr << mem_data_addr;
    sensitive << alu_result_execute_to_memory;
}

void RiscvProcessor::process() {
    // 内存访问仲裁逻辑
    if (mem_data_read_enable.read()) {
        // 数据访问优先级高于指令获取
        mem_read_enable.write(true);
        mem_addr.write(alu_result_execute_to_memory.read()); // 使用ALU结果作为数据地址
        
        // 数据访问需要暂停指令获取
        stall.write(true);
        
        std::cout << "Data memory read access at address 0x" 
                  << std::hex << mem_addr.read().to_uint() << std::dec << std::endl;
    } 
    else if (mem_instr_read_enable.read()) {
        // 如果没有数据访问，处理指令获取
        mem_read_enable.write(true);
        mem_addr.write(mem_instr_addr.read()); // 使用PC作为指令地址
        stall.write(false);
        
        std::cout << "Instruction fetch at address 0x" 
                  << std::hex << mem_addr.read().to_uint() << std::dec << std::endl;
    } 
    else {
        // 没有内存访问
        mem_read_enable.write(false);
        stall.write(false);
    }
    
    // 处理内存写访问
    if (mem_write_enable.read()) {
        // 如果有数据写入请求，使用ALU结果作为地址
        mem_addr.write(alu_result_execute_to_memory.read());
        
        std::cout << "Data memory write access at address 0x" 
                  << std::hex << mem_addr.read().to_uint() << std::dec << std::endl;
    }
}

} // namespace riscv
