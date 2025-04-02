/*
 * rv-sim-sysc - Instruction fetch header
 * Copyright (C) 2025 ZhaoCake
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef FETCH_H
#define FETCH_H

#include <systemc>
#include "../../include/types.h"

namespace riscv {

class Fetch : public sc_core::sc_module {
public:
    // Clock and reset
    sc_core::sc_in<bool> clk;
    sc_core::sc_in<bool> reset;
    
    // Control signals
    sc_core::sc_in<bool> stall;        // To stall the fetch stage
    sc_core::sc_in<bool> branch_taken; // Indicates a branch is taken
    sc_core::sc_in<Word> branch_target; // Target address for branch
    
    // Outputs
    sc_core::sc_out<Word> pc;          // Current program counter
    sc_core::sc_out<Word> next_pc;     // PC + 4 (for JAL/JALR instructions)
    sc_core::sc_out<bool> mem_read;    // Signal to memory for instruction read
    sc_core::sc_out<Word> mem_addr;    // Address to read from memory
    sc_core::sc_in<Word> instruction;  // Instruction fetched from memory
    sc_core::sc_out<Word> instr_out;   // Instruction output to decode stage

    // Constructor
    SC_HAS_PROCESS(Fetch);
    Fetch(sc_core::sc_module_name name);
    
    // Destructor
    ~Fetch();

private:
    // Program counter register
    Word pc_reg;
    
    // Process methods
    void fetch_process();
};

} // namespace riscv

#endif // FETCH_H