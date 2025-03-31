#include "Top.h"
#include <iostream>
#include <iomanip>

namespace riscv {

Top::Top(sc_core::sc_module_name name) : sc_module(name) {
    // Create module instances
    fetch = new Fetch("fetch");
    decode = new Decode("decode");
    execute = new Execute("execute");
    register_file = new RegisterFile("register_file");
    memory = new Memory("memory");
    
    // Connect Fetch module
    fetch->clk(clk);
    fetch->reset(reset);
    fetch->stall(stall_sig);
    fetch->branch_taken(branch_taken_sig);
    fetch->branch_target(branch_target_sig);
    fetch->pc(pc_sig);
    fetch->next_pc(next_pc_sig);
    fetch->mem_read(rom_read_sig);
    fetch->mem_addr(rom_address_sig);
    fetch->instruction(rom_data_sig);
    fetch->instr_out(instruction_sig);
    
    // Connect Decode module
    decode->clk(clk);
    decode->reset(reset);
    decode->instruction(instruction_sig);
    decode->pc(pc_sig);
    decode->rs1_addr(rs1_addr_sig);
    decode->rs1_data(rs1_data_sig);
    decode->rs2_addr(rs2_addr_sig);
    decode->rs2_data(rs2_data_sig);
    decode->imm(imm_sig);
    decode->rd_addr(rd_addr_sig);
    decode->rs1_value(rs1_value_sig);
    decode->rs2_value(rs2_value_sig);
    decode->reg_write(reg_write_sig);
    decode->mem_read(mem_read_sig);
    decode->mem_write(mem_write_sig);
    decode->alu_op(alu_op_sig);
    decode->alu_src(alu_src_sig);
    decode->mem_to_reg(mem_to_reg_sig);
    decode->branch(branch_sig);
    decode->jump(jump_sig);
    
    // Connect Execute module
    execute->clk(clk);
    execute->reset(reset);
    execute->pc(pc_sig);
    execute->rs1_value(rs1_value_sig);
    execute->rs2_value(rs2_value_sig);
    execute->imm(imm_sig);
    execute->rd_addr(rd_addr_sig);
    execute->alu_op(alu_op_sig);
    execute->alu_src(alu_src_sig);
    execute->branch(branch_sig);
    execute->jump(jump_sig);
    execute->mem_read(mem_read_sig);
    execute->mem_write(mem_write_sig);
    execute->reg_write(reg_write_sig);
    execute->mem_to_reg(mem_to_reg_sig);
    execute->alu_result(alu_result_sig);
    execute->branch_taken(branch_taken_sig);
    execute->branch_target(branch_target_sig);
    execute->mem_write_data(mem_write_data_sig);
    execute->mem_read_out(mem_read_out_sig);
    execute->mem_write_out(mem_write_out_sig);
    execute->reg_write_out(reg_write_out_sig);
    execute->mem_to_reg_out(mem_to_reg_out_sig);
    execute->rd_addr_out(rd_addr_out_sig);
    
    // Connect RegisterFile module
    register_file->clk(clk);
    register_file->write_enable(reg_write_wb_sig);
    register_file->rs1_addr(rs1_addr_sig);
    register_file->rs1_data(rs1_data_sig);
    register_file->rs2_addr(rs2_addr_sig);
    register_file->rs2_data(rs2_data_sig);
    register_file->rd_addr(rd_addr_wb_sig);
    register_file->rd_data(rd_data_wb_sig);
    
    // Connect Memory module
    memory->rom_read(rom_read_sig);
    memory->rom_address(rom_address_sig);
    memory->rom_data(rom_data_sig);
    memory->ram_read(ram_read_sig);
    memory->ram_write(ram_write_sig);
    memory->ram_address(ram_address_sig);
    memory->ram_write_data(ram_write_data_sig);
    memory->ram_read_data(ram_read_data_sig);
    
    // Initialize stall signal to false
    stall_sig.write(false);
    
    // Register processes
    SC_METHOD(memory_access_process);
    sensitive << alu_result_sig << mem_write_data_sig << mem_read_out_sig << mem_write_out_sig;
    
    SC_METHOD(writeback_process);
    sensitive << reg_write_out_sig << mem_to_reg_out_sig << rd_addr_out_sig << alu_result_sig << ram_read_data_sig;
    
    SC_METHOD(debug_process);
    sensitive << clk.pos();
    
    std::cout << "Top module initialized" << std::endl;
}

Top::~Top() {
    delete fetch;
    delete decode;
    delete execute;
    delete register_file;
    delete memory;
}

void Top::memory_access_process() {
    // Forward signals from Execute to Memory
    ram_address_sig.write(alu_result_sig.read());
    ram_write_data_sig.write(mem_write_data_sig.read());
    ram_read_sig.write(mem_read_out_sig.read());
    ram_write_sig.write(mem_write_out_sig.read());
    
    if (mem_read_out_sig.read() || mem_write_out_sig.read()) {
        std::cout << "Memory Access: " 
                  << (mem_read_out_sig.read() ? "Read from " : "Write to ")
                  << "address 0x" << std::hex << ram_address_sig.read() << std::dec;
        
        if (mem_write_out_sig.read()) {
            std::cout << " value 0x" << std::hex << ram_write_data_sig.read() << std::dec;
        }
        
        std::cout << std::endl;
    }
}

void Top::writeback_process() {
    // Select data to write back to register file
    if (reg_write_out_sig.read()) {
        // Set the register write signals
        reg_write_wb_sig.write(true);
        rd_addr_wb_sig.write(rd_addr_out_sig.read());
        
        if (mem_to_reg_out_sig.read()) {
            // Write back data from memory
            rd_data_wb_sig.write(ram_read_data_sig.read());
            std::cout << "Writeback: from memory, data=0x" << std::hex 
                      << ram_read_data_sig.read() << std::dec << std::endl;
        } else {
            // Write back ALU result
            rd_data_wb_sig.write(alu_result_sig.read());
            std::cout << "Writeback: from ALU, data=0x" << std::hex 
                      << alu_result_sig.read() << std::dec << std::endl;
        }
    } else {
        reg_write_wb_sig.write(false);
    }
}

void Top::debug_process() {
    // Periodic debug information
    static int cycle_count = 0;
    
    if (clk.read()) {
        std::cout << "======== Cycle " << ++cycle_count << " ========" << std::endl;
    }
}

bool Top::load_program(const std::string& filename) {
    return memory->load_binary(filename);
}

void Top::dump_registers() const {
    register_file->dump_registers();
}

void Top::dump_memory(const std::string& filename, uint32_t start, uint32_t size) const {
    memory->dump_memory(filename, start, size);
}

} // namespace riscv