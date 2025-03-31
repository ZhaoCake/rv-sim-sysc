# Simple RISC-V test program

# Initialize registers
addi x1, x0, 10       # x1 = 10
addi x2, x0, 20       # x2 = 20

# Test arithmetic
add x3, x1, x2        # x3 = x1 + x2 = 30
sub x4, x2, x1        # x4 = x2 - x1 = 10
and x5, x3, x4        # x5 = x3 & x4 = 10
or  x6, x3, x4        # x6 = x3 | x4 = 30
xor x7, x3, x4        # x7 = x3 ^ x4 = 20

# Store and load data
sw x3, 0(x0)          # Store x3 (30) at address 0
lw x8, 0(x0)          # Load from address 0 into x8 (should be 30)

# Test branch
beq x1, x1, label     # Should branch to label
addi x9, x0, 99       # Should be skipped

label:
addi x9, x0, 5        # x9 = 5

# Test jump
jal x10, jump_target  # Jump to jump_target, x10 = PC + 4
addi x11, x0, 77      # Should be skipped

jump_target:
addi x12, x0, 42      # x12 = 42

# All done
ebreak                # End program
