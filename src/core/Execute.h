/*
 * rv-sim-sysc - Instruction execution header
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

#ifndef EXECUTE_H
#define EXECUTE_H

#include <systemc>
#include "../../include/types.h"
#include "../../include/constants.h"

namespace riscv {

class Execute : public sc_core::sc_module {
public:
    // Clock and reset
    sc_core::sc_in<bool> clk;
    sc_core::sc_in<bool> reset;
    
    // Inputs from decode stage
    sc_core::sc_in<Word> pc;
    sc_core::sc_in<Word> rs1_value;
    sc_core::sc_in<Word> rs2_value;
    sc_core::sc_in<Word> imm;
    sc_core::sc_in<RegAddr> rd_addr;
    
    // Control signals from decode
    sc_core::sc_in<sc_dt::sc_uint<4>> alu_op;
    sc_core::sc_in<bool> alu_src;
    sc_core::sc_in<bool> branch;
    sc_core::sc_in<bool> jump;
    sc_core::sc_in<bool> mem_read;
    sc_core::sc_in<bool> mem_write;
    sc_core::sc_in<bool> reg_write;
    sc_core::sc_in<bool> mem_to_reg;
    
    // Outputs
    sc_core::sc_out<Word> alu_result;
    sc_core::sc_out<bool> branch_taken;
    sc_core::sc_out<Word> branch_target;
    sc_core::sc_out<Word> mem_write_data;
    
    // Forwarded control signals to memory/writeback
    sc_core::sc_out<bool> mem_read_out;
    sc_core::sc_out<bool> mem_write_out;
    sc_core::sc_out<bool> reg_write_out;
    sc_core::sc_out<bool> mem_to_reg_out;
    sc_core::sc_out<RegAddr> rd_addr_out;

    // Constructor
    SC_HAS_PROCESS(Execute);
    Execute(sc_core::sc_module_name name);
    
    // Destructor
    ~Execute();
    
private:
    // Process methods
    void execute_process();
    
    // ALU function
    Word alu_compute(Word a, Word b, sc_dt::sc_uint<4> op);
    
    // Branch evaluation
    bool evaluate_branch(Word a, Word b, uint32_t funct3);
};

} // namespace riscv

#endif // EXECUTE_H