#include "fetch.h"
#include <iostream>
#include <iomanip>

namespace riscv {

Fetch::Fetch(sc_core::sc_module_name name) : sc_core::sc_module(name), pc(0) {
    SC_METHOD(fetch_process);
    sensitive << clk.pos();
    sensitive << reset;
}

void Fetch::fetch_process() {
    if (reset.read()) {
        pc = 0;
        pc_out.write(0);
        instruction_out.write(0);
        mem_read.write(false);
        return;
    }
    
    // On each clock, read the instruction at the current PC
    mem_addr.write(pc);
    mem_read.write(true);
    
    // Output the current PC and fetched instruction
    pc_out.write(pc);
    
    // Get instruction from memory
    Word instruction = mem_data->read();
    instruction_out.write(instruction);
    
    // Debug output
    std::cout << "Fetch: PC=0x" << std::hex << std::setw(8) << std::setfill('0') << pc.to_uint() 
              << ", Instr=0x" << std::setw(8) << instruction.to_uint() << std::dec << std::endl;
    
    // Update PC for next cycle
    if (!stall.read()) {
        if (branch_taken.read()) {
            Word next_pc = branch_target.read();
            std::cout << "Branch taken: jumping to 0x" << std::hex << next_pc.to_uint() << std::dec << std::endl;
            pc = next_pc;
        } else {
            pc = pc + 4;
        }
    }
}

} // namespace riscv
