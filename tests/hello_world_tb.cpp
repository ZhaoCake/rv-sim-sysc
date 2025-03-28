#include <systemc>
#include <iostream>

// 简单的Hello World模块
SC_MODULE(HelloWorld) {
    SC_CTOR(HelloWorld) {
        SC_METHOD(hello);
        sensitive << dummy;  // 让方法至少有一个敏感事件
        
        // 也可以使用dont_initialize()使方法不在开始时运行
        // dont_initialize();
    }
    
    void hello() {
        std::cout << "Hello World from SystemC!" << std::endl;
        std::cout << "Current simulation time: " << sc_core::sc_time_stamp() << std::endl;
    }
    
    sc_core::sc_signal<bool> dummy;  // 只是为了有一个敏感信号
};

// SystemC主函数
int sc_main(int argc, char* argv[]) {
    HelloWorld hello("hello_world_instance");
    
    std::cout << "Starting simulation..." << std::endl;
    sc_core::sc_start(1, sc_core::SC_NS);
    std::cout << "Simulation finished." << std::endl;
    
    return 0;
}
