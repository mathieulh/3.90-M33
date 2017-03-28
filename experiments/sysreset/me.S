#include <regdef.h>

#define STATUS	$12
#define CAUSE 	$13
#define CONFIG 	$16
#define TAGLO 	$28
#define TAGHI 	$29
#define IXIST	0x01
#define DXIST	0x11

	.global me_run
	.global me_end
	.global me_main
	.set noreorder
	.set noat

me_run:
	li		k0, 0xbc100000
	li		t0, 7
	sw		t0, 80(k0)
	mtc0		zero, TAGLO
	mtc0		zero, TAGHI
	li		k1, 8192
a:
	addi		k1, k1, -64
	bne		k1, zero, a
	cache		IXIST, 0(k1)
	li		k1, 8192
b:
	addi		k1, k1, -64
	bne		k1, zero, b
	cache		DXIST, 0(k1)
	mtc0		zero, CAUSE
	li		k0, 0x20000000
	mtc0		k0, STATUS
	sync
	la		t0, me_main
	lui		t1, 0x8000
	or		k0, t0, t1
	li		sp, 0x80200000
	jr		k0	
	nop
me_end:
