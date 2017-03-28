# PSP Software Development Kit - http://www.pspdev.org
# -----------------------------------------------------------------------
# Licensed under the BSD license, see LICENSE in PSPSDK root for details.
#
# crt0.S - Startup functions and definitions. Inspired by nem's helloworld.
#
# Copyright (c) 2005 Marcus R. Brown <mrbrown@ocgnet.org>
# Copyright (c) 2005 James Forshaw <tyranid@gmail.com>
# Copyright (c) 2005 John Kelley <ps2dev@kelley.ca>
#
# $Id: crt0.S 363 2005-06-27 20:35:14Z tyranid $

		.set noreorder

		.text

		.extern main

#include "as_reg_compat.h"


##############################################################################

		# Support for _init() and _fini().
;		.global _init
;		.global _fini
;		.type	_init, @function
;		.type	_fini, @function

		# The .weak keyword ensures there's no error if
		# _init/_fini aren't defined.
;		.weak	_init
;		.weak	_fini

		# Define the main thread attribute variable as weak
;		.weak   _main_thread_attr

		.global _start
		.ent _start

_start:
#;from loader
#;save r4,r5,r2,r20
		jal		entry
		nop

		addiu 	$sp, -0x20
		sw		$2, 0($sp)	
		sw		$4, 4($sp)
		sw		$5, 8($sp)
		sw		$6,12($sp)
		sw		$7,16($sp)
		sw		$8,20($sp)
		sw		$22,24($sp)
		sw		$gp,28($sp)

# Clear BSS
#	loop:
#		sw		$0,0($t0)
#		sltu	$v0, $t0, $t1
#		bne		$v0, $0, loop
#		addiu	$t0, $t0, 4
		la		$gp, _gp
#;		la		$t0, _init
#;		beqz	$t0, 1f
#;		nop
#;		jalr	$t0
#;		nop
1:
		jal		main
		nop
#
		lw		$2, 0($sp)	
		lw		$4, 4($sp)
		lw		$5, 8($sp)
		lw		$6,12($sp)
		lw		$7,16($sp)
		lw		$8,20($sp)
		lw		$22,24($sp)
		lw		$gp,28($sp)
		jr	$2
		addu	$29,$22,$0
;
		.end _start

		.global _exit
		.ent _exit

_exit:
# Exit
		la		$t0, _fini
		beqz	$t0, 2f
		nop

		jalr	$t0
		nop
2:

		lw		$ra, 0($sp)
		jr		$ra
		addiu	$sp, 0x10
			
		.end _exit

		.bss
