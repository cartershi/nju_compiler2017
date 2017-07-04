.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text

fact:
subu $sp, $sp, 48
sw $ra, 44($sp)
sw $fp, 40($sp)
addi $fp, $sp, 48
sw $a0, -12($fp)
lw $s1, -12($fp)
move $s0, $s1
sw $s0, -16($fp)
li $s1, 1
move $s0, $s1
sw $s0, -20($fp)
lw $s1, -16($fp)
lw $s2, -20($fp)
beq $s1, $s2, label1
j label2
label1:
lw $s1, -12($fp)
move $s0, $s1
sw $s0, -24($fp)
lw $s1, -24($fp)
move $v0, $s1
lw $ra, -4($fp)
lw $fp, -8($fp)
addi $sp, $sp, 48
jr $ra
j label3
label2:
lw $s1, -12($fp)
move $s0, $s1
sw $s0, -32($fp)
lw $s1, -12($fp)
move $s0, $s1
sw $s0, -44($fp)
li $s1, 1
move $s0, $s1
sw $s0, -48($fp)
lw $s1, -44($fp)
lw $s2, -48($fp)
sub $s0, $s1, $s2
sw $s0, -40($fp)
lw $s1, -40($fp)
move $a0, $s1
jal fact
move $s0, $v0
sw $s0, -36($fp)
lw $s1, -32($fp)
lw $s2, -36($fp)
mul $s0, $s1, $s2
sw $s0, -28($fp)
lw $s1, -28($fp)
move $v0, $s1
lw $ra, -4($fp)
lw $fp, -8($fp)
addi $sp, $sp, 48
jr $ra
label3:

main:
subu $sp, $sp, 60
sw $ra, 56($sp)
sw $fp, 52($sp)
addi $fp, $sp, 60
li $v0, 4
la $a0, _prompt
syscall
li $v0, 5
syscall
move $s0, $v0
sw $s0, -12($fp)
lw $s1, -12($fp)
move $s0, $s1
sw $s0, -20($fp)
lw $s1, -20($fp)
move $s0, $s1
sw $s0, -24($fp)
li $s1, 1
move $s0, $s1
sw $s0, -28($fp)
lw $s1, -24($fp)
lw $s2, -28($fp)
bgt $s1, $s2, label4
j label5
label4:
lw $s1, -20($fp)
move $s0, $s1
sw $s0, -40($fp)
lw $s1, -40($fp)
move $a0, $s1
jal fact
move $s0, $v0
sw $s0, -32($fp)
lw $s1, -32($fp)
move $s0, $s1
sw $s0, -44($fp)
j label6
label5:
li $s1, 1
move $s0, $s1
sw $s0, -48($fp)
lw $s1, -48($fp)
move $s0, $s1
sw $s0, -44($fp)
label6:
lw $s1, -44($fp)
move $s0, $s1
sw $s0, -56($fp)
lw $s1, -56($fp)
li $v0, 1
move $a0, $s1
syscall
li $v0, 4
la $a0, _ret
syscall
li $s1, 0
move $s0, $s1
sw $s0, -60($fp)
lw $s1, -60($fp)
move $v0, $s1
lw $ra, -4($fp)
lw $fp, -8($fp)
addi $sp, $sp, 60
jr $ra
