#ifndef EXECUTE_H
#define EXECUTE_H

#include <systemc>
#include "../../include/types.h"
#include "../../include/constants.h"

namespace riscv {

class Execute : public sc_core::sc_module {
public:
    // Ports
    sc_core::sc_in<bool> clk;
    sc_core::sc_in<bool> reset;
    
    // 来自译码单元的输入
    sc_core::sc_in<sc_dt::sc_uint<4>> alu_op; // 使用4位ALU操作码
    sc_core::sc_in<Word> operand1;
    sc_core::sc_in<Word> operand2;
    sc_core::sc_in<Word> pc;
    sc_core::sc_in<Word> imm_value;
    sc_core::sc_in<bool> is_branch;
    
    // 输出
    sc_core::sc_out<Word> alu_result;
    sc_core::sc_out<bool> branch_taken;
    sc_core::sc_out<Word> branch_target;
    
    // Constructor
    SC_HAS_PROCESS(Execute);
    Execute(sc_core::sc_module_name name);
    
private:
    void execute_process();
};

} // namespace riscv

#endif // EXECUTE_H
