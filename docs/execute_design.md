# 执行（Execute）模块设计文档

## 1. 模块概述

执行模块是RISC-V处理器中负责进行算术逻辑计算、分支条件评估和内存地址计算的核心组件。它接收来自译码模块的操作数和控制信号，执行指定的操作，并将结果提供给后续阶段。

## 2. 功能描述

执行模块的主要功能包括：

1. 执行算术逻辑单元(ALU)操作
2. 评估分支条件和生成分支目标地址
3. 计算访存指令的内存地址
4. 处理跳转指令并计算跳转目标
5. 将控制信号和处理结果转发给后续阶段

## 3. 接口定义

### 3.1 输入端口

| 端口名称     | 类型                 | 描述                     |
|------------|---------------------|--------------------------|
| clk        | sc_in<bool>         | 时钟信号                   |
| reset      | sc_in<bool>         | 复位信号                   |
| pc         | sc_in<Word>         | 当前指令程序计数器           |
| rs1_value  | sc_in<Word>         | 源寄存器1的值               |
| rs2_value  | sc_in<Word>         | 源寄存器2的值               |
| imm        | sc_in<Word>         | 立即数值                   |
| rd_addr    | sc_in<RegAddr>      | 目标寄存器地址              |
| alu_op     | sc_in<sc_uint<4>>   | ALU操作码                  |
| alu_src    | sc_in<bool>         | ALU源操作数选择             |
| branch     | sc_in<bool>         | 分支指令标识                |
| jump       | sc_in<bool>         | 跳转指令标识                |
| mem_read   | sc_in<bool>         | 内存读使能                 |
| mem_write  | sc_in<bool>         | 内存写使能                 |
| reg_write  | sc_in<bool>         | 寄存器写使能                |
| mem_to_reg | sc_in<bool>         | 结果选择信号                |

### 3.2 输出端口

| 端口名称        | 类型             | 描述                      |
|---------------|------------------|--------------------------|
| alu_result    | sc_out<Word>     | ALU计算结果                |
| branch_taken  | sc_out<bool>     | 分支是否需要跳转            |
| branch_target | sc_out<Word>     | 分支跳转目标地址            |
| mem_write_data| sc_out<Word>     | 写入内存的数据              |
| mem_read_out  | sc_out<bool>     | 转发的内存读信号            |
| mem_write_out | sc_out<bool>     | 转发的内存写信号            |
| reg_write_out | sc_out<bool>     | 转发的寄存器写信号          |
| mem_to_reg_out| sc_out<bool>     | 转发的结果选择信号          |
| rd_addr_out   | sc_out<RegAddr>  | 转发的目标寄存器地址        |

## 4. 内部结构

### 4.1 算术逻辑单元(ALU)

ALU是执行模块的核心组件，支持多种操作，包括加、减、按位逻辑运算、移位和比较。ALU操作由译码模块提供的`alu_op`信号控制。

#### 4.1.1 ALU操作码定义

| 操作码 | 操作     | 描述           |
|-------|---------|----------------|
| 0     | ADD     | 加法            |
| 1     | SUB     | 减法            |
| 2     | AND     | 按位与          |
| 3     | OR      | 按位或          |
| 4     | XOR     | 按位异或        |
| 5     | SLL     | 逻辑左移        |
| 6     | SRL     | 逻辑右移        |
| 7     | SRA     | 算术右移        |
| 8     | SLT     | 有符号小于比较   |
| 9     | SLTU    | 无符号小于比较   |

### 4.2 分支条件评估

执行模块根据ALU比较结果和分支指令的类型（BEQ、BNE、BLT等）评估分支条件。如果条件满足，`branch_taken`信号被置位，并计算分支目标地址。

### 4.3 操作数选择

执行模块根据`alu_src`信号选择第二个操作数：
- `alu_src = 0`：使用寄存器值(`rs2_value`)
- `alu_src = 1`：使用立即数(`imm`)

## 5. 行为模型

执行模块的处理流程主要由`execute_process()`方法实现：

```cpp
// 伪代码描述
void execute_process() {
    // 转发控制信号
    mem_read_out.write(mem_read.read());
    mem_write_out.write(mem_write.read());
    reg_write_out.write(reg_write.read());
    mem_to_reg_out.write(mem_to_reg.read());
    rd_addr_out.write(rd_addr.read());
    
    // 转发存储器写数据
    mem_write_data.write(rs2_value.read());
    
    // 选择ALU操作数
    Word op1 = rs1_value.read();
    Word op2 = alu_src.read() ? imm.read() : rs2_value.read();
    
    // 特殊情况：AUIPC指令需要使用PC作为第一操作数
    if (特殊条件: AUIPC指令) {
        op1 = pc.read();
    }
    
    // 执行ALU操作
    Word result = alu_compute(op1, op2, alu_op.read());
    alu_result.write(result);
    
    // 计算分支目标地址
    branch_target.write(pc.read() + imm.read());
    
    // 确定分支是否跳转
    bool is_branch_taken = false;
    
    if (jump.read()) {
        // 跳转指令总是跳转
        is_branch_taken = true;
        
        // JALR特殊处理
        if (JALR条件) {
            branch_target.write((op1 + op2) & ~0x1); // 清除LSB
        }
    } 
    else if (branch.read()) {
        // 条件分支
        uint32_t funct3 = (alu_op.read() & 0x7); // 使用alu_op低3位作为分支类型
        is_branch_taken = evaluate_branch(op1, op2, funct3);
    }
    
    branch_taken.write(is_branch_taken);
}
```

### 5.1 ALU计算

ALU计算由辅助方法`alu_compute()`实现：

```cpp
// 伪代码描述
Word alu_compute(Word a, Word b, sc_uint<4> op) {
    switch (op) {
        case ALU_ADD: return a + b;
        case ALU_SUB: return a - b;
        case ALU_AND: return a & b;
        case ALU_OR:  return a | b;
        case ALU_XOR: return a ^ b;
        case ALU_SLL: return a << (b & 0x1F); // 左移（仅使用低5位）
        case ALU_SRL: return a >> (b & 0x1F); // 逻辑右移
        case ALU_SRA: // 算术右移，保留符号位
            return (int32_t)a >> (b & 0x1F);
        case ALU_SLT: // 有符号比较
            return ((int32_t)a < (int32_t)b) ? 1 : 0;
        case ALU_SLTU: // 无符号比较
            return (a < b) ? 1 : 0;
        default:
            return 0;
    }
}
```

### 5.2 分支条件评估

分支条件评估由辅助方法`evaluate_branch()`实现：

```cpp
// 伪代码描述
bool evaluate_branch(Word a, Word b, uint32_t funct3) {
    switch (funct3) {
        case FUNCT3_BEQ:  return a == b;           // 相等分支
        case FUNCT3_BNE:  return a != b;           // 不等分支
        case FUNCT3_BLT:  return (int32_t)a < (int32_t)b;  // 有符号小于分支
        case FUNCT3_BGE:  return (int32_t)a >= (int32_t)b; // 有符号大于等于分支
        case FUNCT3_BLTU: return a < b;            // 无符号小于分支
        case FUNCT3_BGEU: return a >= b;           // 无符号大于等于分支
        default:
            return false;
    }
}
```

## 6. 性能特性

### 6.1 时序特性

- ALU操作和分支条件评估是关键路径
- 对于AUIPC和JALR等特殊指令需要特殊处理流程

### 6.2 资源需求

- ALU硬件资源（加法器、移位器、逻辑单元等）
- 分支评估电路
- 控制信号处理逻辑

## 7. 与其他模块的接口

### 7.1 与译码模块的接口

执行模块从译码模块接收以下信息：
- 操作数：`rs1_value`, `rs2_value`, `imm`, `pc`
- 控制信号：`alu_op`, `alu_src`, `branch`, `jump`, 等

### 7.2 与取指模块的接口

执行模块向取指模块提供以下信号：
- `branch_taken`: 是否需要分支/跳转
- `branch_target`: 分支/跳转目标地址

### 7.3 与存储器模块的接口

执行模块向存储器模块提供：
- `alu_result`: 作为内存访问地址
- `mem_read_out`, `mem_write_out`: 访存控制信号
- `mem_write_data`: 写入内存的数据

## 8. 扩展性考虑

### 8.1 潜在扩展

- **支持更多ALU操作**：为支持M扩展（乘除法）等添加更多ALU操作
- **前向数据通路**：为流水线设计添加数据前向通路解决数据相关
- **执行结果预测**：支持更复杂的分支预测机制

### 8.2 RTL实现考虑

- ALU实现可以根据性能需求选择串行或并行结构
- 分支评估逻辑可以优化以减少关键路径延迟
- 控制信号流水线可以优化以提高时钟频率

## 9. 验证策略

对执行模块的验证主要包括：
- 验证所有ALU操作的正确性
- 验证分支条件评估的正确性
- 验证特殊指令（AUIPC、JALR等）的处理正确性
- 验证控制信号的正确转发

## 10. 参考资源

- RISC-V规范中的ALU操作定义
- RISC-V分支指令编码与行为
- 标准ALU设计参考