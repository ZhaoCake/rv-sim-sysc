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
    sc_core::sc_clock clock("clock", 20, sc_core::SC_NS);  // 20ns，50MHz
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
    std::cout << "Starting reset..." << std::endl;
    sc_core::sc_start(40, sc_core::SC_NS);  // 等待复位, 40ns是复位时间
    reset.write(false);
    std::cout << "Reset complete, starting simulation..." << std::endl;
    
    // 运行仿真，增加时间以确保程序执行完成
    sc_core::sc_start(10000, sc_core::SC_NS); // 增加到10000ns，足够运行更多周期
    
    std::cout << "Simulation completed at " << sc_core::sc_time_stamp() << std::endl;
    
    return 0;
}
