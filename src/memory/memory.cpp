#include "memory.h"
#include <fstream>
#include <iostream>

namespace riscv {

Memory::Memory(sc_core::sc_module_name name) : sc_core::sc_module(name), memory(MEM_SIZE, 0) {
    SC_METHOD(memory_process);
    sensitive << clk.pos();
    sensitive << reset;
}

void Memory::memory_process() {
    if (reset.read()) {
        read_data.write(0);
        return;
    }
    
    Word addr = address.read();
    
    if (read_enable.read()) {
        read_data.write(read_word(addr));
    }
    
    if (write_enable.read()) {
        write_word(addr, write_data.read());
    }
}

Word Memory::read_word(Word addr) {
    if (addr >= MEM_SIZE - 3) {
        std::cerr << "Memory read out of bounds: " << std::hex << addr << std::endl;
        return 0;
    }
    
    // Little-endian读取
    Word data = 0;
    data |= static_cast<Word>(memory[addr]) & 0xFF;
    data |= (static_cast<Word>(memory[addr + 1]) & 0xFF) << 8;
    data |= (static_cast<Word>(memory[addr + 2]) & 0xFF) << 16;
    data |= (static_cast<Word>(memory[addr + 3]) & 0xFF) << 24;
    
    return data;
}

void Memory::write_word(Word addr, Word data) {
    if (addr >= MEM_SIZE - 3) {
        std::cerr << "Memory write out of bounds: " << std::hex << addr << std::endl;
        return;
    }
    
    // Little-endian写入
    memory[addr] = data & 0xFF;
    memory[addr + 1] = (data >> 8) & 0xFF;
    memory[addr + 2] = (data >> 16) & 0xFF;
    memory[addr + 3] = (data >> 24) & 0xFF;
}

bool Memory::load_program(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open program file: " << filename << std::endl;
        return false;
    }
    
    // 读取程序到内存
    file.read(reinterpret_cast<char*>(memory.data()), MEM_SIZE);
    std::streamsize bytes_read = file.gcount();
    
    std::cout << "Loaded " << bytes_read << " bytes from " << filename << std::endl;
    return true;
}

} // namespace riscv
