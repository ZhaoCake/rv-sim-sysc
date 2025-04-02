/*
 * rv-sim-sysc - Memory module implementation
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

#include "Memory.h"

namespace riscv {

Memory::Memory(sc_core::sc_module_name name) : sc_module(name) {
    // Initialize memory with zero
    mem.resize(MEM_SIZE, 0);
    
    // Register processes
    SC_METHOD(rom_process);
    sensitive << rom_read << rom_address;
    
    SC_METHOD(ram_process);
    sensitive << ram_read << ram_write << ram_address << ram_write_data;
    
    std::cout << "Memory module initialized with size: " << mem.size() << " bytes" << std::endl;
}

Memory::~Memory() {
    // Nothing to clean up
}

void Memory::rom_process() {
    if (rom_read.read()) {
        uint32_t addr = rom_address.read();
        if (addr < mem.size()) {
            rom_data.write(read_word(addr));
        } else {
            std::cerr << "ROM: Out of bounds memory access at address 0x" 
                      << std::hex << addr << std::dec << std::endl;
            rom_data.write(0);
        }
    }
}

void Memory::ram_process() {
    uint32_t addr = ram_address.read();
    
    if (ram_read.read()) {
        if (addr < mem.size()) {
            riscv::Word data = read_word(addr);
            ram_read_data.write(data);
        } else {
            std::cerr << "RAM: Out of bounds memory read at address 0x" 
                      << std::hex << addr << std::dec << std::endl;
            ram_read_data.write(0);
        }
    }
    
    if (ram_write.read()) {
        if (addr < mem.size()) {
            riscv::Word data = ram_write_data.read();
            write_word(addr, data);
        } else {
            std::cerr << "RAM: Out of bounds memory write at address 0x" 
                      << std::hex << addr << std::dec << std::endl;
        }
    }
}

bool Memory::load_binary(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open binary file: " << filename << std::endl;
        return false;
    }
    
    // Read the entire file content into memory
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    if (fileSize > MEM_SIZE) {
        std::cerr << "Binary file too large: " << fileSize << " bytes (max: " << MEM_SIZE << ")" << std::endl;
        return false;
    }
    
    file.read(reinterpret_cast<char*>(mem.data()), fileSize);
    
    if (!file) {
        std::cerr << "Error reading binary file: " << filename << std::endl;
        return false;
    }
    
    std::cout << "Successfully loaded " << fileSize << " bytes from " << filename << std::endl;
    return true;
}

void Memory::dump_memory(const std::string& filename, uint32_t start_addr, uint32_t size) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file for memory dump: " << filename << std::endl;
        return;
    }
    
    uint32_t end_addr = std::min(start_addr + size, static_cast<uint32_t>(mem.size()));
    
    file.write(reinterpret_cast<const char*>(&mem[start_addr]), end_addr - start_addr);
    
    if (!file) {
        std::cerr << "Error writing memory dump to file: " << filename << std::endl;
    } else {
        std::cout << "Memory dumped to " << filename << " (" << (end_addr - start_addr) << " bytes)" << std::endl;
    }
}

riscv::Word Memory::read_word(uint32_t address) {
    uint32_t addr = address & ~0x3; // Align to word boundary
    riscv::Word result = 0;
    
    // Little-endian read
    result |= static_cast<uint32_t>(mem[addr]);
    result |= static_cast<uint32_t>(mem[addr + 1]) << 8;
    result |= static_cast<uint32_t>(mem[addr + 2]) << 16;
    result |= static_cast<uint32_t>(mem[addr + 3]) << 24;
    
    return result;
}

void Memory::write_word(uint32_t address, riscv::Word data) {
    uint32_t addr = address & ~0x3; // Align to word boundary
    
    // Little-endian write
    mem[addr] = data & 0xFF;
    mem[addr + 1] = (data >> 8) & 0xFF;
    mem[addr + 2] = (data >> 16) & 0xFF;
    mem[addr + 3] = (data >> 24) & 0xFF;
}

} // namespace riscv
