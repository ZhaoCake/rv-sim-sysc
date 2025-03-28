#include "fetch.h"

namespace riscv {

Fetch::Fetch(sc_core::sc_module_name name) : sc_core::sc_module(name), pc(0) {
    SC_METHOD(fetch_process);
    sensitive << clk.pos();
    sensitive << reset;
}

void Fetch::fetch_process() {
    if (reset.read()) {
        pc = 0;
        pc_out.write(0);
        instruction_out.write(0);
        mem_read.write(false);
    } else {
        if (!stall.read()) {
            if (branch_taken.read()) {
                pc = branch_target.read();
            } else {
                pc = pc + 4; // 移动到下一条指令
            }
            
            // 发送内存读取请求
            mem_addr.write(pc);
            mem_read.write(true);
            
            // 输出当前PC和取出的指令
            pc_out.write(pc);
            instruction_out.write(mem_data->read());
        } else {
            mem_read.write(false);
        }
    }
}

} // namespace riscv
