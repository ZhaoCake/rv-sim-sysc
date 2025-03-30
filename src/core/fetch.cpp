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
            Word next_pc;
            
            if (branch_taken.read()) {
                next_pc = branch_target.read();
            } else {
                next_pc = pc + 4; // 移动到下一条指令
            }
            
            // 发送内存读取请求
            mem_addr.write(pc); // First read instruction at current PC
            mem_read.write(true);
            
            // Then update PC for next cycle
            pc = next_pc;
            
            // 输出当前PC和取出的指令
            pc_out.write(pc);
            instruction_out.write(mem_data->read());
            
            // Debug output
            std::cout << "Fetch: PC=0x" << std::hex << pc.to_uint() 
                      << ", Instr=0x" << mem_data->read().to_uint() << std::endl;
        } else {
            mem_read.write(false);
        }
    }
}

} // namespace riscv
