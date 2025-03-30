# 简单的加法测试
.text
.globl _start

_start:
    # 初始化寄存器
    li x1, 10        # x1 = 10
    li x2, 20        # x2 = 20
    li x3, 30        # x3 = 30
    li x4, 0         # 初始化结果寄存器为0
    
    # 执行一系列加法
    add x4, x1, x2   # x4 = x1 + x2 = 30
    add x5, x4, x3   # x5 = x4 + x3 = 60
    addi x6, x5, 5   # x6 = x5 + 5 = 65
    
    # 存储结果到内存
    li x7, 0x100     # 结果存储地址
    sw x4, 0(x7)     # 存储 x4 (30)
    sw x5, 4(x7)     # 存储 x5 (60)
    sw x6, 8(x7)     # 存储 x6 (65)
    
    # 无限循环表示程序结束
loop:
    j loop
