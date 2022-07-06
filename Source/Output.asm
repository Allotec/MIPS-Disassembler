main:
addiu $29, $29, -16
sw $30, 12($29)
addu $30, $0, $29
sw $0, 0($30)
sw $0, 4($30)
sw $0, 4($30)
addi $7, $0, 1000
.L3:
lw $2, 4($30)
slt $2, $2, $7
beq $2, $0, .L0
nop
lw $4, 4($30)
lui $1, 0x5555
ori $2, $1, 0
ori $2, $2, 21846
mult $4, $2
mfhi $3
sra $2, $4, 31
subu $3, $3, $2
addu $2, $0, $3
sll $2, $2, 1
addu $2, $2, $3
subu $3, $4, $2
beq $3, $0, .L1
nop
lw $4, 4($30)
lui $1, 0x6666
ori $2, $1, 0
ori $2, $2, 26215
mult $4, $2
mfhi $2
sra $3, $2, 1
sra $2, $4, 31
subu $3, $3, $2
addu $2, $0, $3
sll $2, $2, 2
addu $2, $2, $3
subu $3, $4, $2
bne $3, $0, .L2
nop
.L1:
lw $3, 0($30)
lw $2, 4($30)
addu $2, $3, $2
sw $2, 0($30)
.L2:
lw $2, 4($30)
addiu $2, $2, 1
sw $2, 4($30)
j .L3
nop
.L0:
addu $2, $0, $0
addu $29, $0, $30
lw $30, 12($29)
addiu $29, $29, 16
jr $31
nop
