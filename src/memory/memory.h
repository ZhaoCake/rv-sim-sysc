#ifndef MEMORY_H
#define MEMORY_H

#include <systemc>
#include <vector>
#include "../../include/types.h"
#include "../../include/constants.h"

namespace riscv {

class Memory : public sc_core::sc_module {
public:
    // Ports
    sc_core::sc_in<bool> clk;
    sc_core::sc_in<bool> reset;
    
    // 存储器接口
    sc_core::sc_in<Word> address;
    sc_core::sc_in<Word> write_data;
    sc_core::sc_in<bool> read_enable;
    sc_core::sc_in<bool> write_enable;
    sc_core::sc_out<Word> read_data;
    
    // Constructor and destructor
    SC_HAS_PROCESS(Memory);
    Memory(sc_core::sc_module_name name);
    
    // 加载程序方法
    bool load_program(const std::string& filename);
    
private:
    // Using standard C++ byte type for memory to avoid SystemC overhead
    std::vector<uint8_t> memory;
    
    void memory_process();
    Word read_word(Word addr);
    void write_word(Word addr, Word data);
};

} // namespace riscv

#endif // MEMORY_H
