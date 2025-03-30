# 综合测试：测试R型、I型、S型和分支指令
.text
.globl _start

_start:
    # 初始化寄存器
    li x1, 10
    li x2, 20
    li x3, 5
    li x4, 0x300     # 内存基址
    
    # 测试算术指令 (R-type)
    add x5, x1, x2   # x5 = 10 + 20 = 30
    sub x6, x2, x1   # x6 = 20 - 10 = 10
    and x7, x1, x3   # x7 = 10 & 5 = 0
    or  x8, x1, x3   # x8 = 10 | 5 = 15
    xor x9, x1, x3   # x9 = 10 ^ 5 = 15
    sll x10, x1, x3  # x10 = 10 << 5 = 320
    srl x11, x2, x3  # x11 = 20 >> 5 = 0
    slt x12, x1, x2  # x12 = (10 < 20) ? 1 : 0 = 1
    sltu x13, x2, x1 # x13 = (20 < 10) ? 1 : 0 = 0
    
    # 测试立即数指令 (I-type)
    addi x14, x1, 5  # x14 = 10 + 5 = 15
    xori x15, x1, 7  # x15 = 10 ^ 7 = 13
    ori  x16, x1, 7  # x16 = 10 | 7 = 15
    andi x17, x1, 6  # x17 = 10 & 6 = 2
    slli x18, x1, 2  # x18 = 10 << 2 = 40
    srli x19, x2, 2  # x19 = 20 >> 2 = 5
    
    # 存储结果
    sw x5, 0(x4)
    sw x6, 4(x4)
    sw x7, 8(x4)
    sw x8, 12(x4)
    sw x9, 16(x4)
    sw x10, 20(x4)
    sw x11, 24(x4)
    sw x12, 28(x4)
    sw x13, 32(x4)
    sw x14, 36(x4)
    sw x15, 40(x4)
    sw x16, 44(x4)
    sw x17, 48(x4)
    sw x18, 52(x4)
    sw x19, 56(x4)
    
    # 测试条件分支
    li x20, 0
    li x21, 1
    
    # 增加一个循环来测试分支指令
    li x22, 0        # 计数器
    li x23, 5        # 最大计数
    
loop_test:
    addi x22, x22, 1        # 增加计数器
    sw x22, 60(x4)          # 存储当前计数
    blt x22, x23, loop_test # 如果计数 < 5，则继续循环
    
    # 无限循环表示程序结束
loop:
    j loop
