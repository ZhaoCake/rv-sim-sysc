#include "execute.h"

namespace riscv {

Execute::Execute(sc_core::sc_module_name name) : sc_core::sc_module(name) {
    SC_METHOD(execute_process);
    sensitive << operand1 << operand2 << alu_op << pc << imm_value << is_branch;
}

void Execute::execute_process() {
    if (reset.read()) {
        alu_result.write(0);
        branch_taken.write(false);
        branch_target.write(0);
        return;
    }
    
    Word op1 = operand1.read();
    Word op2 = operand2.read();
    unsigned int operation = alu_op.read();
    Word result = 0;
    
    // ALU操作
    switch (operation) {
        case ALU_ADD:
            result = op1 + op2;
            break;
        case ALU_SUB:
            result = op1 - op2;
            break;
        case ALU_AND:
            result = op1 & op2;
            break;
        case ALU_OR:
            result = op1 | op2;
            break;
        case ALU_XOR:
            result = op1 ^ op2;
            break;
        case ALU_SLL:
            result = op1 << (op2 & 0x1F);
            break;
        case ALU_SRL:
            result = op1 >> (op2 & 0x1F);
            break;
        case ALU_SRA:
            result = static_cast<signed int>(op1) >> (op2 & 0x1F);
            break;
        case ALU_SLT:
            result = (static_cast<signed int>(op1) < static_cast<signed int>(op2)) ? 1 : 0;
            break;
        case ALU_SLTU:
            result = (op1 < op2) ? 1 : 0;
            break;
        default:
            result = 0;
            break;
    }
    
    alu_result.write(result);
    
    // 分支处理
    if (is_branch.read()) {
        // 对于JAL和JALR指令，始终分支
        if (result != 0) {
            branch_taken.write(true);
            branch_target.write(pc.read() + imm_value.read());
        } else {
            branch_taken.write(false);
            branch_target.write(0);
        }
    } else {
        branch_taken.write(false);
        branch_target.write(0);
    }
}

} // namespace riscv
