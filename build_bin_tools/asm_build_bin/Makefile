# 汇编测试文件构建Makefile

RISCV_PREFIX = riscv64-linux-gnu-
AS = $(RISCV_PREFIX)as
LD = $(RISCV_PREFIX)ld
OBJCOPY = $(RISCV_PREFIX)objcopy
OBJDUMP = $(RISCV_PREFIX)objdump

ASM_FILES = $(wildcard *.s)
BIN_FILES = $(patsubst %.s,../bin/%.bin,$(ASM_FILES))

# 架构和ABI标志
ARCH_FLAGS = -march=rv32i -mabi=ilp32

.PHONY: all clean disasm

# 默认目标
all: prepare $(BIN_FILES)

# 准备输出目录
prepare:
	@mkdir -p ../bin

# 规则：从.s文件创建.bin文件
../bin/%.bin: %.s
	@echo "编译 $<..."
	@$(AS) $(ARCH_FLAGS) $< -o $*.o
	@$(LD) -m elf32lriscv $*.o -o $*.elf
	@$(OBJCOPY) -O binary $*.elf $@
	@echo "生成了 $@"
	@rm -f $*.o $*.elf

# 反汇编所有ELF文件
disasm: $(patsubst %.s,%.disasm,$(ASM_FILES))

%.disasm: %.s
	@echo "反汇编 $<..."
	@$(AS) $(ARCH_FLAGS) $< -o $*.o
	@$(LD) -m elf32lriscv $*.o -o $*.elf
	@$(OBJDUMP) -d $*.elf > $*.disasm
	@echo "生成了 $*.disasm"
	@rm -f $*.o $*.elf

# 清理规则
clean:
	rm -f *.o *.elf *.disasm
	rm -f ../bin/*.bin
