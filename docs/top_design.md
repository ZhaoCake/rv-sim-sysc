# 顶层（Top）模块设计文档

## 1. 模块概述

顶层模块是RISC-V单周期处理器的集成中枢，负责实例化、连接和协调所有处理器子模块，形成一个完整的处理器系统。它提供外部接口，管理模块间的信号连接，并协调处理器运行中的控制流。顶层模块还支持一些系统级功能，如程序加载、调试信息输出等。

## 2. 功能描述

顶层模块的主要功能包括：

1. 实例化所有处理器核心模块（Fetch、Decode、Execute、RegisterFile、Memory）
2. 连接各模块间的接口信号
3. 协调处理器运行过程中的控制流
4. 提供对系统状态的全局观察和控制能力
5. 支持程序加载和执行
6. 提供调试和监控接口

## 3. 接口定义

### 3.1 外部接口

| 端口名称  | 类型          | 描述                    |
|---------|---------------|------------------------|
| clk     | sc_in<bool>   | 系统时钟信号             |
| reset   | sc_in<bool>   | 系统复位信号             |

### 3.2 公共方法

| 方法名称       | 参数                              | 返回值 | 描述                      |
|--------------|-----------------------------------|------|--------------------------|
| load_program | const std::string& filename       | bool | 加载程序到处理器             |
| dump_registers| (无)                              | void | 打印寄存器文件的内容        |
| dump_memory  | std::string filename, uint32_t start, uint32_t size | void | 将内存内容导出到文件 |

## 4. 内部结构

### 4.1 模块实例

顶层模块包含以下子模块实例：

| 模块名称      | 类型          | 描述                     |
|-------------|---------------|-------------------------|
| fetch       | Fetch         | 取指模块                 |
| decode      | Decode        | 译码模块                 |
| execute     | Execute       | 执行模块                 |
| register_file| RegisterFile  | 寄存器文件模块           |
| memory      | Memory        | 存储器模块               |

### 4.2 内部信号

顶层模块维护众多内部信号用于连接各子模块。主要信号可分类为：

#### 4.2.1 Fetch <-> Memory

| 信号名称      | 类型          | 描述                     |
|-------------|---------------|-------------------------|
| rom_read_sig   | sc_signal<bool> | 指令存储器读取请求    |
| rom_address_sig| sc_signal<Word> | 指令存储器地址         |
| rom_data_sig   | sc_signal<Word> | 指令存储器数据         |

#### 4.2.2 Fetch <-> Decode

| 信号名称        | 类型           | 描述                     |
|---------------|----------------|-------------------------|
| pc_sig        | sc_signal<Word>| 程序计数器值              |
| next_pc_sig   | sc_signal<Word>| 下一条指令的地址          |
| instruction_sig| sc_signal<Word>| 获取的指令               |

#### 4.2.3 Execute -> Fetch

| 信号名称        | 类型            | 描述                     |
|---------------|-----------------|-------------------------|
| branch_taken_sig | sc_signal<bool> | 分支/跳转是否有效        |
| branch_target_sig| sc_signal<Word> | 分支/跳转目标地址        |

#### 4.2.4 Decode <-> RegisterFile

| 信号名称      | 类型                | 描述                     |
|-------------|---------------------|-------------------------|
| rs1_addr_sig | sc_signal<RegAddr>  | 源寄存器1地址            |
| rs1_data_sig | sc_signal<Word>     | 源寄存器1数据            |
| rs2_addr_sig | sc_signal<RegAddr>  | 源寄存器2地址            |
| rs2_data_sig | sc_signal<Word>     | 源寄存器2数据            |

#### 4.2.5 Decode -> Execute 

包括控制信号和数据传递信号，如ALU操作码、立即数、寄存器值等。

#### 4.2.6 Execute -> Memory/WriteBack

包括执行结果、内存访问请求等。

### 4.3 处理阶段

单周期处理器在顶层模块中实现以下主要处理阶段：

1. **取指（Fetch）阶段**：获取下一条指令
2. **译码（Decode）阶段**：解析指令并生成控制信号
3. **执行（Execute）阶段**：执行ALU操作、评估分支条件
4. **访存（Memory）阶段**：执行数据内存访问
5. **写回（WriteBack）阶段**：将结果写回寄存器

在单周期实现中，这些阶段在同一个时钟周期内完成，通过组合逻辑直接串联。

## 5. 行为模型

顶层模块实现了若干SystemC方法来协调处理器各阶段：

### 5.1 Memory访问阶段

```cpp
// 伪代码描述
void memory_access_process() {
    // 转发Execute模块的结果到Memory模块
    ram_address_sig.write(alu_result_sig.read());
    ram_write_data_sig.write(mem_write_data_sig.read());
    ram_read_sig.write(mem_read_out_sig.read());
    ram_write_sig.write(mem_write_out_sig.read());
}
```

### 5.2 写回阶段

```cpp
// 伪代码描述
void writeback_process() {
    // 决定写回寄存器的数据来源
    if (reg_write_out_sig.read()) {
        reg_write_wb_sig.write(true);
        rd_addr_wb_sig.write(rd_addr_out_sig.read());
        
        if (mem_to_reg_out_sig.read()) {
            // 从内存写回
            rd_data_wb_sig.write(ram_read_data_sig.read());
        } else {
            // 从ALU写回
            rd_data_wb_sig.write(alu_result_sig.read());
        }
    } else {
        reg_write_wb_sig.write(false);
    }
}
```

### 5.3 调试过程

```cpp
// 伪代码描述
void debug_process() {
    // 打印每个周期的调试信息
    static int cycle_count = 0;
    
    if (clk.read()) {
        std::cout << "======== Cycle " << ++cycle_count << " ========" << std::endl;
    }
}
```

## 6. 模块连接策略

顶层模块在构造函数中完成所有子模块的实例化和端口连接：

```cpp
// 伪代码描述
Top::Top(sc_core::sc_module_name name) : sc_module(name) {
    // 创建模块实例
    fetch = new Fetch("fetch");
    decode = new Decode("decode");
    execute = new Execute("execute");
    register_file = new RegisterFile("register_file");
    memory = new Memory("memory");
    
    // 连接Fetch模块
    fetch->clk(clk);
    fetch->reset(reset);
    // ... 其他连接
    
    // 连接Decode模块
    decode->clk(clk);
    decode->reset(reset);
    // ... 其他连接
    
    // 连接Execute模块
    // ... 省略
    
    // 连接RegisterFile模块
    // ... 省略
    
    // 连接Memory模块
    // ... 省略
    
    // 注册处理方法
    SC_METHOD(memory_access_process);
    sensitive << alu_result_sig << mem_write_data_sig << mem_read_out_sig << mem_write_out_sig;
    
    SC_METHOD(writeback_process);
    sensitive << reg_write_out_sig << mem_to_reg_out_sig << rd_addr_out_sig 
              << alu_result_sig << ram_read_data_sig;
}
```

## 7. 系统功能支持

### 7.1 程序加载

顶层模块委托Memory模块加载程序：

```cpp
bool Top::load_program(const std::string& filename) {
    return memory->load_binary(filename);
}
```

### 7.2 状态观察

顶层模块提供访问寄存器和内存状态的方法：

```cpp
void Top::dump_registers() const {
    register_file->dump_registers();
}

void Top::dump_memory(const std::string& filename, uint32_t start, uint32_t size) const {
    memory->dump_memory(filename, start, size);
}
```

## 8. 性能特性

### 8.1 时序特性

- **周期性能**：在单周期实现中，指令执行需要一个完整时钟周期
- **关键路径**：取指->译码->执行->访存->写回的组合逻辑路径

### 8.2 资源需求

- 各子模块所需的资源
- 模块间连接的信号资源
- 处理阶段逻辑资源

## 9. 扩展性考虑

### 9.1 潜在扩展

- **多周期实现**：转换为多周期架构，每个阶段使用独立的时钟周期
- **流水线实现**：添加流水线寄存器，实现指令级并行执行
- **中断支持**：添加中断控制器和处理逻辑
- **特权模式**：支持RISC-V特权级架构

### 9.2 RTL实现考虑

在RTL实现时需要特别关注：
- 各阶段时序约束的满足
- 控制信号的正确传播
- 数据通路的时序分析
- 流水线冲突的处理

## 10. 验证策略

对顶层模块的验证主要包括：
- 集成测试，验证各模块间接口正确连接
- 系统级功能测试，运行完整的测试程序
- 边界条件测试，如复位、异常情况等
- 性能测试，确认系统满足时序要求

## 11. 参考资源

- RISC-V规范
- 计算机体系结构设计方法学
- SystemC TLM设计和建模指南