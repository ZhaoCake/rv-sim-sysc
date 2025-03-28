#include "decode.h"

namespace riscv {

Decode::Decode(sc_core::sc_module_name name) : sc_core::sc_module(name) {
    SC_METHOD(decode_process);
    sensitive << instruction << pc << rs1_data << rs2_data;
}

void Decode::decode_process() {
    if (reset.read()) {
        // 重置所有输出信号
        rd_addr.write(0);
        rs1_addr.write(0);
        rs2_addr.write(0);
        alu_op.write(0);
        operand1.write(0);
        operand2.write(0);
        mem_read.write(false);
        mem_write.write(false);
        reg_write.write(false);
        is_branch.write(false);
        imm_value.write(0);
        return;
    }
    
    // 获取指令 - 将SystemC类型转换为标准C++类型
    Instruction inst;
    inst.value = static_cast<CppWord>(instruction.read().to_uint());
    
    // 解析opcode
    unsigned int opcode = inst.value & 0x7F;  // 提取低7位
    
    // 默认值
    rs1_addr.write(0);
    rs2_addr.write(0);
    rd_addr.write(0);
    alu_op.write(ALU_ADD);
    mem_read.write(false);
    mem_write.write(false);
    reg_write.write(false);
    is_branch.write(false);
    imm_value.write(0);
    
    switch (opcode) {
        case OP_LUI: {
            // U-type: rd = imm << 12
            rd_addr.write(inst.u.rd);
            imm_value.write(inst.u.imm << 12);
            operand1.write(0);
            operand2.write(inst.u.imm << 12);
            alu_op.write(ALU_ADD);  // 直接传递操作数2
            reg_write.write(true);
            break;
        }
        
        case OP_AUIPC: {
            // U-type: rd = pc + (imm << 12)
            rd_addr.write(inst.u.rd);
            imm_value.write(inst.u.imm << 12);
            operand1.write(pc.read());
            operand2.write(inst.u.imm << 12);
            alu_op.write(ALU_ADD);
            reg_write.write(true);
            break;
        }
        
        case OP_JAL: {
            // J-type: rd = pc + 4; pc += imm
            rd_addr.write(inst.u.rd);
            
            // J-type immediate构建
            Word jimm = 0;
            jimm |= ((inst.value >> 31) & 0x1) << 20;    // imm[20]
            jimm |= ((inst.value >> 12) & 0xFF) << 12;   // imm[19:12]
            jimm |= ((inst.value >> 20) & 0x1) << 11;    // imm[11]
            jimm |= ((inst.value >> 21) & 0x3FF) << 1;   // imm[10:1]
            
            // 符号扩展
            if (jimm & 0x00100000)
                jimm |= 0xFFF00000;
                
            imm_value.write(jimm);
            operand1.write(pc.read());
            operand2.write(4);
            alu_op.write(ALU_ADD);
            reg_write.write(true);
            is_branch.write(true);
            break;
        }
        
        case OP_JALR: {
            // I-type: rd = pc + 4; pc = rs1 + imm
            rd_addr.write(inst.i.rd);
            rs1_addr.write(inst.i.rs1);
            
            // 符号扩展imm
            Word imm = inst.i.imm;
            if (imm & 0x800)
                imm |= 0xFFFFF000;
                
            imm_value.write(imm);
            operand1.write(rs1_data.read());
            operand2.write(imm);
            alu_op.write(ALU_ADD);
            reg_write.write(true);
            is_branch.write(true);
            break;
        }
        
        case OP_BRANCH: {
            // B-type: if (rs1 OP rs2) pc += imm
            rs1_addr.write(inst.r.rs1);
            rs2_addr.write(inst.r.rs2);
            
            // B-type immediate构建
            Word bimm = 0;
            bimm |= ((inst.value >> 31) & 0x1) << 12;   // imm[12]
            bimm |= ((inst.value >> 7) & 0x1) << 11;    // imm[11]
            bimm |= ((inst.value >> 25) & 0x3F) << 5;   // imm[10:5]
            bimm |= ((inst.value >> 8) & 0xF) << 1;     // imm[4:1]
            
            // 符号扩展
            if (bimm & 0x1000)
                bimm |= 0xFFFFE000;
                
            imm_value.write(bimm);
            operand1.write(rs1_data.read());
            operand2.write(rs2_data.read());
            
            switch (inst.r.funct3) {
                case FUNCT3_BEQ:  alu_op.write(ALU_SUB); break;
                case FUNCT3_BNE:  alu_op.write(ALU_SUB); break;
                case FUNCT3_BLT:  alu_op.write(ALU_SLT); break;
                case FUNCT3_BGE:  alu_op.write(ALU_SLT); break;
                case FUNCT3_BLTU: alu_op.write(ALU_SLTU); break;
                case FUNCT3_BGEU: alu_op.write(ALU_SLTU); break;
                default: alu_op.write(ALU_ADD); break;
            }
            
            is_branch.write(true);
            break;
        }
        
        case OP_LOAD: {
            // I-type: rd = MEM[rs1 + imm]
            rd_addr.write(inst.i.rd);
            rs1_addr.write(inst.i.rs1);
            
            // 符号扩展imm
            Word imm = inst.i.imm;
            if (imm & 0x800)
                imm |= 0xFFFFF000;
                
            imm_value.write(imm);
            operand1.write(rs1_data.read());
            operand2.write(imm);
            alu_op.write(ALU_ADD);
            mem_read.write(true);
            reg_write.write(true);
            break;
        }
        
        case OP_STORE: {
            // S-type: MEM[rs1 + imm] = rs2
            rs1_addr.write(inst.s.rs1);
            rs2_addr.write(inst.s.rs2);
            
            // S-type immediate构建
            Word simm = 0;
            simm |= ((inst.value >> 25) & 0x7F) << 5;  // imm[11:5]
            simm |= ((inst.value >> 7) & 0x1F);        // imm[4:0]
            
            // 符号扩展
            if (simm & 0x800)
                simm |= 0xFFFFF000;
                
            imm_value.write(simm);
            operand1.write(rs1_data.read());
            operand2.write(simm);
            alu_op.write(ALU_ADD);
            mem_write.write(true);
            break;
        }
        
        case OP_IMM: {
            // I-type: rd = rs1 OP imm
            rd_addr.write(inst.i.rd);
            rs1_addr.write(inst.i.rs1);
            
            // 符号扩展imm
            Word imm = inst.i.imm;
            if ((inst.i.funct3 != 0x1) && (inst.i.funct3 != 0x5)) {
                if (imm & 0x800)
                    imm |= 0xFFFFF000;
            }
                
            imm_value.write(imm);
            operand1.write(rs1_data.read());
            operand2.write(imm);
            
            switch (inst.i.funct3) {
                case 0x0: alu_op.write(ALU_ADD); break;  // ADDI
                case 0x1: alu_op.write(ALU_SLL); break;  // SLLI
                case 0x2: alu_op.write(ALU_SLT); break;  // SLTI
                case 0x3: alu_op.write(ALU_SLTU); break; // SLTIU
                case 0x4: alu_op.write(ALU_XOR); break;  // XORI
                case 0x5: // SRLI/SRAI
                    if ((imm >> 5) & 0x7F) 
                        alu_op.write(ALU_SRA);  // SRAI
                    else
                        alu_op.write(ALU_SRL);  // SRLI
                    break;
                case 0x6: alu_op.write(ALU_OR); break;   // ORI
                case 0x7: alu_op.write(ALU_AND); break;  // ANDI
                default:  alu_op.write(ALU_ADD); break;
            }
            
            reg_write.write(true);
            break;
        }
        
        case OP_REG: {
            // R-type: rd = rs1 OP rs2
            rd_addr.write(inst.r.rd);
            rs1_addr.write(inst.r.rs1);
            rs2_addr.write(inst.r.rs2);
            
            operand1.write(rs1_data.read());
            operand2.write(rs2_data.read());
            
            if (inst.r.funct7 == 0x00) {
                switch (inst.r.funct3) {
                    case 0x0: alu_op.write(ALU_ADD); break;  // ADD
                    case 0x1: alu_op.write(ALU_SLL); break;  // SLL
                    case 0x2: alu_op.write(ALU_SLT); break;  // SLT
                    case 0x3: alu_op.write(ALU_SLTU); break; // SLTU
                    case 0x4: alu_op.write(ALU_XOR); break;  // XOR
                    case 0x5: alu_op.write(ALU_SRL); break;  // SRL
                    case 0x6: alu_op.write(ALU_OR); break;   // OR
                    case 0x7: alu_op.write(ALU_AND); break;  // AND
                    default:  alu_op.write(ALU_ADD); break;
                }
            } else if (inst.r.funct7 == 0x20) {
                switch (inst.r.funct3) {
                    case 0x0: alu_op.write(ALU_SUB); break;  // SUB
                    case 0x5: alu_op.write(ALU_SRA); break;  // SRA
                    default:  alu_op.write(ALU_ADD); break;
                }
            }
            
            reg_write.write(true);
            break;
        }
        
        default:
            // 未知或不支持的指令
            break;
    }
}

Word Decode::sign_extend(Word value, int bits) {
    // 先转换为标准类型处理符号扩展
    CppWord cpp_value = static_cast<CppWord>(value);
    if (cpp_value & (1 << (bits - 1)))
        cpp_value |= ~((1 << bits) - 1);
    // 再转回SystemC类型
    return static_cast<Word>(cpp_value);
}

} // namespace riscv
