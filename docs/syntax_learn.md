# SystemC 语法记录

## 1. 核心概念

SystemC是一个C++库，用于系统级建模、设计和验证。它扩展了C++，增加了硬件描述语言的功能。

### 1.1 基本结构

SystemC程序的基本结构包括:
```cpp
#include <systemc>

// 使用SystemC命名空间
using namespace sc_core;

// 定义模块
SC_MODULE(ModuleName) {
    // 端口声明
    sc_in<bool> input1;
    sc_out<int> output1;
    
    // 内部信号
    sc_signal<bool> internal_signal;
    
    // 子模块
    SubModule* submodule;
    
    // 构造函数
    SC_CTOR(ModuleName) {
        // 注册进程
        SC_METHOD(process_method);
        sensitive << input1;  // 敏感列表
        
        // 实例化子模块
        submodule = new SubModule("submodule_instance");
        // 连接子模块
        submodule->port1(signal1);
    }
    
    // 进程定义
    void process_method() {
        // 进程行为
    }
};

// 主函数
int sc_main(int argc, char* argv[]) {
    // 实例化顶层模块
    ModuleName top("top_instance");
    
    // 开始仿真
    sc_start(100, SC_NS);
    
    return 0;
}
```

## 2. 模块和端口

### 2.1 模块定义

模块是SystemC的基本构建块，类似于硬件设计中的组件：

```cpp
SC_MODULE(Counter) {
    // 模块定义
    // ...
};
```

### 2.2 端口类型

SystemC提供多种端口类型：

- `sc_in<T>`: 输入端口
- `sc_out<T>`: 输出端口
- `sc_inout<T>`: 双向端口
- `sc_port<IF>`: 接口端口

```cpp
sc_in<bool> clock;
sc_out<sc_uint<8>> data_out;
```

## 3. 进程和敏感列表

SystemC支持三种进程类型：

### 3.1 SC_METHOD

不消耗仿真时间的进程，每当敏感事件发生时执行：

```cpp
SC_MODULE(Example) {
    sc_in<bool> clock;
    
    SC_CTOR(Example) {
        SC_METHOD(process);
        sensitive << clock.pos();  // 对时钟上升沿敏感
    }
    
    void process() {
        // 逻辑代码
    }
};
```

### 3.2 SC_THREAD

只启动一次，可以使用wait()语句暂停执行：

```cpp
SC_MODULE(Example) {
    sc_in<bool> clock;
    
    SC_CTOR(Example) {
        SC_THREAD(process);
        sensitive << clock.pos();
    }
    
    void process() {
        while(true) {
            // 处理逻辑
            wait();  // 等待下一个时钟周期
        }
    }
};
```

### 3.3 SC_CTHREAD

专门为时钟设计的线程，仅在指定时钟边沿执行：

```cpp
SC_MODULE(Example) {
    sc_in<bool> clock;
    
    SC_CTOR(Example) {
        SC_CTHREAD(process, clock.pos());
    }
    
    void process() {
        while(true) {
            // 处理逻辑
            wait();  // 等待下一个时钟上升沿
        }
    }
};
```

## 4. 数据类型

SystemC提供多种特殊数据类型：

- `sc_bit`: 单比特
- `sc_bv<W>`: 比特向量
- `sc_int<W>`, `sc_uint<W>`: 有符号和无符号整数
- `sc_bigint<W>`, `sc_biguint<W>`: 大整数类型
- `sc_fixed<W,I>`, `sc_ufixed<W,I>`: 定点数

```cpp
sc_uint<8> counter;  // 8位无符号整数
sc_int<16> value;    // 16位有符号整数
sc_bv<32> bus;       // 32位比特向量
```

## 5. 信号和通信

### 5.1 信号定义

信号用于模块间通信：

```cpp
sc_signal<bool> control_signal;
sc_signal<sc_uint<8>> data_bus;
```

### 5.2 读写信号

```cpp
// 读信号
bool value = signal.read();

// 写信号
signal.write(new_value);
```

## 6. 仿真控制

### 6.1 启动仿真

```cpp
// 运行特定时间
sc_start(100, SC_NS);

// 无限运行直到停止
sc_start();
```

### 6.2 停止仿真

```cpp
sc_stop();  // 停止仿真
```

### 6.3 时间管理

```cpp
sc_time current_time = sc_time_stamp();  // 获取当前仿真时间
sc_time delay(10, SC_NS);                // 10纳秒延迟
```

## 7. 调试技术

### 7.1 跟踪信号

```cpp
sc_trace_file* tf = sc_create_vcd_trace_file("wave");
sc_trace(tf, signal, "signal_name");  // 跟踪信号
```

### 7.2 打印信息

```cpp
SC_REPORT_INFO("ID", "Information message");
SC_REPORT_WARNING("ID", "Warning message");
SC_REPORT_ERROR("ID", "Error message");
SC_REPORT_FATAL("ID", "Fatal error message");
```

