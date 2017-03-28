	.file	1 "nokxploit.c"
	.section .mdebug.eabi32
	.section .gcc_compiled_long32
	.previous
	.text
	.align	2
	.globl	copyBootMem
	.ent	copyBootMem
copyBootMem:
	.frame	$fp,24,$31		# vars= 16, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$fp,16($sp)
	move	$fp,$sp
	li	$2,-2000683008			# 0xffffffff88c00000
	sw	$2,4($fp)
	li	$2,-2009137152			# 0xffffffff883f0000
	sw	$2,0($fp)
	sw	$0,8($fp)
	j	$L2
	nop

$L3:
	lw	$2,4($fp)
	lw	$3,0($2)
	lw	$2,0($fp)
	sw	$3,0($2)
	lw	$2,0($fp)
	addiu	$2,$2,4
	sw	$2,0($fp)
	lw	$2,4($fp)
	addiu	$2,$2,4
	sw	$2,4($fp)
	lw	$2,8($fp)
	addiu	$2,$2,1
	sw	$2,8($fp)
$L2:
	lw	$2,8($fp)
	slt	$2,$2,8192
	bne	$2,$0,$L3
	nop

	move	$sp,$fp
	lw	$fp,16($sp)
	addiu	$sp,$sp,24
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	copyBootMem
	.size	copyBootMem, .-copyBootMem
	.rdata
	.align	2
$LC0:
	.ascii	"ELF\000"
	.text
	.align	2
	.globl	hooked_LoadExecForKernel_28D0D249
	.ent	hooked_LoadExecForKernel_28D0D249
hooked_LoadExecForKernel_28D0D249:
	.frame	$fp,128,$31		# vars= 120, regs= 2/0, args= 0, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-128
	sw	$31,124($sp)
	sw	$fp,120($sp)
	move	$fp,$sp
	sw	$4,112($fp)
	sw	$5,116($fp)
	addiu	$2,$fp,64
 #APP
	sw $ra, 0($2)

 #NO_APP
	addiu	$2,$fp,16
 #APP
	sw $a0, 0($2)

 #NO_APP
	addiu	$2,$fp,20
 #APP
	sw $a1, 0($2)

 #NO_APP
	addiu	$2,$fp,24
 #APP
	sw $a2, 0($2)

 #NO_APP
	addiu	$2,$fp,28
 #APP
	sw $a3, 0($2)

 #NO_APP
	addiu	$2,$fp,32
 #APP
	sw $s0, 0($2)

 #NO_APP
	addiu	$2,$fp,36
 #APP
	sw $s1, 0($2)

 #NO_APP
	addiu	$2,$fp,40
 #APP
	sw $s2, 0($2)

 #NO_APP
	addiu	$2,$fp,44
 #APP
	sw $s3, 0($2)

 #NO_APP
	addiu	$2,$fp,48
 #APP
	sw $s4, 0($2)

 #NO_APP
	addiu	$2,$fp,52
 #APP
	sw $s5, 0($2)

 #NO_APP
	addiu	$2,$fp,56
 #APP
	sw $s6, 0($2)

 #NO_APP
	addiu	$2,$fp,60
 #APP
	sw $s7, 0($2)

 #NO_APP
	lw	$2,112($fp)
	move	$4,$2
	li	$5,1			# 0x1
	li	$6,511			# 0x1ff
	jal	sceIoOpen
	sw	$2,12($fp)
	lw	$2,12($fp)
	bltz	$2,$L7
	addiu	$2,$fp,68
	lw	$4,12($fp)
	move	$5,$2
	li	$6,40			# 0x28
	jal	sceIoRead
	lw	$3,68($fp)
	li	$2,1346502656			# 0x50420000
	ori	$2,$2,0x5000
	bne	$3,$2,$L9
	lw	$2,112($fp)
	move	$4,$2
	jal	strlen
	move	$3,$2
	lw	$2,112($fp)
	addu	$2,$3,$2
	addiu	$3,$2,-3
	lui	$2,%hi($LC0)
	addiu	$4,$2,%lo($LC0)
	lwl	$4,3($4)
	move	$5,$4
	lwr	$5,%lo($LC0)($2)
	move	$2,$5
	swl	$2,3($3)
	swr	$2,0($3)
	lw	$2,112($fp)
	move	$4,$2
	li	$5,514			# 0x202
	li	$6,511			# 0x1ff
	jal	sceIoOpen
	sw	$2,8($fp)
	lw	$2,100($fp)
	lw	$4,12($fp)
	move	$5,$2
	move	$6,$0
	jal	sceIoLseek32
	lw	$3,104($fp)
	lw	$2,100($fp)
	subu	$2,$3,$2
	sw	$2,4($fp)
	sw	$0,0($fp)
$L11:
	lw	$4,12($fp)
	lui	$2,%hi(buffer)
	addiu	$5,$2,%lo(buffer)
	li	$6,16384			# 0x4000
	jal	sceIoRead
	sw	$2,0($fp)
	lw	$2,0($fp)
	blez	$2,$L12
	lw	$3,0($fp)
	lw	$4,8($fp)
	lui	$2,%hi(buffer)
	addiu	$5,$2,%lo(buffer)
	move	$6,$3
	jal	sceIoWrite
$L12:
	lw	$2,0($fp)
	slt	$2,$2,16384
	bne	$2,$0,$L14
	j	$L11
$L14:
	lw	$4,8($fp)
	jal	sceIoClose
$L9:
	lw	$4,12($fp)
	jal	sceIoClose
$L7:
	addiu	$2,$fp,60
 #APP
	lw $s7, 0($2)

 #NO_APP
	addiu	$2,$fp,56
 #APP
	lw $s6, 0($2)

 #NO_APP
	addiu	$2,$fp,52
 #APP
	lw $s5, 0($2)

 #NO_APP
	addiu	$2,$fp,48
 #APP
	lw $s4, 0($2)

 #NO_APP
	addiu	$2,$fp,44
 #APP
	lw $s3, 0($2)

 #NO_APP
	addiu	$2,$fp,40
 #APP
	lw $s2, 0($2)

 #NO_APP
	addiu	$2,$fp,36
 #APP
	lw $s1, 0($2)

 #NO_APP
	addiu	$2,$fp,32
 #APP
	lw $s0, 0($2)

 #NO_APP
	addiu	$2,$fp,28
 #APP
	lw $a3, 0($2)

 #NO_APP
	addiu	$2,$fp,24
 #APP
	lw $a2, 0($2)

 #NO_APP
	addiu	$2,$fp,20
 #APP
	lw $a1, 0($2)

 #NO_APP
	addiu	$2,$fp,112
 #APP
	lw $a0, 0($2)

 #NO_APP
	addiu	$2,$fp,64
 #APP
	lw $ra, 0($2)

	addiu $sp, $sp, 0xfff0

	addu  $a2, $a1, $zero

	li $v0, 0x880BC27C

	jr $v0

 #NO_APP
	move	$sp,$fp
	lw	$31,124($sp)
	lw	$fp,120($sp)
	addiu	$sp,$sp,128
	j	$31
	.end	hooked_LoadExecForKernel_28D0D249
	.size	hooked_LoadExecForKernel_28D0D249, .-hooked_LoadExecForKernel_28D0D249

	.comm	buffer,16384,4
	.ident	"GCC: (GNU) 4.0.2 (PSPDEV 20051022)"
