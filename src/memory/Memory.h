#ifndef MEMORY_H
#define MEMORY_H

#include <systemc>
#include <vector>
#include <fstream>
#include <iostream>
#include "../../include/types.h"
#include "../../include/constants.h"

namespace riscv {

// Memory module with both ROM (fetch) and RAM (load/store) interfaces
class Memory : public sc_core::sc_module {
public:
    // ROM interface ports (for instruction fetching)
    sc_core::sc_in<bool> rom_read;
    sc_core::sc_in<riscv::Word> rom_address;
    sc_core::sc_out<riscv::Word> rom_data;
    
    // RAM interface ports (for load/store unit)
    sc_core::sc_in<bool> ram_read;
    sc_core::sc_in<bool> ram_write;
    sc_core::sc_in<riscv::Word> ram_address;
    sc_core::sc_in<riscv::Word> ram_write_data;
    sc_core::sc_out<riscv::Word> ram_read_data;

    // Constructor
    SC_HAS_PROCESS(Memory);
    Memory(sc_core::sc_module_name name);
    
    // Destructor
    ~Memory();
    
    // Load memory from binary file
    bool load_binary(const std::string& filename);
    
    // Dump memory contents to file for debugging
    void dump_memory(const std::string& filename, uint32_t start_addr, uint32_t size) const;

private:
    // Memory array
    std::vector<uint8_t> mem;
    
    // Memory access processes
    void rom_process();
    void ram_process();
    
    // 读写函数，都以32位读写
    riscv::Word read_word(uint32_t address);
    void write_word(uint32_t address, riscv::Word data);
};

} // namespace riscv

#endif // MEMORY_H
