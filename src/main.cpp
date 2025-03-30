#include <systemc>
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <string>
#include "core/top.h"

void print_usage(const char* prog_name) {
    std::cout << "Usage: " << prog_name << " <program_file> [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --wave <filename>    Generate waveform trace (VCD format)" << std::endl;
    std::cout << "  --cycles <num>       Maximum simulation cycles (default: 200)" << std::endl;
    std::cout << "  --slow               Add delays between cycles for observation" << std::endl;
    std::cout << "  --help               Display this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Example: " << prog_name << " add_test.bin --wave wave.vcd" << std::endl;
    std::cout << "View waveforms with GTKWave: gtkwave wave.vcd" << std::endl;
}

int sc_main(int argc, char* argv[]) {
    // 默认参数值
    std::string program_file;
    std::string wave_file = "";  // 空字符串表示不生成波形
    unsigned int max_cycles = 200;
    bool slow_mode = false;
    
    // 解析命令行参数
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    program_file = argv[1];
    
    // 处理选项参数
    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--wave" && i + 1 < argc) {
            wave_file = argv[++i];
        } else if (arg == "--cycles" && i + 1 < argc) {
            max_cycles = std::stoi(argv[++i]);
        } else if (arg == "--slow") {
            slow_mode = true;
        } else if (arg == "--help") {
            print_usage(argv[0]);
            return 0;
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            print_usage(argv[0]);
            return 1;
        }
    }
    
    try {
        // 创建时钟和复位信号
        sc_core::sc_clock clock("clock", 20, sc_core::SC_NS);  // 20ns，50MHz
        sc_core::sc_signal<bool> reset;
        
        // 创建处理器实例
        riscv::RiscvProcessor processor("riscv_processor");
        processor.clk(clock);
        processor.reset(reset);
        
        // 创建波形跟踪文件（如果指定了）
        sc_core::sc_trace_file* tf = nullptr;
        if (!wave_file.empty()) {
            tf = sc_core::sc_create_vcd_trace_file(wave_file.c_str());
            if (tf) {
                std::cout << "Waveform tracing enabled to file: " << wave_file << std::endl;
                
                // 添加基本信号到波形文件
                sc_core::sc_trace(tf, clock, "clock");
                sc_core::sc_trace(tf, reset, "reset");
                
                // 使用处理器的trace方法添加内部信号
                processor.trace(tf);
            }
        }
        
        // 加载程序
        if (!processor.load_program(program_file)) {
            if (tf) sc_core::sc_close_vcd_trace_file(tf);
            return 1;
        }
        
        // 初始化仿真
        std::cout << "Starting reset phase..." << std::endl;
        reset.write(true);
        sc_core::sc_start(40, sc_core::SC_NS);  // 等待复位, 40ns是复位时间
        reset.write(false);
        std::cout << "Reset complete, starting simulation..." << std::endl;
        
        // 运行仿真，直到达到最大周期数
        unsigned int cycle_count = 0;
        
        while (cycle_count < max_cycles) {
            std::cout << "\n--- Cycle " << cycle_count << " ---" << std::endl;
            sc_core::sc_start(20, sc_core::SC_NS);  // 一个时钟周期
            cycle_count++;
            
            // 检测空指令(0x00000000)
            // 这里需要处理器实例添加一个方法来获取当前指令
            // 例如: if (processor.current_instruction() == 0) break;
            
            // 每个周期后添加小延迟，便于观察（如果启用）
            if (slow_mode) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
        
        if (cycle_count >= max_cycles) {
            std::cout << "\nSimulation stopped after reaching maximum cycle count (" 
                      << max_cycles << ")" << std::endl;
        }
        
        std::cout << "\nSimulation completed after " << cycle_count << " cycles at " 
                  << sc_core::sc_time_stamp() << std::endl;
        
        // 关闭波形文件
        if (tf) sc_core::sc_close_vcd_trace_file(tf);
                
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
