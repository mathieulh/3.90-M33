
	.set noreorder
	.set noat

	.global ExtendStack
	.ent	ExtendStack

ExtendStack:

	addiu	$sp, $sp, -0x10
	sw		$ra, 0($sp)
	sw		$s1, 4($sp)
	move	$s1, $sp
	li		$sp, 0xbfc10000
	jalr	$a1
	nop
	move	$sp, $s1
	lw		$ra, 0($sp)
	lw		$s1, 4($sp);
	jr		$ra
	addiu	$sp, $sp, 0x10


	.end ExtendStack

