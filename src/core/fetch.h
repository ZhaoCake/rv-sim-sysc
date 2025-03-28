#ifndef FETCH_H
#define FETCH_H

#include <systemc>
#include "../../include/types.h"

namespace riscv {

class Fetch : public sc_core::sc_module {
public:
    // Ports
    sc_core::sc_in<bool> clk;
    sc_core::sc_in<bool> reset;
    sc_core::sc_in<bool> stall;
    sc_core::sc_in<bool> branch_taken;
    sc_core::sc_in<Word> branch_target;
    sc_core::sc_out<Word> pc_out;
    sc_core::sc_out<Word> instruction_out;
    
    // Memory interface
    sc_core::sc_port<sc_core::sc_signal_in_if<Word>> mem_data;
    sc_core::sc_out<Word> mem_addr;
    sc_core::sc_out<bool> mem_read;
    
    // Constructor
    SC_HAS_PROCESS(Fetch);
    Fetch(sc_core::sc_module_name name);
    
private:
    Word pc; // 程序计数器
    
    void fetch_process();
};

} // namespace riscv

#endif // FETCH_H
