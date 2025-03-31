#ifndef TOP_H
#define TOP_H

#include <systemc>
#include "../../include/types.h"
#include "Fetch.h"
#include "Decode.h"
#include "Execute.h"
#include "../register/RegisterFile.h"
#include "../memory/Memory.h"

namespace riscv {

class Top : public sc_core::sc_module {
public:
    // Clock and reset signals
    sc_core::sc_in<bool> clk;
    sc_core::sc_in<bool> reset;

    // Constructor
    SC_HAS_PROCESS(Top);
    Top(sc_core::sc_module_name name);
    
    // Destructor
    ~Top();
    
    // Public methods
    bool load_program(const std::string& filename);
    void dump_registers() const;
    void dump_memory(const std::string& filename, uint32_t start, uint32_t size) const;
    
private:
    // Internal signals for connecting modules
    
    // Fetch <-> Memory (instruction)
    sc_core::sc_signal<bool> rom_read_sig;
    sc_core::sc_signal<Word> rom_address_sig;
    sc_core::sc_signal<Word> rom_data_sig;
    
    // Fetch <-> Decode
    sc_core::sc_signal<Word> pc_sig;
    sc_core::sc_signal<Word> next_pc_sig;
    sc_core::sc_signal<Word> instruction_sig;
    
    // Execute -> Fetch (branch)
    sc_core::sc_signal<bool> branch_taken_sig;
    sc_core::sc_signal<Word> branch_target_sig;
    
    // Decode <-> RegisterFile
    sc_core::sc_signal<RegAddr> rs1_addr_sig;
    sc_core::sc_signal<Word> rs1_data_sig;
    sc_core::sc_signal<RegAddr> rs2_addr_sig;
    sc_core::sc_signal<Word> rs2_data_sig;
    
    // Decode -> Execute
    sc_core::sc_signal<Word> imm_sig;
    sc_core::sc_signal<RegAddr> rd_addr_sig;
    sc_core::sc_signal<Word> rs1_value_sig;
    sc_core::sc_signal<Word> rs2_value_sig;
    sc_core::sc_signal<bool> reg_write_sig;
    sc_core::sc_signal<bool> mem_read_sig;
    sc_core::sc_signal<bool> mem_write_sig;
    sc_core::sc_signal<sc_dt::sc_uint<4>> alu_op_sig;
    sc_core::sc_signal<bool> alu_src_sig;
    sc_core::sc_signal<bool> mem_to_reg_sig;
    sc_core::sc_signal<bool> branch_sig;
    sc_core::sc_signal<bool> jump_sig;
    
    // Execute outputs
    sc_core::sc_signal<Word> alu_result_sig;
    sc_core::sc_signal<Word> mem_write_data_sig;
    sc_core::sc_signal<bool> mem_read_out_sig;
    sc_core::sc_signal<bool> mem_write_out_sig;
    sc_core::sc_signal<bool> reg_write_out_sig;
    sc_core::sc_signal<bool> mem_to_reg_out_sig;
    sc_core::sc_signal<RegAddr> rd_addr_out_sig;
    
    // Execute/Memory/WriteBack -> RAM interface
    sc_core::sc_signal<bool> ram_read_sig;
    sc_core::sc_signal<bool> ram_write_sig;
    sc_core::sc_signal<Word> ram_address_sig;
    sc_core::sc_signal<Word> ram_write_data_sig;
    sc_core::sc_signal<Word> ram_read_data_sig;
    
    // Register writeback
    sc_core::sc_signal<bool> reg_write_wb_sig;
    sc_core::sc_signal<RegAddr> rd_addr_wb_sig;
    sc_core::sc_signal<Word> rd_data_wb_sig;
    
    // Stall signal
    sc_core::sc_signal<bool> stall_sig;
    
    // Module instances
    Fetch* fetch;
    Decode* decode;
    Execute* execute;
    RegisterFile* register_file;
    Memory* memory;
    
    // Process methods
    void memory_access_process();
    void writeback_process();
    void debug_process();
};

} // namespace riscv

#endif // TOP_H