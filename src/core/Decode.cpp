/*
 * rv-sim-sysc - Instruction decode implementation
 * Copyright (C) 2025 ZhaoCake
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "Decode.h"
#include <iostream>

namespace riscv {

Decode::Decode(sc_core::sc_module_name name) : sc_module(name) {
    // Register processes
    SC_METHOD(decode_process);
    sensitive << instruction << rs1_data << rs2_data;
    
    std::cout << "Decode module initialized" << std::endl;
}

Decode::~Decode() {
    // Nothing to clean up
}

void Decode::decode_process() {
    // Default values
    reg_write.write(false);
    mem_read.write(false);
    mem_write.write(false);
    alu_op.write(ALU_ADD);
    alu_src.write(false);
    mem_to_reg.write(false);
    branch.write(false);
    jump.write(false);
    
    // Get instruction
    Word instr_word = instruction.read();
    Instruction instr;
    instr.value = instr_word;
    
    // Extract opcode
    uint32_t opcode = instr_word & 0x7F;
    
    // Extract register addresses
    rs1_addr.write(static_cast<RegAddr>((instr_word >> 15) & 0x1F));
    rs2_addr.write(static_cast<RegAddr>((instr_word >> 20) & 0x1F));
    rd_addr.write(static_cast<RegAddr>((instr_word >> 7) & 0x1F));
    
    // Forward register values
    rs1_value.write(rs1_data.read());
    rs2_value.write(rs2_data.read());
    
    // Decode based on opcode
    switch (opcode) {
        case OP_LUI: {
            // LUI: Load Upper Immediate
            imm.write(extract_imm(instr, 'U'));
            reg_write.write(true);
            alu_op.write(ALU_ADD);  // Pass through immediate
            alu_src.write(true);    // Use immediate
            std::cout << "Decode: LUI, rd=" << rd_addr.read() << ", imm=0x" << std::hex << imm.read() << std::dec << std::endl;
            break;
        }
        case OP_AUIPC: {
            // AUIPC: Add Upper Immediate to PC
            imm.write(extract_imm(instr, 'U'));
            reg_write.write(true);
            alu_op.write(ALU_ADD);  // Add immediate to PC
            alu_src.write(true);    // Use immediate
            std::cout << "Decode: AUIPC, rd=" << rd_addr.read() << ", imm=0x" << std::hex << imm.read() << std::dec << std::endl;
            break;
        }
        case OP_JAL: {
            // JAL: Jump and Link
            imm.write(extract_imm(instr, 'J'));
            reg_write.write(true);
            jump.write(true);
            std::cout << "Decode: JAL, rd=" << rd_addr.read() << ", imm=0x" << std::hex << imm.read() << std::dec << std::endl;
            break;
        }
        case OP_JALR: {
            // JALR: Jump and Link Register
            imm.write(extract_imm(instr, 'I'));
            reg_write.write(true);
            jump.write(true);
            alu_src.write(true);    // Use immediate
            std::cout << "Decode: JALR, rd=" << rd_addr.read() << ", rs1=" << rs1_addr.read() << ", imm=0x" << std::hex << imm.read() << std::dec << std::endl;
            break;
        }
        case OP_BRANCH: {
            // Branch instructions
            imm.write(extract_imm(instr, 'B'));
            branch.write(true);
            uint32_t funct3 = (instr_word >> 12) & 0x7;
            switch (funct3) {
                case FUNCT3_BEQ:
                    std::cout << "Decode: BEQ";
                    break;
                case FUNCT3_BNE:
                    std::cout << "Decode: BNE";
                    break;
                case FUNCT3_BLT:
                    std::cout << "Decode: BLT";
                    break;
                case FUNCT3_BGE:
                    std::cout << "Decode: BGE";
                    break;
                case FUNCT3_BLTU:
                    std::cout << "Decode: BLTU";
                    break;
                case FUNCT3_BGEU:
                    std::cout << "Decode: BGEU";
                    break;
                default:
                    std::cout << "Decode: Unknown branch";
                    break;
            }
            std::cout << ", rs1=" << rs1_addr.read() << ", rs2=" << rs2_addr.read() << ", imm=0x" << std::hex << imm.read() << std::dec << std::endl;
            break;
        }
        case OP_LOAD: {
            // Load instructions
            imm.write(extract_imm(instr, 'I'));
            reg_write.write(true);
            mem_read.write(true);
            alu_src.write(true);    // Use immediate
            mem_to_reg.write(true); // Load from memory
            std::cout << "Decode: LOAD, rd=" << rd_addr.read() << ", rs1=" << rs1_addr.read() << ", imm=0x" << std::hex << imm.read() << std::dec << std::endl;
            break;
        }
        case OP_STORE: {
            // Store instructions
            imm.write(extract_imm(instr, 'S'));
            mem_write.write(true);
            alu_src.write(true);    // Use immediate
            std::cout << "Decode: STORE, rs1=" << rs1_addr.read() << ", rs2=" << rs2_addr.read() << ", imm=0x" << std::hex << imm.read() << std::dec << std::endl;
            break;
        }
        case OP_IMM: {
            // Immediate arithmetic instructions
            imm.write(extract_imm(instr, 'I'));
            reg_write.write(true);
            alu_src.write(true);    // Use immediate
            
            uint32_t funct3 = (instr_word >> 12) & 0x7;
            switch (funct3) {
                case 0x0:  // ADDI
                    alu_op.write(ALU_ADD);
                    std::cout << "Decode: ADDI";
                    break;
                case 0x1:  // SLLI
                    alu_op.write(ALU_SLL);
                    std::cout << "Decode: SLLI";
                    break;
                case 0x2:  // SLTI
                    alu_op.write(ALU_SLT);
                    std::cout << "Decode: SLTI";
                    break;
                case 0x3:  // SLTIU
                    alu_op.write(ALU_SLTU);
                    std::cout << "Decode: SLTIU";
                    break;
                case 0x4:  // XORI
                    alu_op.write(ALU_XOR);
                    std::cout << "Decode: XORI";
                    break;
                case 0x5:  // SRLI/SRAI
                    if ((instr_word >> 30) & 0x1) {
                        alu_op.write(ALU_SRA);
                        std::cout << "Decode: SRAI";
                    } else {
                        alu_op.write(ALU_SRL);
                        std::cout << "Decode: SRLI";
                    }
                    break;
                case 0x6:  // ORI
                    alu_op.write(ALU_OR);
                    std::cout << "Decode: ORI";
                    break;
                case 0x7:  // ANDI
                    alu_op.write(ALU_AND);
                    std::cout << "Decode: ANDI";
                    break;
                default:
                    std::cout << "Decode: Unknown immediate";
                    break;
            }
            std::cout << ", rd=" << rd_addr.read() << ", rs1=" << rs1_addr.read() << ", imm=0x" << std::hex << imm.read() << std::dec << std::endl;
            break;
        }
        case OP_REG: {
            // Register-register arithmetic instructions
            reg_write.write(true);
            alu_src.write(false);    // Use rs2
            
            uint32_t funct3 = (instr_word >> 12) & 0x7;
            uint32_t funct7 = (instr_word >> 25) & 0x7F;
            
            switch (funct3) {
                case 0x0:  // ADD/SUB
                    if (funct7 & 0x20) {
                        alu_op.write(ALU_SUB);
                        std::cout << "Decode: SUB";
                    } else {
                        alu_op.write(ALU_ADD);
                        std::cout << "Decode: ADD";
                    }
                    break;
                case 0x1:  // SLL
                    alu_op.write(ALU_SLL);
                    std::cout << "Decode: SLL";
                    break;
                case 0x2:  // SLT
                    alu_op.write(ALU_SLT);
                    std::cout << "Decode: SLT";
                    break;
                case 0x3:  // SLTU
                    alu_op.write(ALU_SLTU);
                    std::cout << "Decode: SLTU";
                    break;
                case 0x4:  // XOR
                    alu_op.write(ALU_XOR);
                    std::cout << "Decode: XOR";
                    break;
                case 0x5:  // SRL/SRA
                    if (funct7 & 0x20) {
                        alu_op.write(ALU_SRA);
                        std::cout << "Decode: SRA";
                    } else {
                        alu_op.write(ALU_SRL);
                        std::cout << "Decode: SRL";
                    }
                    break;
                case 0x6:  // OR
                    alu_op.write(ALU_OR);
                    std::cout << "Decode: OR";
                    break;
                case 0x7:  // AND
                    alu_op.write(ALU_AND);
                    std::cout << "Decode: AND";
                    break;
                default:
                    std::cout << "Decode: Unknown register op";
                    break;
            }
            std::cout << ", rd=" << rd_addr.read() << ", rs1=" << rs1_addr.read() << ", rs2=" << rs2_addr.read() << std::endl;
            break;
        }
        default:
            std::cout << "Decode: Unknown opcode 0x" << std::hex << opcode << std::dec << std::endl;
            break;
    }
}

Word Decode::extract_imm(const Instruction& instr, char format) {
    Word imm_val = 0;
    
    switch (format) {
        case 'I': {
            // I-type: Sign-extended 12-bit immediate
            imm_val = instr.i.imm;
            // Sign extend
            if (imm_val & 0x800) {
                imm_val |= 0xFFFFF000;
            }
            break;
        }
        case 'S': {
            // S-type: Concatenate imm11:5 and imm4:0
            imm_val = (instr.s.imm1 << 5) | instr.s.imm0;
            // Sign extend
            if (imm_val & 0x800) {
                imm_val |= 0xFFFFF000;
            }
            break;
        }
        case 'B': {
            // B-type: Concatenate and shift
            imm_val = (instr.b.imm1 << 5) | instr.b.imm0;
            // Reconstruct the immediate
            Word temp = 0;
            // imm[12] = instr[31]
            if (imm_val & 0x1000) {
                temp |= 0x1000;
            }
            // imm[11] = instr[7]
            if (instr.b.imm0 & 0x1) {
                temp |= 0x800;
            }
            // imm[10:5] = instr[30:25]
            temp |= ((imm_val >> 6) & 0x3F) << 5;
            // imm[4:1] = instr[11:8]
            temp |= ((instr.b.imm0 >> 1) & 0xF) << 1;
            imm_val = temp;
            // Sign extend
            if (imm_val & 0x1000) {
                imm_val |= 0xFFFFE000;
            }
            break;
        }
        case 'U': {
            // U-type: Upper immediate shifted left by 12
            imm_val = instr.u.imm << 12;
            break;
        }
        case 'J': {
            // J-type: Complex layout
            Word temp = 0;
            // imm[20] = instr[31]
            if (instr.j.imm & 0x80000) {
                temp |= 0x100000;
            }
            // imm[19:12] = instr[19:12]
            temp |= (instr.j.imm & 0xFF000);
            // imm[11] = instr[20]
            if (instr.j.imm & 0x100) {
                temp |= 0x800;
            }
            // imm[10:1] = instr[30:21]
            temp |= ((instr.j.imm >> 9) & 0x3FF) << 1;
            imm_val = temp;
            // Sign extend
            if (imm_val & 0x100000) {
                imm_val |= 0xFFF00000;
            }
            break;
        }
        default:
            std::cerr << "Unknown instruction format: " << format << std::endl;
            break;
    }
    
    return imm_val;
}

} // namespace riscv