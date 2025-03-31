#include "Execute.h"
#include <iostream>

namespace riscv {

Execute::Execute(sc_core::sc_module_name name) : sc_module(name) {
    // Register processes
    SC_METHOD(execute_process);
    sensitive << pc << rs1_value << rs2_value << imm << rd_addr 
              << alu_op << alu_src << branch << jump 
              << mem_read << mem_write << reg_write << mem_to_reg;
    
    std::cout << "Execute module initialized" << std::endl;
}

Execute::~Execute() {
    // Nothing to clean up
}

void Execute::execute_process() {
    // Forward control signals
    mem_read_out.write(mem_read.read());
    mem_write_out.write(mem_write.read());
    reg_write_out.write(reg_write.read());
    mem_to_reg_out.write(mem_to_reg.read());
    rd_addr_out.write(rd_addr.read());
    
    // Forward memory write data
    mem_write_data.write(rs2_value.read());
    
    // Choose operands
    Word op1 = rs1_value.read();
    Word op2 = alu_src.read() ? imm.read() : rs2_value.read();
    
    // For AUIPC instruction, use PC as first operand
    if (alu_op.read() == ALU_ADD && alu_src.read() && 
        ((imm.read() & 0xFFF) == 0)) {  // Check if likely an upper immediate
        op1 = pc.read();
    }
    
    // Compute ALU result
    Word result = alu_compute(op1, op2, alu_op.read());
    alu_result.write(result);
    
    // Calculate branch target
    branch_target.write(pc.read() + imm.read());
    
    // Determine if branch is taken
    bool is_branch_taken = false;
    
    if (jump.read()) {
        // For JAL/JALR, always take the branch
        is_branch_taken = true;
        
        if (alu_op.read() == ALU_ADD && rd_addr.read() != 0) {
            // For JALR, compute target based on rs1 + imm
            if (alu_src.read()) {
                branch_target.write((op1 + op2) & ~0x1); // Clear LSB as per RISC-V spec
            }
        }
    } else if (branch.read()) {
        // For conditional branches, evaluate the condition
        uint32_t funct3 = (alu_op.read() & 0x7); // Use lower 3 bits of alu_op for branch type
        is_branch_taken = evaluate_branch(op1, op2, funct3);
    }
    
    branch_taken.write(is_branch_taken);
    
    // Debug output
    std::cout << "Execute: ALU op=" << alu_op.read() << ", op1=0x" << std::hex << op1 
              << ", op2=0x" << op2 << ", result=0x" << result << std::dec << std::endl;
    
    if (branch.read() || jump.read()) {
        std::cout << "Execute: Branch/Jump target=0x" << std::hex << branch_target.read() 
                  << ", taken=" << is_branch_taken << std::dec << std::endl;
    }
}

Word Execute::alu_compute(Word a, Word b, sc_dt::sc_uint<4> op) {
    switch (op.to_uint()) {
        case ALU_ADD:
            return a + b;
        case ALU_SUB:
            return a - b;
        case ALU_AND:
            return a & b;
        case ALU_OR:
            return a | b;
        case ALU_XOR:
            return a ^ b;
        case ALU_SLL:
            return a << (b & 0x1F); // Shift by the lower 5 bits only
        case ALU_SRL:
            return a >> (b & 0x1F); // Logical shift right
        case ALU_SRA: {
            // Arithmetic shift right (preserve sign bit)
            int32_t signed_a = static_cast<int32_t>(a);
            return static_cast<Word>(signed_a >> (b & 0x1F));
        }
        case ALU_SLT: {
            // Set less than (signed)
            int32_t signed_a = static_cast<int32_t>(a);
            int32_t signed_b = static_cast<int32_t>(b);
            return (signed_a < signed_b) ? 1 : 0;
        }
        case ALU_SLTU:
            // Set less than unsigned
            return (a < b) ? 1 : 0;
        default:
            std::cerr << "Unknown ALU operation: " << op << std::endl;
            return 0;
    }
}

bool Execute::evaluate_branch(Word a, Word b, uint32_t funct3) {
    switch (funct3) {
        case FUNCT3_BEQ:
            return a == b;
        case FUNCT3_BNE:
            return a != b;
        case FUNCT3_BLT: {
            int32_t signed_a = static_cast<int32_t>(a);
            int32_t signed_b = static_cast<int32_t>(b);
            return signed_a < signed_b;
        }
        case FUNCT3_BGE: {
            int32_t signed_a = static_cast<int32_t>(a);
            int32_t signed_b = static_cast<int32_t>(b);
            return signed_a >= signed_b;
        }
        case FUNCT3_BLTU:
            return a < b;
        case FUNCT3_BGEU:
            return a >= b;
        default:
            std::cerr << "Unknown branch condition: " << funct3 << std::endl;
            return false;
    }
}

} // namespace riscv