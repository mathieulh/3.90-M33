	.section .mdebug.eabi32
	.section .gcc_compiled_long32
	.previous
	.section	.debug_abbrev,"",@progbits
$Ldebug_abbrev0:
	.section	.debug_info,"",@progbits
$Ldebug_info0:
	.section	.debug_line,"",@progbits
$Ldebug_line0:
	.text
$Ltext0:
	.align	2
	.globl	strlen
$LFB2:
	.file 1 "minilibc.c"
	.loc 1 7 0
	.ent	strlen
strlen:
	.frame	$fp,24,$31		# vars= 16, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
$LCFI0:
	sw	$fp,16($sp)
$LCFI1:
	move	$fp,$sp
$LCFI2:
	sw	$4,8($fp)
	.loc 1 8 0
	sw	$0,0($fp)
	.loc 1 10 0
	j	$L2
	nop

$L3:
	.loc 1 11 0
	lw	$2,0($fp)
	addiu	$2,$2,1
	sw	$2,0($fp)
$L2:
	.loc 1 10 0
	lw	$2,8($fp)
	lb	$2,0($2)
	sltu	$2,$0,$2
	andi	$3,$2,0x00ff
	lw	$2,8($fp)
	addiu	$2,$2,1
	sw	$2,8($fp)
	bne	$3,$0,$L3
	nop

	.loc 1 13 0
	lw	$2,0($fp)
	.loc 1 14 0
	move	$sp,$fp
	lw	$fp,16($sp)
	addiu	$sp,$sp,24
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	strlen
$LFE2:
	.size	strlen, .-strlen
	.section	.debug_frame,"",@progbits
$Lframe0:
	.4byte	$LECIE0-$LSCIE0
$LSCIE0:
	.4byte	0xffffffff
	.byte	0x1
	.ascii	"\000"
	.uleb128 0x1
	.sleb128 -4
	.byte	0x1f
	.byte	0xc
	.uleb128 0x1d
	.uleb128 0x0
	.align	2
$LECIE0:
$LSFDE0:
	.4byte	$LEFDE0-$LASFDE0
$LASFDE0:
	.4byte	$Lframe0
	.4byte	$LFB2
	.4byte	$LFE2-$LFB2
	.byte	0x4
	.4byte	$LCFI0-$LFB2
	.byte	0xe
	.uleb128 0x18
	.byte	0x4
	.4byte	$LCFI1-$LCFI0
	.byte	0x9e
	.uleb128 0x2
	.byte	0x4
	.4byte	$LCFI2-$LCFI1
	.byte	0xc
	.uleb128 0x1e
	.uleb128 0x18
	.align	2
$LEFDE0:
	.align	0
	.text
$Letext0:
	.section	.debug_info
	.4byte	0xb7
	.2byte	0x2
	.4byte	$Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	$Ldebug_line0
	.4byte	$Letext0
	.4byte	$Ltext0
	.ascii	"GNU C 4.0.2 (PSPDEV 20051022) -g\000"
	.byte	0x1
	.ascii	"minilibc.c\000"
	.ascii	"/usr/local/code/noxploit/src/patch\000"
	.uleb128 0x2
	.4byte	0xa5
	.byte	0x1
	.ascii	"strlen\000"
	.byte	0x1
	.byte	0x7
	.byte	0x1
	.4byte	0xa5
	.4byte	$LFB2
	.4byte	$LFE2
	.4byte	$LSFDE0
	.byte	0x1
	.byte	0x6e
	.uleb128 0x3
	.ascii	"str\000"
	.byte	0x1
	.byte	0x6
	.4byte	0xac
	.byte	0x2
	.byte	0x8e
	.sleb128 8
	.uleb128 0x4
	.ascii	"n\000"
	.byte	0x1
	.byte	0x8
	.4byte	0xa5
	.byte	0x2
	.byte	0x8e
	.sleb128 0
	.byte	0x0
	.uleb128 0x5
	.ascii	"int\000"
	.byte	0x4
	.byte	0x5
	.uleb128 0x6
	.byte	0x4
	.4byte	0xb2
	.uleb128 0x5
	.ascii	"char\000"
	.byte	0x1
	.byte	0x6
	.byte	0x0
	.section	.debug_abbrev
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x10
	.uleb128 0x6
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x25
	.uleb128 0x8
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x1b
	.uleb128 0x8
	.byte	0x0
	.byte	0x0
	.uleb128 0x2
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x1
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x2001
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x5
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x4
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x5
	.uleb128 0x24
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x6
	.uleb128 0xf
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.section	.debug_pubnames,"",@progbits
	.4byte	0x19
	.2byte	0x2
	.4byte	$Ldebug_info0
	.4byte	0xbb
	.4byte	0x68
	.ascii	"strlen\000"
	.4byte	0x0
	.section	.debug_aranges,"",@progbits
	.4byte	0x1c
	.2byte	0x2
	.4byte	$Ldebug_info0
	.byte	0x4
	.byte	0x0
	.2byte	0x0
	.2byte	0x0
	.4byte	$Ltext0
	.4byte	$Letext0-$Ltext0
	.4byte	0x0
	.4byte	0x0
	.ident	"GCC: (GNU) 4.0.2 (PSPDEV 20051022)"
