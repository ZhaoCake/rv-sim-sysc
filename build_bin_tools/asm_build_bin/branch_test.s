# 测试分支指令
.text
.globl _start

_start:
    # 初始化寄存器
    li x1, 10
    li x2, 20
    li x3, 10
    li x4, 0
    li x5, 0
    li x6, 0
    li x7, 0
    li x8, 0x100     # 存储结果的地址
    
    # 测试 beq (相等分支)
    beq x1, x3, beq_true
    li x4, 0         # 如果不跳转，x4 = 0
    j beq_next
beq_true:
    li x4, 1         # 如果跳转，x4 = 1
beq_next:
    
    # 测试 bne (不等分支)
    bne x1, x2, bne_true
    li x5, 0         # 如果不跳转，x5 = 0
    j bne_next
bne_true:
    li x5, 1         # 如果跳转，x5 = 1
bne_next:
    
    # 测试 blt (小于分支)
    blt x1, x2, blt_true
    li x6, 0         # 如果不跳转，x6 = 0
    j blt_next
blt_true:
    li x6, 1         # 如果跳转，x6 = 1
blt_next:
    
    # 测试 bge (大于等于分支)
    bge x1, x3, bge_true
    li x7, 0         # 如果不跳转，x7 = 0
    j bge_next
bge_true:
    li x7, 1         # 如果跳转，x7 = 1
bge_next:
    
    # 存储结果
    sw x4, 0(x8)     # beq 结果
    sw x5, 4(x8)     # bne 结果
    sw x6, 8(x8)     # blt 结果
    sw x7, 12(x8)    # bge 结果
    
    # 无限循环表示程序结束
loop:
    j loop
