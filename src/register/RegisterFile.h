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