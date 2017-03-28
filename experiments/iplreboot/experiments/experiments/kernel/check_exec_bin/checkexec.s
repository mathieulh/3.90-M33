	.file	1 "CheckExec.c"
	.section .mdebug.eabi32
	.section .gcc_compiled_long32
	.previous
	.text
	.align	2
	.globl	_start
	.ent	_start
_start:
	.frame	$sp,16,$31		# vars= 16, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-16
	li	$2,-2012938240			# 0xffffffff88050000
	ori	$2,$2,0xf96c
	sw	$2,0($sp)
	lw	$3,0($sp)
	li	$2,65011712			# 0x3e00000
	ori	$2,$2,0x8
	sw	$2,0($3)
	lw	$2,0($sp)
	addiu	$3,$2,4
	li	$2,4129			# 0x1021
	sw	$2,0($3)
	li	$2,-2013200384			# 0xffffffff88010000
	ori	$2,$2,0x6ca0
	lw	$2,0($2)
	sw	$2,4($sp)
	lw	$3,4($sp)
	li	$2,67043328			# 0x3ff0000
	ori	$2,$2,0xffff
	and	$2,$3,$2
	sll	$3,$2,2
	li	$2,-2147483648			# 0xffffffff80000000
	or	$2,$3,$2
	sw	$2,8($sp)
	lw	$3,8($sp)
	lui	$2,%hi(original_checkexec_fn)
	sw	$3,%lo(original_checkexec_fn)($2)
	lui	$2,%hi(patched_checkexec)
	addiu	$2,$2,%lo(patched_checkexec)
	sw	$2,8($sp)
	lw	$3,8($sp)
	li	$2,268369920			# 0xfff0000
	ori	$2,$2,0xffff
	and	$2,$3,$2
	srl	$3,$2,2
	li	$2,201326592			# 0xc000000
	or	$2,$3,$2
	sw	$2,4($sp)
	li	$2,-2013200384			# 0xffffffff88010000
	ori	$3,$2,0x6ca0
	lw	$2,4($sp)
	sw	$2,0($3)
	addiu	$sp,$sp,16
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	_start
	.size	_start, .-_start
	.align	2
	.globl	patched_checkexec
	.ent	patched_checkexec
patched_checkexec:
	.frame	$sp,40,$31		# vars= 32, regs= 1/0, args= 0, gp= 0
	.mask	0x80000000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-40
	sw	$31,32($sp)
	sw	$4,16($sp)
	sw	$5,20($sp)
	lw	$2,20($sp)
	addiu	$2,$2,90
	sh	$0,0($2)
	lw	$2,20($sp)
	addiu	$2,$2,1420
	lw	$2,0($2)
	sw	$2,8($sp)
	lui	$2,%hi(original_checkexec_fn)
	lw	$2,%lo(original_checkexec_fn)($2)
	lw	$4,16($sp)
	lw	$5,20($sp)
	jal	$2
	nop

	sw	$2,4($sp)
	lw	$2,20($sp)
	addiu	$2,$2,90
	lhu	$2,0($2)
	andi	$2,$2,0x1
	andi	$2,$2,0x00ff
	beq	$2,$0,$L4
	nop

	lw	$2,4($sp)
	sw	$2,24($sp)
	j	$L6
	nop

$L4:
	lw	$2,20($sp)
	addiu	$2,$2,36
	lw	$2,0($2)
	sw	$2,0($sp)
	lw	$2,20($sp)
	addiu	$2,$2,32
	move	$3,$2
	li	$2,1
	sb	$2,0($3)
	lw	$2,20($sp)
	addiu	$2,$2,72
	move	$3,$2
	li	$2,1
	sb	$2,0($3)
	lw	$2,20($sp)
	addiu	$2,$2,84
	move	$3,$2
	li	$2,1
	sb	$2,0($3)
	lw	$2,20($sp)
	addiu	$2,$2,28
	move	$3,$2
	lw	$2,0($sp)
	sw	$2,0($3)
	lw	$2,20($sp)
	addiu	$2,$2,36
	sw	$0,0($2)
	lw	$2,20($sp)
	addiu	$2,$2,16
	move	$3,$2
	li	$2,6668			# 0x1a0c
	sw	$2,0($3)
	lw	$2,20($sp)
	addiu	$2,$2,40
	move	$3,$2
	li	$2,65264			# 0xfef0
	sw	$2,0($3)
	lw	$2,20($sp)
	addiu	$2,$2,60
	move	$3,$2
	li	$2,1184			# 0x4a0
	sw	$2,0($3)
	lw	$2,20($sp)
	addiu	$2,$2,76
	move	$3,$2
	li	$2,10496			# 0x2900
	sw	$2,0($3)
	lw	$2,20($sp)
	addiu	$2,$2,88
	move	$3,$2
	li	$2,65536			# 0x10000
	ori	$2,$2,0x124c
	sw	$2,0($3)
	lw	$2,0($sp)
	beq	$2,$0,$L7
	nop

	lw	$2,0($sp)
	move	$4,$2
	lw	$5,16($sp)
	lw	$6,8($sp)
	jal	memcpy
	nop

$L7:
	li	$2,1			# 0x1
	sw	$2,24($sp)
$L6:
	lw	$2,24($sp)
	lw	$31,32($sp)
	addiu	$sp,$sp,40
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	patched_checkexec
	.size	patched_checkexec, .-patched_checkexec

	.comm	original_checkexec_fn,4,4
	.ident	"GCC: (GNU) 4.0.1 (PSPDEV 20050827)"
