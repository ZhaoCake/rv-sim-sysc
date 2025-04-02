/*
 * rv-sim-sysc - Instruction fetch implementation
 * Copyright (C) 2025 ZhaoCake
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "Fetch.h"
#include <iostream>

namespace riscv {

Fetch::Fetch(sc_core::sc_module_name name) : sc_module(name), pc_reg(0) {
    // Register processes
    SC_METHOD(fetch_process);
    sensitive << clk.pos() << reset;
    
    std::cout << "Fetch module initialized" << std::endl;
}

Fetch::~Fetch() {
    // Nothing to clean up
}

void Fetch::fetch_process() {
    if (reset.read()) {
        // Reset PC to 0
        pc_reg = 0;
        pc.write(pc_reg);
        next_pc.write(pc_reg + 4);
        mem_read.write(true);
        mem_addr.write(pc_reg);
        instr_out.write(0);  // NOP during reset
        
    } else if (clk.read()) {
        // If not stalled, update PC on clock edge
        if (!stall.read()) {
            if (branch_taken.read()) {
                // Branch is taken, update PC to branch target
                pc_reg = branch_target.read();
            } else {
                // Normal PC increment
                pc_reg = pc_reg + 4;
            }
            
            // Update outputs
            pc.write(pc_reg);
            next_pc.write(pc_reg + 4);
            
            // Request instruction from memory
            mem_read.write(true);
            mem_addr.write(pc_reg);
            
            // Forward the instruction to decode stage
            instr_out.write(instruction.read());
            
            std::cout << "Fetch: PC = 0x" << std::hex << pc_reg 
                      << ", Instruction = 0x" << instruction.read() << std::dec << std::endl;
        }
    }
}

} // namespace riscv