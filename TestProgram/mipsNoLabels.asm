main:
addiu   $sp,$sp,-16
sw      $fp,12($sp)
move    $fp,$sp
sw      $0,0($fp)
sw      $0,4($fp)
sw      $0,4($fp)
addi $7, $0, 1000
.L5:
lw      $2,4($fp)
slt     $2,$2,$7
beq     $2,$0,.L2
nop

lw      $4,4($fp)
li   $2,1431633920
ori  $2,$2,0x5556
mult    $4,$2
mfhi    $3
sra     $2,$4,31
subu    $3,$3,$2
move    $2,$3
sll     $2,$2,1
addu    $2,$2,$3
subu    $3,$4,$2
beq     $3,$0,.L3
nop

lw      $4,4($fp)
li   $2,1717960704
ori  $2,$2,0x6667
mult    $4,$2
mfhi    $2
sra     $3,$2,1
sra     $2,$4,31
subu    $3,$3,$2
move    $2,$3
sll     $2,$2,2
addu    $2,$2,$3
subu    $3,$4,$2
bne     $3,$0,.L4
nop

.L3:
lw      $3,0($fp)
lw      $2,4($fp)
addu    $2,$3,$2
sw      $2,0($fp)
.L4:
lw      $2,4($fp)
addiu   $2,$2,1
sw      $2,4($fp)
j       .L5
nop

.L2:
move    $2,$0
move    $sp,$fp
lw      $fp,12($sp)
addiu   $sp,$sp,16
jr      $31
nop
