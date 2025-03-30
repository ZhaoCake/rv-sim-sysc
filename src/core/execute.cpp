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
        // More detailed branch logic to handle different branch types
        Word pc_val = pc.read();
        Word imm_val = imm_value.read();
        
        // Special handling for JAL and JALR
        // For JAL: PC = PC + imm (PC relative)
        // For JALR: PC = rs1 + imm (absolute)
        bool is_jal = (operation == ALU_ADD && result.to_uint() == pc_val.to_uint() + 4);
        bool is_jalr = (operation == ALU_ADD && result.to_uint() != pc_val.to_uint() + 4);
        
        if (is_jal || is_jalr) {
            branch_taken.write(true);
            // For JAL, pc_val + imm_val; for JALR, just result (already calculated as rs1+imm)
            branch_target.write(is_jalr ? (result.to_uint() & ~1) : (pc_val + imm_val)); 
        }
        else {
            // Normal branch instructions
            bool take_branch = false;
            
            switch (operation) {
                case ALU_SUB: // BEQ/BNE
                    take_branch = (result.to_uint() == 0); // BEQ
                    break;
                case ALU_SLT: // BLT/BGE
                case ALU_SLTU: // BLTU/BGEU
                    take_branch = (result.to_uint() != 0);
                    break;
                default:
                    take_branch = false;
            }
            
            branch_taken.write(take_branch);
            if (take_branch) {
                branch_target.write(pc_val + imm_val);
            } else {
                branch_target.write(0);
            }
        }
    } else {
        branch_taken.write(false);
        branch_target.write(0);
    }
}

} // namespace riscv
