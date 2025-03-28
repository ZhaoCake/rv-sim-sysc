#include "top.h"

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
    fetch.mem_addr(mem_addr);
    fetch.mem_read(mem_read_enable);
    
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
    decode.mem_read(mem_read_enable);
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
}

void RiscvProcessor::process() {
    // 此方法用于处理可能的额外同步逻辑
}

} // namespace riscv
