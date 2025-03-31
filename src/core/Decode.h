#ifndef DECODE_H
#define DECODE_H

#include <systemc>
#include "../../include/types.h"
#include "../../include/constants.h"

namespace riscv {

class Decode : public sc_core::sc_module {
public:
    // Clock and reset
    sc_core::sc_in<bool> clk;
    sc_core::sc_in<bool> reset;
    
    // Input from fetch stage
    sc_core::sc_in<Word> instruction;
    sc_core::sc_in<Word> pc;
    
    // Register file interface
    sc_core::sc_out<RegAddr> rs1_addr;
    sc_core::sc_in<Word> rs1_data;
    sc_core::sc_out<RegAddr> rs2_addr;
    sc_core::sc_in<Word> rs2_data;
    
    // Outputs to execute stage
    sc_core::sc_out<Word> imm;          // Immediate value
    sc_core::sc_out<RegAddr> rd_addr;   // Destination register address
    sc_core::sc_out<Word> rs1_value;    // Value of rs1
    sc_core::sc_out<Word> rs2_value;    // Value of rs2
    
    // Control signals
    sc_core::sc_out<bool> reg_write;       // Write to register file
    sc_core::sc_out<bool> mem_read;        // Read from memory
    sc_core::sc_out<bool> mem_write;       // Write to memory
    sc_core::sc_out<sc_dt::sc_uint<4>> alu_op;   // ALU operation
    sc_core::sc_out<bool> alu_src;         // 0: rs2, 1: immediate
    sc_core::sc_out<bool> mem_to_reg;      // 0: ALU result, 1: memory data
    sc_core::sc_out<bool> branch;          // Is branch instruction
    sc_core::sc_out<bool> jump;            // Is jump instruction

    // Constructor
    SC_HAS_PROCESS(Decode);
    Decode(sc_core::sc_module_name name);
    
    // Destructor
    ~Decode();
    
private:
    // Process methods
    void decode_process();
    
    // Helper method to extract immediates based on format
    Word extract_imm(const Instruction& instr, char format);
};

} // namespace riscv

#endif // DECODE_H