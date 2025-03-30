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
    unsigned int address = addr.to_uint();
    
    // Check for valid alignment (words should be 4-byte aligned)
    if (address % 4 != 0) {
        std::cerr << "Warning: Unaligned memory read at address 0x" << std::hex << address << std::endl;
        // Continue with unaligned read (some RISC-V implementations support this)
    }
    
    // Check bounds with proper unsigned comparison
    if (address >= MEM_SIZE - 3) {
        std::cerr << "Error: Memory read out of bounds: 0x" << std::hex << address << std::endl;
        return 0;
    }
    
    // Little-endian读取
    Word data = 0;
    data |= static_cast<Word>(memory[address]) & 0xFF;
    data |= (static_cast<Word>(memory[address + 1]) & 0xFF) << 8;
    data |= (static_cast<Word>(memory[address + 2]) & 0xFF) << 16;
    data |= (static_cast<Word>(memory[address + 3]) & 0xFF) << 24;
    
    return data;
}

void Memory::write_word(Word addr, Word data) {
    unsigned int address = addr.to_uint();
    
    // Check for valid alignment
    if (address % 4 != 0) {
        std::cerr << "Warning: Unaligned memory write at address 0x" << std::hex << address << std::endl;
        // Continue with unaligned write
    }
    
    // Check bounds with proper unsigned comparison
    if (address >= MEM_SIZE - 3) {
        std::cerr << "Error: Memory write out of bounds: 0x" << std::hex << address << std::endl;
        return;
    }
    
    // Little-endian写入
    memory[address] = data.to_uint() & 0xFF;
    memory[address + 1] = (data.to_uint() >> 8) & 0xFF;
    memory[address + 2] = (data.to_uint() >> 16) & 0xFF;
    memory[address + 3] = (data.to_uint() >> 24) & 0xFF;
}


/*
* 加载程序到内存
* @param filename 程序文件名, 例如"program.bin"
* @return 成功加载返回true，失败返回false
*/
bool Memory::load_program(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary); // 以二进制模式打开文件，也就是传入的是bin文件
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
