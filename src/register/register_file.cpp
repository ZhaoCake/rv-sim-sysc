#include "register_file.h"

namespace riscv {

RegisterFile::RegisterFile(sc_core::sc_module_name name) : sc_core::sc_module(name) {
    // 初始化寄存器
    for (auto &reg : registers) {
        reg = 0;
    }
    
    SC_METHOD(read_process);
    sensitive << rs1_addr << rs2_addr;
    
    SC_METHOD(write_process);
    sensitive << clk.pos();
    sensitive << reset;
}

void RegisterFile::read_process() {
    if (reset.read()) {
        rs1_data.write(0);
        rs2_data.write(0);
        return;
    }
    
    // 读取寄存器值
    unsigned int rs1 = rs1_addr.read();
    unsigned int rs2 = rs2_addr.read();
    
    // x0寄存器始终为0
    if (rs1 == 0) {
        rs1_data.write(0);
    } else {
        rs1_data.write(registers[rs1]);
    }
    
    if (rs2 == 0) {
        rs2_data.write(0);
    } else {
        rs2_data.write(registers[rs2]);
    }
}

void RegisterFile::write_process() {
    if (reset.read()) {
        for (auto &reg : registers) {
            reg = 0;
        }
        return;
    }
    
    // 写入寄存器
    if (write_enable.read()) {
        unsigned int rd = rd_addr.read();
        if (rd != 0) {  // x0寄存器不可写
            registers[rd] = write_data.read();
        }
    }
}

} // namespace riscv
