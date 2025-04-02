/*
 * rv-sim-sysc - Register file implementation
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

#include "RegisterFile.h"
#include <iostream>
#include <iomanip>

namespace riscv {

RegisterFile::RegisterFile(sc_core::sc_module_name name) : sc_module(name) {
    // Initialize all registers to 0
    for (int i = 0; i < REG_COUNT; i++) {
        registers[i] = 0;
    }
    
    // Register processes
    SC_METHOD(read_process);
    sensitive << rs1_addr << rs2_addr;
    
    SC_METHOD(write_process);
    sensitive << clk.pos();
    
    std::cout << "RegisterFile module initialized" << std::endl;
}

RegisterFile::~RegisterFile() {
    // Nothing to clean up
}

void RegisterFile::read_process() {
    // Always read the values from the registers based on the addresses
    // RISC-V register x0 is hardwired to 0
    if (rs1_addr.read() == 0) {
        rs1_data.write(0);
    } else {
        rs1_data.write(registers[rs1_addr.read()]);
    }
    
    if (rs2_addr.read() == 0) {
        rs2_data.write(0);
    } else {
        rs2_data.write(registers[rs2_addr.read()]);
    }
}

void RegisterFile::write_process() {
    // Only write on the positive clock edge and when write_enable is true
    if (write_enable.read()) {
        uint32_t rd = rd_addr.read();
        // Register x0 is hardwired to 0, can't be modified
        if (rd != 0) {
            registers[rd] = rd_data.read();
            std::cout << "Register x" << rd << " updated to 0x" 
                      << std::hex << registers[rd] << std::dec << std::endl;
        }
    }
}

void RegisterFile::dump_registers() const {
    std::cout << "Register File Contents:" << std::endl;
    for (int i = 0; i < REG_COUNT; i++) {
        std::cout << "x" << std::setw(2) << std::setfill('0') << i << " = 0x" 
                 << std::hex << std::setw(8) << std::setfill('0') << registers[i] 
                 << std::dec << (((i + 1) % 4 == 0) ? "\n" : "  ");
    }
    std::cout << std::endl;
}

} // namespace riscv