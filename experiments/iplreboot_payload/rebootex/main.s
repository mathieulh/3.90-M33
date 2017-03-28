	.file	1 "main.c"
	.section .mdebug.eabi32
	.section .gcc_compiled_long32
	.previous
	.globl	Ipl_Payload
	.data
	.align	2
	.type	Ipl_Payload, @object
	.size	Ipl_Payload, 4
Ipl_Payload:
	.word	-2009071616
	.text
	.align	2
	.globl	Reboot_Entry
	.ent	Reboot_Entry
Reboot_Entry:
	.frame	$fp,8,$31		# vars= 0, regs= 2/0, args= 0, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-8
	sw	$31,4($sp)
	sw	$fp,0($sp)
	move	$fp,$sp
	lui	$2,%hi(Ipl_Payload)
	lw	$2,%lo(Ipl_Payload)($2)
	li	$4,-2013265920			# 0xffffffff88000000
	li	$5,33554432			# 0x2000000
	move	$6,$0
	move	$7,$0
	jal	$2
	nop

	move	$sp,$fp
	lw	$31,4($sp)
	lw	$fp,0($sp)
	addiu	$sp,$sp,8
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	Reboot_Entry
	.size	Reboot_Entry, .-Reboot_Entry
	.ident	"GCC: (GNU) 4.0.2 (PSPDEV 20051022)"
