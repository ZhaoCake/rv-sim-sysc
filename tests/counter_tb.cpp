#include <systemc>
#include <iostream>

// 简单的计数器模块
SC_MODULE(Counter) {
    sc_core::sc_in<bool> clk;
    sc_core::sc_in<bool> reset;
    sc_core::sc_out<sc_dt::sc_uint<8>> count_out;
    
    SC_CTOR(Counter) {
        SC_METHOD(count);
        sensitive << clk.pos() << reset;
        
        m_count = 0;
    }
    
    void count() {
        if (reset.read()) {
            m_count = 0;
        } else {
            m_count++;
        }
        
        count_out.write(m_count);
    }
    
    private:
        sc_dt::sc_uint<8> m_count;
};

// 测试台模块
SC_MODULE(CounterTestbench) {
    sc_core::sc_signal<bool> clk_sig;
    sc_core::sc_signal<bool> reset_sig;
    sc_core::sc_signal<sc_dt::sc_uint<8>> count_sig;
    
    Counter* counter;
    sc_core::sc_trace_file* tf;
    
    SC_CTOR(CounterTestbench) {
        // 实例化被测模块
        counter = new Counter("counter_instance");
        counter->clk(clk_sig);
        counter->reset(reset_sig);
        counter->count_out(count_sig);
        
        SC_THREAD(clock_gen);
        SC_THREAD(stimulus);
        
        // 创建波形文件
        tf = sc_core::sc_create_vcd_trace_file("counter_waveform");
        sc_core::sc_trace(tf, clk_sig, "clk");
        sc_core::sc_trace(tf, reset_sig, "reset");
        sc_core::sc_trace(tf, count_sig, "count");
    }
    
    ~CounterTestbench() {
        sc_core::sc_close_vcd_trace_file(tf);
        delete counter;
    }
    
    void clock_gen() {
        // 生成时钟
        while (true) {
            clk_sig.write(false);
            wait(5, sc_core::SC_NS);
            clk_sig.write(true);
            wait(5, sc_core::SC_NS);
        }
    }
    
    void stimulus() {
        // 测试激励
        reset_sig.write(true);
        wait(10, sc_core::SC_NS);
        reset_sig.write(false);
        
        // 让计数器运行一段时间
        wait(200, sc_core::SC_NS);
        
        std::cout << "Final counter value: " << count_sig.read() << std::endl;
        
        // 结束仿真
        sc_core::sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    CounterTestbench tb("counter_testbench");
    
    std::cout << "Starting counter testbench..." << std::endl;
    sc_core::sc_start();
    std::cout << "Simulation completed at " << sc_core::sc_time_stamp() << std::endl;
    
    return 0;
}
