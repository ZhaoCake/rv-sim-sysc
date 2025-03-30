#include "memory.h"
#include <fstream>
#include <iostream>
#include <iomanip>

namespace riscv {

Memory::Memory(sc_core::sc_module_name name) : sc_core::sc_module(name) {
    // Initialize memory with standard C++ types instead of SystemC types
    memory.resize(MEM_SIZE, 0);
    
    SC_METHOD(memory_process);
    sensitive << clk.pos();
    sensitive << reset;
    
    std::cout << "Memory initialized with size: " << memory.size() << " bytes" << std::endl;
}

void Memory::memory_process() {
    if (reset.read()) {
        read_data.write(0);
        return;
    }
    
    Word addr = address.read();
    
    if (read_enable.read()) {
        Word data = read_word(addr);
        read_data.write(data);
        
        // Debug memory reads
        std::cout << "Memory read: addr=0x" << std::hex << addr.to_uint() 
                  << " data=0x" << data.to_uint() << std::dec << std::endl;
    }
    
    if (write_enable.read()) {
        // Debug memory writes
        std::cout << "Memory write: addr=0x" << std::hex << addr.to_uint() 
                  << " data=0x" << write_data.read().to_uint() << std::dec << std::endl;
                  
        write_word(addr, write_data.read());
    }
}

Word Memory::read_word(Word addr) {
    unsigned int address = addr.to_uint();
    
    // Check for valid alignment (words should be 4-byte aligned)
    if (address % 4 != 0) {
        std::cerr << "Warning: Unaligned memory read at address 0x" << std::hex << address << std::dec << std::endl;
    }
    
    // Check bounds with proper unsigned comparison
    if (address >= MEM_SIZE - 3) {
        std::cerr << "Error: Memory read out of bounds: 0x" << std::hex << address << std::dec << std::endl;
        return 0;
    }
    
    // Little-endian读取 - convert standard types to SystemC type
    uint32_t data = 0;
    data |= static_cast<uint32_t>(memory[address]);
    data |= static_cast<uint32_t>(memory[address + 1]) << 8;
    data |= static_cast<uint32_t>(memory[address + 2]) << 16;
    data |= static_cast<uint32_t>(memory[address + 3]) << 24;
    
    return static_cast<Word>(data);
}

void Memory::write_word(Word addr, Word data) {
    unsigned int address = addr.to_uint();
    uint32_t value = data.to_uint();
    
    // Check for valid alignment
    if (address % 4 != 0) {
        std::cerr << "Warning: Unaligned memory write at address 0x" << std::hex << address << std::dec << std::endl;
    }
    
    // Check bounds with proper unsigned comparison
    if (address >= MEM_SIZE - 3) {
        std::cerr << "Error: Memory write out of bounds: 0x" << std::hex << address << std::dec << std::endl;
        return;
    }
    
    // Little-endian写入 - convert SystemC type to standard types
    memory[address] = value & 0xFF;
    memory[address + 1] = (value >> 8) & 0xFF;
    memory[address + 2] = (value >> 16) & 0xFF;
    memory[address + 3] = (value >> 24) & 0xFF;
}

/*
* 加载程序到内存
* @param filename 程序文件名, 例如"program.bin"
* @return 成功加载返回true，失败返回false
*/
bool Memory::load_program(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open program file: " << filename << std::endl;
        return false;
    }
    
    // 读取程序到内存 - use reinterpret_cast since we're using standard uint8_t
    file.read(reinterpret_cast<char*>(memory.data()), MEM_SIZE);
    std::streamsize bytes_read = file.gcount();
    
    std::cout << "Loaded " << bytes_read << " bytes from " << filename << std::endl;
    
    // Debug - print first few bytes of the loaded program
    std::cout << "Program header (first 16 bytes):" << std::endl;
    for (int i = 0; i < std::min<int>(16, bytes_read); i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                  << static_cast<int>(memory[i]) << " ";
        if ((i + 1) % 4 == 0) std::cout << std::endl;
    }
    std::cout << std::dec << std::endl;
    
    return true;
}

} // namespace riscv
