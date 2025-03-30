# 计算斐波那契数列的前10个数
.text
.globl _start

_start:
    # 初始化
    li x1, 0         # 第一个数 F(0) = 0
    li x2, 1         # 第二个数 F(1) = 1
    li x3, 10        # 要计算的斐波那契数列项数
    li x4, 0x100     # 存储结果的起始地址
    li x5, 0         # 循环计数器
    
    # 存储前两个数
    sw x1, 0(x4)     # 存储 F(0)
    sw x2, 4(x4)     # 存储 F(1)
    addi x4, x4, 8   # 更新存储地址
    addi x5, x5, 2   # 更新计数器
    
fibonacci_loop:
    # 检查是否已经计算了足够的项
    beq x5, x3, done
    
    # 计算下一个斐波那契数
    add x6, x1, x2   # x6 = x1 + x2
    
    # 存储结果
    sw x6, 0(x4)
    addi x4, x4, 4
    
    # 更新值，准备下一次迭代
    mv x1, x2
    mv x2, x6
    addi x5, x5, 1
    
    j fibonacci_loop
    
done:
    # 无限循环表示程序结束
loop:
    j loop
