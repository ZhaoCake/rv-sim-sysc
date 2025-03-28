#include <systemc>
#include <iostream>
#include <iomanip>
#include "../include/constants.h"
#include "../include/types.h"
#include "../src/register/register_file.h"

using namespace riscv;

// 寄存器文件测试台
SC_MODULE(RegisterFileTestbench) {
    // 信号声明
    sc_core::sc_signal<bool> clk_sig;
    sc_core::sc_signal<bool> reset_sig;
    
    sc_core::sc_signal<RegAddr> rs1_addr_sig;
    sc_core::sc_signal<RegAddr> rs2_addr_sig;
    sc_core::sc_signal<Word> rs1_data_sig;
    sc_core::sc_signal<Word> rs2_data_sig;
    
    sc_core::sc_signal<bool> write_enable_sig;
    sc_core::sc_signal<RegAddr> rd_addr_sig;
    sc_core::sc_signal<Word> write_data_sig;
    
    // 寄存器文件实例
    RegisterFile* reg_file;
    sc_core::sc_trace_file* tf;
    
    SC_CTOR(RegisterFileTestbench) {
        // 实例化寄存器文件
        reg_file = new RegisterFile("regfile_instance");
        reg_file->clk(clk_sig);
        reg_file->reset(reset_sig);
        reg_file->rs1_addr(rs1_addr_sig);
        reg_file->rs2_addr(rs2_addr_sig);
        reg_file->rs1_data(rs1_data_sig);
        reg_file->rs2_data(rs2_data_sig);
        reg_file->write_enable(write_enable_sig);
        reg_file->rd_addr(rd_addr_sig);
        reg_file->write_data(write_data_sig);
        
        // 测试进程
        SC_THREAD(clock_gen);
        SC_THREAD(stimulus);
        
        // 波形跟踪
        tf = sc_core::sc_create_vcd_trace_file("regfile_waveform");
        sc_core::sc_trace(tf, clk_sig, "clk");
        sc_core::sc_trace(tf, reset_sig, "reset");
        sc_core::sc_trace(tf, rs1_addr_sig, "rs1_addr");
        sc_core::sc_trace(tf, rs2_addr_sig, "rs2_addr");
        sc_core::sc_trace(tf, rs1_data_sig, "rs1_data");
        sc_core::sc_trace(tf, rs2_data_sig, "rs2_data");
        sc_core::sc_trace(tf, write_enable_sig, "write_enable");
        sc_core::sc_trace(tf, rd_addr_sig, "rd_addr");
        sc_core::sc_trace(tf, write_data_sig, "write_data");
    }
    
    ~RegisterFileTestbench() {
        sc_core::sc_close_vcd_trace_file(tf);
        delete reg_file;
    }
    
    void clock_gen() {
        while (true) {
            clk_sig.write(false);
            wait(5, sc_core::SC_NS);
            clk_sig.write(true);
            wait(5, sc_core::SC_NS);
        }
    }
    
    void stimulus() {
        // 初始化信号
        reset_sig.write(true);
        write_enable_sig.write(false);
        rd_addr_sig.write(0);
        write_data_sig.write(0);
        rs1_addr_sig.write(0);
        rs2_addr_sig.write(0);
        
        // 等待复位完成
        wait(20, sc_core::SC_NS);
        reset_sig.write(false);
        wait(10, sc_core::SC_NS);
        
        std::cout << "Starting Register File tests..." << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        
        // 测试1: 写入并读取多个寄存器
        std::cout << "Test 1: Write and read multiple registers" << std::endl;
        
        // 写入数据到寄存器
        for (int i = 1; i <= 10; i++) {
            // 等待时钟上升沿
            wait(10, sc_core::SC_NS);
            
            rd_addr_sig.write(i);
            write_data_sig.write(i * 100);
            write_enable_sig.write(true);
            
            wait(10, sc_core::SC_NS);
            write_enable_sig.write(false);
        }
        
        // 读取并验证数据
        for (int i = 1; i <= 10; i++) {
            rs1_addr_sig.write(i);
            
            // 等待一个周期让读取完成
            wait(10, sc_core::SC_NS);
            
            std::cout << "Register x" << i << " = 0x" 
                      << std::hex << rs1_data_sig.read() << std::endl;
        }
        
        // 测试2: 确认x0始终为零
        std::cout << "\nTest 2: Verify x0 is always zero" << std::endl;
        
        // 尝试写入x0
        rd_addr_sig.write(0);
        write_data_sig.write(0xDEADBEEF);
        write_enable_sig.write(true);
        wait(10, sc_core::SC_NS);
        write_enable_sig.write(false);
        
        // 读取x0
        rs1_addr_sig.write(0);
        wait(10, sc_core::SC_NS);
        
        std::cout << "Register x0 after write attempt = 0x" 
                  << std::hex << rs1_data_sig.read() << std::endl;
        
        // 测试3: 并行读取
        std::cout << "\nTest 3: Parallel read from two registers" << std::endl;
        
        rs1_addr_sig.write(5);
        rs2_addr_sig.write(10);
        wait(10, sc_core::SC_NS);
        
        std::cout << "Register x5 = 0x" << std::hex << rs1_data_sig.read() << std::endl;
        std::cout << "Register x10 = 0x" << std::hex << rs2_data_sig.read() << std::endl;
        
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "Register File tests completed!" << std::endl;
        
        // 结束仿真
        sc_core::sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    RegisterFileTestbench tb("regfile_testbench");
    
    sc_core::sc_start();
    
    std::cout << "Simulation completed at " << sc_core::sc_time_stamp() << std::endl;
    
    return 0;
}
