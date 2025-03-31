# 数据类型规范文档

## 1. 概述

本文档定义了RISC-V单周期处理器TLM模型中使用的核心数据类型和常量。这些定义位于项目的`include`目录中，被所有处理器模块共享，确保类型和常量的一致性。

## 2. 基本数据类型

### 2.1 定义位置

基本数据类型定义在`include/types.h`文件中，包括：

```cpp
// 基本数据类型
typedef uint32_t Word;          // 32位字
typedef uint8_t Byte;           // 8位字节
typedef uint32_t Address;       // 内存地址
typedef uint8_t RegAddr;        // 寄存器地址(0-31)

// 复合数据类型
// 指令格式联合体，方便访问指令不同部分
union Instruction {
    Word value;                 // 完整的32位指令值
    
    // R-类型：寄存器-寄存器操作指令
    struct {
        uint32_t opcode : 7;    // [6:0]   - 操作码
        uint32_t rd : 5;        // [11:7]  - 目标寄存器
        uint32_t funct3 : 3;    // [14:12] - 功能码3
        uint32_t rs1 : 5;       // [19:15] - 源寄存器1
        uint32_t rs2 : 5;       // [24:20] - 源寄存器2
        uint32_t funct7 : 7;    // [31:25] - 功能码7
    } r;
    
    // I-类型：立即数操作和加载指令
    struct {
        uint32_t opcode : 7;    // [6:0]   - 操作码
        uint32_t rd : 5;        // [11:7]  - 目标寄存器
        uint32_t funct3 : 3;    // [14:12] - 功能码3
        uint32_t rs1 : 5;       // [19:15] - 源寄存器1
        uint32_t imm : 12;      // [31:20] - 12位立即数
    } i;
    
    // S-类型：存储指令
    struct {
        uint32_t opcode : 7;    // [6:0]   - 操作码
        uint32_t imm0 : 5;      // [11:7]  - 立即数[4:0]
        uint32_t funct3 : 3;    // [14:12] - 功能码3
        uint32_t rs1 : 5;       // [19:15] - 源寄存器1
        uint32_t rs2 : 5;       // [24:20] - 源寄存器2
        uint32_t imm1 : 7;      // [31:25] - 立即数[11:5]
    } s;
    
    // B-类型：分支指令
    struct {
        uint32_t opcode : 7;    // [6:0]   - 操作码
        uint32_t imm0 : 5;      // [11:7]  - 立即数部分
        uint32_t funct3 : 3;    // [14:12] - 功能码3
        uint32_t rs1 : 5;       // [19:15] - 源寄存器1
        uint32_t rs2 : 5;       // [24:20] - 源寄存器2
        uint32_t imm1 : 7;      // [31:25] - 立即数部分
    } b;
    
    // U-类型：上部立即数指令(LUI, AUIPC)
    struct {
        uint32_t opcode : 7;    // [6:0]   - 操作码
        uint32_t rd : 5;        // [11:7]  - 目标寄存器
        uint32_t imm : 20;      // [31:12] - 20位立即数
    } u;
    
    // J-类型：跳转指令(JAL)
    struct {
        uint32_t opcode : 7;    // [6:0]   - 操作码
        uint32_t rd : 5;        // [11:7]  - 目标寄存器
        uint32_t imm : 20;      // [31:12] - 复杂布局的20位立即数
    } j;
};
```

## 3. 常量定义

### 3.1 定义位置

常量定义在`include/constants.h`文件中，主要包括以下几类：

### 3.2 通用常量

```cpp
// 通用常量
#define REG_COUNT 32       // 寄存器数量
#define MEMORY_SIZE (16*1024*1024) // 内存大小: 16MB
```

### 3.3 操作码常量

```cpp
// RISC-V 操作码定义 (7位)
#define OP_LUI    0b0110111  // Load Upper Immediate
#define OP_AUIPC  0b0010111  // Add Upper Immediate to PC
#define OP_JAL    0b1101111  // Jump and Link
#define OP_JALR   0b1100111  // Jump and Link Register
#define OP_BRANCH 0b1100011  // 条件分支
#define OP_LOAD   0b0000011  // 加载指令
#define OP_STORE  0b0100011  // 存储指令
#define OP_IMM    0b0010011  // 立即数算术指令
#define OP_REG    0b0110011  // 寄存器-寄存器算术指令
```

### 3.4 功能码常量

```cpp
// 分支指令功能码 (funct3, 3位)
#define FUNCT3_BEQ  0b000   // 相等分支
#define FUNCT3_BNE  0b001   // 不等分支
#define FUNCT3_BLT  0b100   // 小于分支(有符号)
#define FUNCT3_BGE  0b101   // 大于等于分支(有符号)
#define FUNCT3_BLTU 0b110   // 小于分支(无符号)
#define FUNCT3_BGEU 0b111   // 大于等于分支(无符号)

// 加载指令功能码
#define FUNCT3_LB   0b000   // 加载字节(有符号扩展)
#define FUNCT3_LH   0b001   // 加载半字(有符号扩展)
#define FUNCT3_LW   0b010   // 加载字
#define FUNCT3_LBU  0b100   // 加载字节(无符号扩展)
#define FUNCT3_LHU  0b101   // 加载半字(无符号扩展)

// 存储指令功能码
#define FUNCT3_SB   0b000   // 存储字节
#define FUNCT3_SH   0b001   // 存储半字
#define FUNCT3_SW   0b010   // 存储字
```

### 3.5 ALU操作码常量

```cpp
// ALU操作码定义
#define ALU_ADD   0   // 加法
#define ALU_SUB   1   // 减法
#define ALU_AND   2   // 按位与
#define ALU_OR    3   // 按位或
#define ALU_XOR   4   // 按位异或
#define ALU_SLL   5   // 逻辑左移
#define ALU_SRL   6   // 逻辑右移
#define ALU_SRA   7   // 算术右移
#define ALU_SLT   8   // 有符号小于设置
#define ALU_SLTU  9   // 无符号小于设置
```

## 4. 数据类型使用指南

### 4.1 类型选择建议

在开发RISC-V处理器模块时，应遵循以下类型使用原则：

1. **使用类型别名而非原始类型**：例如使用`Word`而非直接使用`uint32_t`
2. **保持类型一致性**：在模块间接口传递数据时尊重已定义的类型
3. **使用指令联合体**：处理指令解码时使用`Instruction`联合体以简化代码

### 4.2 常量使用建议

在使用常量时注意：

1. **使用符号常量而非硬编码值**：例如使用`OP_LUI`而非直接使用`0b0110111`
2. **检查常量定义完整性**：确保实现的指令集所需的所有常量都已定义
3. **遵循命名约定**：使用大写字母和下划线命名常量

## 5. 扩展考虑

### 5.1 支持扩展指令集

当需要支持RISC-V扩展指令集（如M、F、D扩展）时，需要在`types.h`和`constants.h`中添加相应的类型和常量定义。例如：

```cpp
// M扩展指令功能码
#define FUNCT3_MUL    0b000  // 乘法
#define FUNCT3_MULH   0b001  // 乘法高位(有符号×有符号)
#define FUNCT3_MULHSU 0b010  // 乘法高位(有符号×无符号)
#define FUNCT3_MULHU  0b011  // 乘法高位(无符号×无符号)
#define FUNCT3_DIV    0b100  // 除法(有符号)
#define FUNCT3_DIVU   0b101  // 除法(无符号)
#define FUNCT3_REM    0b110  // 余数(有符号)
#define FUNCT3_REMU   0b111  // 余数(无符号)
```

### 5.2 64位支持

若需支持RV64I指令集，需要修改基本数据类型：

```cpp
typedef uint64_t Word;          // 修改为64位字
typedef uint32_t HalfWord;      // 添加32位半字类型
```

## 6. RTL设计考虑

在将TLM模型转换为RTL设计时，这些数据类型定义可以作为Verilog/VHDL参数和类型定义的基础。例如：

```verilog
// Verilog等价定义
`define REG_COUNT 32
`define ALU_ADD   4'b0000
`define ALU_SUB   4'b0001
// ... 其他常量
```

## 7. 参考资源

- RISC-V 规范文档
- SystemC 数据类型指南
- RTL设计最佳实践