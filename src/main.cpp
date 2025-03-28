#include <systemc>
#include <iostream>
#include "core/top.h"

int sc_main(int argc, char* argv[]) {
    // 检查命令行参数
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <program_file>" << std::endl;
        return 1;
    }
    
    // 创建时钟和复位信号
    sc_core::sc_clock clock("clock", 10, sc_core::SC_NS);
    sc_core::sc_signal<bool> reset;
    
    // 创建处理器实例
    riscv::RiscvProcessor processor("riscv_processor");
    processor.clk(clock);
    processor.reset(reset);
    
    // 加载程序
    if (!processor.load_program(argv[1])) {
        return 1;
    }
    
    // 初始化仿真
    reset.write(true);
    sc_core::sc_start(20, sc_core::SC_NS);
    reset.write(false);
    
    // 运行仿真
    sc_core::sc_start(1000, sc_core::SC_NS);
    
    std::cout << "Simulation completed at " << sc_core::sc_time_stamp() << std::endl;
    
    return 0;
}
