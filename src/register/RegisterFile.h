/*
 * rv-sim-sysc - Register file header
 * Copyright (C) 2025 ZhaoCake
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef REGISTER_FILE_H
#define REGISTER_FILE_H

#include <systemc>
#include "../../include/types.h"
#include "../../include/constants.h"

namespace riscv {

class RegisterFile : public sc_core::sc_module {
public:
    // Input control signals
    sc_core::sc_in<bool> clk;
    sc_core::sc_in<bool> write_enable;
    
    // Read ports (2 for RISC-V)
    sc_core::sc_in<RegAddr> rs1_addr;
    sc_core::sc_out<Word> rs1_data;
    sc_core::sc_in<RegAddr> rs2_addr;
    sc_core::sc_out<Word> rs2_data;
    
    // Write port
    sc_core::sc_in<RegAddr> rd_addr;
    sc_core::sc_in<Word> rd_data;

    // Constructor
    SC_HAS_PROCESS(RegisterFile);
    RegisterFile(sc_core::sc_module_name name);
    
    // Destructor
    ~RegisterFile();
    
    // Debug method to dump all registers
    void dump_registers() const;

private:
    // Register array - RISC-V has 32 registers
    Word registers[REG_COUNT];
    
    // Process methods
    void read_process();
    void write_process();
};

} // namespace riscv

#endif // REGISTER_FILE_H