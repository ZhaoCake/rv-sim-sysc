#ifndef REGISTER_FILE_H
#define REGISTER_FILE_H

#include <systemc>
#include <array>
#include "../../include/types.h"
#include "../../include/constants.h"

namespace riscv {

class RegisterFile : public sc_core::sc_module {
public:
    // Ports
    sc_core::sc_in<bool> clk;
    sc_core::sc_in<bool> reset;
    
    // 读端口
    sc_core::sc_in<RegAddr> rs1_addr;
    sc_core::sc_in<RegAddr> rs2_addr;
    sc_core::sc_out<Word> rs1_data;
    sc_core::sc_out<Word> rs2_data;
    
    // 写端口
    sc_core::sc_in<bool> write_enable;
    sc_core::sc_in<RegAddr> rd_addr;
    sc_core::sc_in<Word> write_data;
    
    // Constructor
    SC_HAS_PROCESS(RegisterFile);
    RegisterFile(sc_core::sc_module_name name);
    
private:
    std::array<Word, REG_COUNT> registers;
    
    void read_process();
    void write_process();
};

} // namespace riscv

#endif // REGISTER_FILE_H
