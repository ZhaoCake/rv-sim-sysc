# 存储器（Memory）模块设计文档

## 1. 模块概述

存储器模块是RISC-V处理器中负责指令存储和数据存储的关键组件。在此单周期处理器设计中，它同时承担指令存储器（ROM）和数据存储器（RAM）的功能，提供统一的存储接口。存储器模块不仅支持处理器的正常指令获取和数据访问操作，还提供程序加载、内存状态检查等调试功能。

## 2. 功能描述

存储器模块的主要功能包括：

1. 指令存储区：存储和提供程序指令
2. 数据存储区：支持数据读写操作
3. 支持字节寻址的内存访问
4. 提供程序加载功能
5. 支持内存状态导出和调试

## 3. 接口定义

### 3.1 指令存储器接口

| 端口名称     | 类型             | 描述                     |
|------------|------------------|--------------------------|
| rom_read   | sc_in<bool>      | 指令读取请求               |
| rom_address| sc_in<Word>      | 指令读取地址               |
| rom_data   | sc_out<Word>     | 读出的指令数据             |

### 3.2 数据存储器接口

| 端口名称       | 类型             | 描述                     |
|--------------|------------------|--------------------------|
| ram_read     | sc_in<bool>      | 数据读取请求               |
| ram_write    | sc_in<bool>      | 数据写入请求               |
| ram_address  | sc_in<Word>      | 数据访问地址               |
| ram_write_data| sc_in<Word>     | 要写入的数据               |
| ram_read_data| sc_out<Word>     | 读出的数据                |

## 4. 内部结构

### 4.1 存储器组织

整个存储器空间被划分为以下几个区域：

| 地址范围              | 用途                  | 特性              |
|---------------------|----------------------|------------------|
| 0x00000000-0x00FFFFFF | 代码段（指令存储区）    | 可读（可执行）      |
| 0x01000000-0x01FFFFFF | 数据段                | 可读可写          |
| 0x02000000-0x02FFFFFF | 栈段                  | 可读可写          |
| 0xF0000000-0xFFFFFFFF | 内存映射I/O           | 可读可写          |

### 4.2 内存实现

存储器内部采用标准字节数组实现：

```cpp
// 伪代码描述
uint8_t memory[MEMORY_SIZE]; // 通常以MB为单位
```

### 4.3 内存访问方法

内存模块支持字节、半字和字级别的访问操作，通过适当的地址解码和数据组合实现。

## 5. 行为模型

### 5.1 指令读取

指令读取过程是组合逻辑实现的：

```cpp
// 伪代码描述
void handle_rom_read() {
    if (rom_read.read()) {
        Word address = rom_address.read();
        // 检查地址是否合法
        if (is_valid_address(address)) {
            Word instr = read_word(address);
            rom_data.write(instr);
        } else {
            // 非法地址访问处理
            rom_data.write(0); // 返回NOP指令
            std::cerr << "Error: Invalid instruction address: 0x" << std::hex << address << std::endl;
        }
    }
}
```

### 5.2 数据读写

数据读写过程同样是组合逻辑实现：

```cpp
// 伪代码描述
void handle_ram_access() {
    Word address = ram_address.read();
    
    if (ram_read.read()) {
        // 数据读取
        if (is_valid_address(address)) {
            Word data = read_word(address);
            ram_read_data.write(data);
        } else {
            // 非法地址读取处理
            ram_read_data.write(0);
            std::cerr << "Error: Invalid data read address: 0x" << std::hex << address << std::endl;
        }
    }
    
    if (ram_write.read()) {
        // 数据写入
        if (is_valid_address(address) && is_writable_address(address)) {
            Word data = ram_write_data.read();
            write_word(address, data);
        } else {
            // 非法地址写入处理
            std::cerr << "Error: Invalid data write address: 0x" << std::hex << address << std::endl;
        }
    }
}
```

### 5.3 内存访问辅助函数

为支持不同位宽的内存访问，提供以下辅助函数：

```cpp
// 伪代码描述
// 读取一个字（4字节对齐）
Word read_word(Word address) {
    Word value = 0;
    // 小端序读取
    value |= memory[address + 0];
    value |= memory[address + 1] << 8;
    value |= memory[address + 2] << 16;
    value |= memory[address + 3] << 24;
    return value;
}

// 写入一个字（4字节对齐）
void write_word(Word address, Word value) {
    // 小端序写入
    memory[address + 0] = (value >> 0) & 0xFF;
    memory[address + 1] = (value >> 8) & 0xFF;
    memory[address + 2] = (value >> 16) & 0xFF;
    memory[address + 3] = (value >> 24) & 0xFF;
}
```

## 6. 功能扩展

### 6.1 程序加载功能

存储器模块提供加载二进制程序到指令存储区的功能：

```cpp
// 伪代码描述
bool load_binary(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Cannot open file: " << filename << std::endl;
        return false;
    }
    
    // 从文件读取到内存
    file.read(reinterpret_cast<char*>(memory), MEMORY_SIZE);
    std::cout << "Loaded " << file.gcount() << " bytes from " << filename << std::endl;
    
    file.close();
    return true;
}
```

### 6.2 内存状态导出

为支持调试，提供将内存内容导出到文件的功能：

```cpp
// 伪代码描述
void dump_memory(const std::string& filename, uint32_t start, uint32_t size) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Cannot create file: " << filename << std::endl;
        return;
    }
    
    // 限制范围在有效内存区域内
    size = std::min(size, MEMORY_SIZE - start);
    
    // 写入文件
    file.write(reinterpret_cast<const char*>(memory + start), size);
    std::cout << "Dumped " << size << " bytes to " << filename << std::endl;
    
    file.close();
}
```

## 7. 性能特性

### 7.1 时序特性

- **访问延迟**：作为TLM模型，当前实现假设内存访问无延迟，在同周期完成
- **并发访问**：支持在同一周期分别访问指令和数据存储区

### 7.2 资源需求

- 大小可配置的内存空间
- 地址解码和访问控制逻辑

## 8. 与其他模块的接口

### 8.1 与取指模块的接口

存储器模块向取指模块提供指令：
- 输入：`rom_read`, `rom_address` - 指令访问请求
- 输出：`rom_data` - 指令内容

### 8.2 与执行/访存阶段的接口

存储器模块支持执行阶段的数据访问操作：
- 输入：`ram_read`, `ram_write`, `ram_address`, `ram_write_data` - 数据访问请求
- 输出：`ram_read_data` - 读取的数据

## 9. 扩展性考虑

### 9.1 潜在扩展

- **缓存实现**：添加指令和数据缓存层
- **内存管理单元**：集成MMU支持虚拟内存
- **多层次存储结构**：差异化的延迟模型
- **内存保护机制**：实现基于特权级的访问控制

### 9.2 RTL实现考虑

在RTL实现中，内存通常会被实现为SRAM或BRAM块：
- 分离的指令和数据存储器
- 可配置的存储器宽度和深度
- 访问延迟的真实建模
- 时序约束和时钟域考虑

## 10. 验证策略

对存储器模块的验证主要包括：
- 验证指令的正确读取
- 验证数据的读写一致性
- 验证边界情况（地址边界、非对齐访问等）
- 验证内存保护规则
- 验证程序加载和内存导出功能

## 11. 参考资源

- RISC-V特权级规范中的内存模型
- SystemC存储器建模指南
- 标准内存访问协议