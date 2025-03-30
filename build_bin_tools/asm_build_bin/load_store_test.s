# 测试加载和存储指令
.text
.globl _start

_start:
    # 初始化数据
    li x1, 0x100     # 基地址
    li x2, 0xABCD1234 # 测试数据1
    li x3, 0x87654321 # 测试数据2
    
    # 存储字
    sw x2, 0(x1)
    sw x3, 4(x1)
    
    # 存储半字
    li x4, 0xDEAD
    sh x4, 8(x1)
    
    # 存储字节
    li x5, 0xBE
    sb x5, 10(x1)
    
    # 从内存加载数据
    lw x10, 0(x1)    # 应该得到 0xABCD1234
    lw x11, 4(x1)    # 应该得到 0x87654321
    lh x12, 8(x1)    # 加载半字 0xDEAD
    lb x13, 10(x1)   # 加载字节 0xBE
    
    # 测试无符号加载
    lhu x14, 8(x1)   # 无符号半字加载
    lbu x15, 10(x1)  # 无符号字节加载
    
    # 存储结果到新位置
    li x20, 0x200    # 结果存储地址
    sw x10, 0(x20)
    sw x11, 4(x20)
    sw x12, 8(x20)
    sw x13, 12(x20)
    sw x14, 16(x20)
    sw x15, 20(x20)
    
    # 无限循环表示程序结束
loop:
    j loop
