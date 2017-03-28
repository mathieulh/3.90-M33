	.file	1 "loader.c"
	.section .mdebug.eabi32
	.section .gcc_compiled_long32
	.previous
	.globl	numfailed
	.section	.bss,"aw",@nobits
	.align	2
	.type	numfailed, @object
	.size	numfailed, 4
numfailed:
	.space	4
	.globl	hash_values
	.data
	.align	2
	.type	hash_values, @object
	.size	hash_values, 20
hash_values:
	.byte	-37
	.byte	-84
	.byte	50
	.byte	-42
	.byte	60
	.byte	89
	.byte	-24
	.byte	-50
	.byte	-89
	.byte	115
	.byte	29
	.byte	-16
	.byte	108
	.byte	39
	.byte	124
	.byte	15
	.byte	-105
	.byte	-58
	.byte	12
	.byte	-49
	.globl	functions
	.align	2
	.type	functions, @object
	.size	functions, 496
functions:
	.word	278876348
	.word	sceIoOpen
	.word	IoFileMgrForUser
	.word	0
	.word	-2129900605
	.word	sceIoClose
	.word	IoFileMgrForUser
	.word	0
	.word	1784909187
	.word	sceIoRead
	.word	IoFileMgrForUser
	.word	0
	.word	1122763692
	.word	sceIoWrite
	.word	IoFileMgrForUser
	.word	0
	.word	595443023
	.word	sceKernelAllocPartitionMemory
	.word	SysMem
	.word	0
	.word	-1227481854
	.word	sceKernelFreePartitionMemory
	.word	SysMem
	.word	0
	.word	-1650828383
	.word	sceKernelGetBlockHeadAddr
	.word	SysMem
	.word	0
	.word	-1567493881
	.word	sceKernelMaxFreeMemSize
	.word	SysMem
	.word	0
	.word	943684556
	.word	sceKernelTerminateDeleteThread
	.word	Threadman
	.word	0
	.word	691750328
	.word	sceKernelGetThreadId
	.word	Threadman
	.word	0
	.word	1148030438
	.word	sceKernelCreateThread
	.word	Threadman
	.word	0
	.word	-193624995
	.word	sceKernelStartThread
	.word	Threadman
	.word	0
	.word	-827462841
	.word	sceKernelDelayThread
	.word	Threadman
	.word	0
	.word	-306554812
	.word	sceKernelDeleteCallback
	.word	Threadman
	.word	0
	.word	-317452064
	.word	sceKernelDeleteFpl
	.word	Threadman
	.word	0
	.word	683034780
	.word	sceKernelDeleteSema
	.word	Threadman
	.word	0
	.word	-702650015
	.word	sceKernelReleaseSubIntrHandler
	.word	InterruptManager
	.word	0
	.word	-905665863
	.word	0
	.word	InterruptManager
	.word	0
	.word	-74571028
	.word	0
	.word	InterruptManager
	.word	0
	.word	-1271537979
	.word	sceKernelDcacheWritebackInvalidateAll
	.word	UtilsForUser
	.word	0
	.word	2043790330
	.word	sceKernelDcacheWritebackAll
	.word	UtilsForUser
	.word	0
	.word	-1121198585
	.word	sceUmdUnRegisterUMDCallBack
	.word	UMDForUser
	.word	0
	.word	-542590216
	.word	scePowerUnregisterCallback
	.word	Power
	.word	0
	.word	1875142739
	.word	sceAudioChRelease
	.word	sceAudio
	.word	0
	.word	89598559
	.word	sceKernelExitGame
	.word	LoadExecForUser
	.word	0
	.word	1357955564
	.word	sceKernelStartModule
	.word	ModuleMgrForUser
	.word	0
	.word	1355074903
	.word	sceUtilitySavedataInitStart
	.word	sceUtility
	.word	0
	.word	-1752124612
	.word	sceUtilitySavedataShutdownStart
	.word	sceUtility
	.word	0
	.word	-726048773
	.word	sceUtilitySavedataUpdate
	.word	sceUtility
	.word	0
	.word	-2005607456
	.word	sceUtilitySavedataGetStatus
	.word	sceUtility
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.globl	missing_functions
	.align	2
	.type	missing_functions, @object
	.size	missing_functions, 396
missing_functions:
	.word	-1807654608
	.word	sceKernelGetThreadmanIdList
	.word	691750328
	.word	8
	.word	8
	.word	58
	.word	8
	.word	0
	.word	0
	.word	398551118
	.word	sceKernelReferThreadStatus
	.word	691750328
	.word	5
	.word	5
	.word	-8
	.word	8
	.word	0
	.word	0
	.word	-1170189866
	.word	sceKernelCancelCallback
	.word	691750328
	.word	-107
	.word	-107
	.word	72
	.word	-107
	.word	0
	.word	0
	.word	1930352828
	.word	sceKernelReferCallbackStatus
	.word	691750328
	.word	-104
	.word	-104
	.word	38
	.word	-104
	.word	0
	.word	0
	.word	-274838416
	.word	sceKernelDeleteEventFlag
	.word	691750328
	.word	-82
	.word	-82
	.word	103
	.word	-82
	.word	0
	.word	0
	.word	-1844506550
	.word	sceKernelIcacheInvalidateAll
	.word	-1271537979
	.word	6
	.word	6
	.word	-2
	.word	6
	.word	0
	.word	0
	.word	-115739096
	.word	sceKernelTotalFreeMemSize
	.word	595443023
	.word	-1
	.word	-8
	.word	8
	.word	-1
	.word	0
	.word	0
	.word	-1976003567
	.word	sceKernelDisableSubIntr
	.word	-702650015
	.word	2
	.word	2
	.word	-3
	.word	2
	.word	0
	.word	0
	.word	-1120989036
	.word	sceKernelLoadExec
	.word	89598559
	.word	-2
	.word	-2
	.word	3
	.word	0
	.word	0
	.word	0
	.word	-1753357434
	.word	sceKernelLoadModule
	.word	1357955564
	.word	-4
	.word	-4
	.word	4
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.space	20
	.globl	LoadExecForUser
	.align	2
	.type	LoadExecForUser, @object
	.size	LoadExecForUser, 16
LoadExecForUser:
	.ascii	"LoadExecForUser\000"
	.globl	sceUtility
	.align	2
	.type	sceUtility, @object
	.size	sceUtility, 11
sceUtility:
	.ascii	"sceUtility\000"
	.globl	sceAudio
	.align	2
	.type	sceAudio, @object
	.size	sceAudio, 9
sceAudio:
	.ascii	"sceAudio\000"
	.globl	Power
	.align	2
	.type	Power, @object
	.size	Power, 9
Power:
	.ascii	"scePower\000"
	.globl	UMDForUser
	.align	2
	.type	UMDForUser, @object
	.size	UMDForUser, 11
UMDForUser:
	.ascii	"sceUmdUser\000"
	.globl	SuspendForUser
	.align	2
	.type	SuspendForUser, @object
	.size	SuspendForUser, 18
SuspendForUser:
	.ascii	"sceSuspendForUser\000"
	.globl	ModuleMgrForUser
	.align	2
	.type	ModuleMgrForUser, @object
	.size	ModuleMgrForUser, 17
ModuleMgrForUser:
	.ascii	"ModuleMgrForUser\000"
	.globl	InterruptManager
	.align	2
	.type	InterruptManager, @object
	.size	InterruptManager, 17
InterruptManager:
	.ascii	"InterruptManager\000"
	.globl	UtilsForUser
	.align	2
	.type	UtilsForUser, @object
	.size	UtilsForUser, 13
UtilsForUser:
	.ascii	"UtilsForUser\000"
	.globl	ge
	.align	2
	.type	ge, @object
	.size	ge, 11
ge:
	.ascii	"sceGe_user\000"
	.globl	SysMem
	.align	2
	.type	SysMem, @object
	.size	SysMem, 18
SysMem:
	.ascii	"SysMemUserForUser\000"
	.globl	sceCtrl
	.align	2
	.type	sceCtrl, @object
	.size	sceCtrl, 8
sceCtrl:
	.ascii	"sceCtrl\000"
	.globl	sceDisplay
	.align	2
	.type	sceDisplay, @object
	.size	sceDisplay, 11
sceDisplay:
	.ascii	"sceDisplay\000"
	.globl	Threadman
	.align	2
	.type	Threadman, @object
	.size	Threadman, 17
Threadman:
	.ascii	"ThreadManForUser\000"
	.globl	IoFileMgrForUser
	.align	2
	.type	IoFileMgrForUser, @object
	.size	IoFileMgrForUser, 17
IoFileMgrForUser:
	.ascii	"IoFileMgrForUser\000"
	.globl	g_loginitialised
	.section	.bss
	.align	2
	.type	g_loginitialised, @object
	.size	g_loginitialised, 4
g_loginitialised:
	.space	4
	.text
	.align	2
	.globl	sceIoOpen
	.ent	sceIoOpen
sceIoOpen:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x208f
 #NO_APP
	j	$31
	.end	sceIoOpen
	.size	sceIoOpen, .-sceIoOpen
	.align	2
	.globl	sceIoClose
	.ent	sceIoClose
sceIoClose:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x208d
 #NO_APP
	j	$31
	.end	sceIoClose
	.size	sceIoClose, .-sceIoClose
	.align	2
	.globl	sceIoRead
	.ent	sceIoRead
sceIoRead:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x2091
 #NO_APP
	j	$31
	.end	sceIoRead
	.size	sceIoRead, .-sceIoRead
	.align	2
	.globl	sceIoWrite
	.ent	sceIoWrite
sceIoWrite:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x2093
 #NO_APP
	j	$31
	.end	sceIoWrite
	.size	sceIoWrite, .-sceIoWrite
	.align	2
	.globl	sceKernelExitGame
	.ent	sceKernelExitGame
sceKernelExitGame:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x20F2
 #NO_APP
	j	$31
	.end	sceKernelExitGame
	.size	sceKernelExitGame, .-sceKernelExitGame
	.align	2
	.globl	sceKernelLoadExec
	.ent	sceKernelLoadExec
sceKernelLoadExec:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x20F0
 #NO_APP
	j	$31
	.end	sceKernelLoadExec
	.size	sceKernelLoadExec, .-sceKernelLoadExec
	.align	2
	.globl	sceKernelLoadModule
	.ent	sceKernelLoadModule
sceKernelLoadModule:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x20d2
 #NO_APP
	j	$31
	.end	sceKernelLoadModule
	.size	sceKernelLoadModule, .-sceKernelLoadModule
	.align	2
	.globl	sceKernelStartModule
	.ent	sceKernelStartModule
sceKernelStartModule:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x20d6
 #NO_APP
	j	$31
	.end	sceKernelStartModule
	.size	sceKernelStartModule, .-sceKernelStartModule
	.align	2
	.globl	_sceKernelLoadModule
	.ent	_sceKernelLoadModule
_sceKernelLoadModule:
	.frame	$sp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x00010000,-8
	.fmask	0x00000000,0
	addiu	$sp,$sp,-8
	sw	$16,0($sp)
 #APP
	move $16, $27;
move $27, $0;
la $2, 0x8805b5f8
jalr $2
nop
move $27, $16

 #NO_APP
	lw	$16,0($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,8
	.set	macro
	.set	reorder

	.end	_sceKernelLoadModule
	.size	_sceKernelLoadModule, .-_sceKernelLoadModule
	.align	2
	.globl	_sceKernelStartModule
	.ent	_sceKernelStartModule
_sceKernelStartModule:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x20d6
 #NO_APP
	j	$31
	.end	_sceKernelStartModule
	.size	_sceKernelStartModule, .-_sceKernelStartModule
	.align	2
	.globl	sceKernelTerminateDeleteThread
	.ent	sceKernelTerminateDeleteThread
sceKernelTerminateDeleteThread:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x2074;
 #NO_APP
	j	$31
	.end	sceKernelTerminateDeleteThread
	.size	sceKernelTerminateDeleteThread, .-sceKernelTerminateDeleteThread
	.align	2
	.globl	sceKernelCancelCallback
	.ent	sceKernelCancelCallback
sceKernelCancelCallback:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x2010
 #NO_APP
	j	$31
	.end	sceKernelCancelCallback
	.size	sceKernelCancelCallback, .-sceKernelCancelCallback
	.align	2
	.globl	sceKernelDeleteCallback
	.ent	sceKernelDeleteCallback
sceKernelDeleteCallback:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x200E;
 #NO_APP
	j	$31
	.end	sceKernelDeleteCallback
	.size	sceKernelDeleteCallback, .-sceKernelDeleteCallback
	.align	2
	.globl	sceKernelReferCallbackStatus
	.ent	sceKernelReferCallbackStatus
sceKernelReferCallbackStatus:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x2013
 #NO_APP
	j	$31
	.end	sceKernelReferCallbackStatus
	.size	sceKernelReferCallbackStatus, .-sceKernelReferCallbackStatus
	.align	2
	.globl	sceKernelReferThreadStatus
	.ent	sceKernelReferThreadStatus
sceKernelReferThreadStatus:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x2080
 #NO_APP
	j	$31
	.end	sceKernelReferThreadStatus
	.size	sceKernelReferThreadStatus, .-sceKernelReferThreadStatus
	.align	2
	.globl	sceKernelDelayThread
	.ent	sceKernelDelayThread
sceKernelDelayThread:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x201c;
 #NO_APP
	j	$31
	.end	sceKernelDelayThread
	.size	sceKernelDelayThread, .-sceKernelDelayThread
	.align	2
	.globl	sceKernelGetThreadId
	.ent	sceKernelGetThreadId
sceKernelGetThreadId:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x207B
 #NO_APP
	j	$31
	.end	sceKernelGetThreadId
	.size	sceKernelGetThreadId, .-sceKernelGetThreadId
	.align	2
	.globl	sceKernelGetThreadmanIdList
	.ent	sceKernelGetThreadmanIdList
sceKernelGetThreadmanIdList:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x2083
 #NO_APP
	j	$31
	.end	sceKernelGetThreadmanIdList
	.size	sceKernelGetThreadmanIdList, .-sceKernelGetThreadmanIdList
	.align	2
	.globl	sceKernelAllocPartitionMemory
	.ent	sceKernelAllocPartitionMemory
sceKernelAllocPartitionMemory:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x2169;
 #NO_APP
	j	$31
	.end	sceKernelAllocPartitionMemory
	.size	sceKernelAllocPartitionMemory, .-sceKernelAllocPartitionMemory
	.align	2
	.globl	sceKernelFreePartitionMemory
	.ent	sceKernelFreePartitionMemory
sceKernelFreePartitionMemory:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x20E2;
 #NO_APP
	j	$31
	.end	sceKernelFreePartitionMemory
	.size	sceKernelFreePartitionMemory, .-sceKernelFreePartitionMemory
	.align	2
	.globl	sceKernelGetBlockHeadAddr
	.ent	sceKernelGetBlockHeadAddr
sceKernelGetBlockHeadAddr:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x20e3;
 #NO_APP
	j	$31
	.end	sceKernelGetBlockHeadAddr
	.size	sceKernelGetBlockHeadAddr, .-sceKernelGetBlockHeadAddr
	.align	2
	.globl	sceKernelCreateThread
	.ent	sceKernelCreateThread
sceKernelCreateThread:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x206d
 #NO_APP
	j	$31
	.end	sceKernelCreateThread
	.size	sceKernelCreateThread, .-sceKernelCreateThread
	.align	2
	.globl	sceKernelStartThread
	.ent	sceKernelStartThread
sceKernelStartThread:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x206f
 #NO_APP
	j	$31
	.end	sceKernelStartThread
	.size	sceKernelStartThread, .-sceKernelStartThread
	.align	2
	.globl	sceKernelMaxFreeMemSize
	.ent	sceKernelMaxFreeMemSize
sceKernelMaxFreeMemSize:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x20DF;
 #NO_APP
	j	$31
	.end	sceKernelMaxFreeMemSize
	.size	sceKernelMaxFreeMemSize, .-sceKernelMaxFreeMemSize
	.align	2
	.globl	sceKernelTotalFreeMemSize
	.ent	sceKernelTotalFreeMemSize
sceKernelTotalFreeMemSize:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x20E0;
 #NO_APP
	j	$31
	.end	sceKernelTotalFreeMemSize
	.size	sceKernelTotalFreeMemSize, .-sceKernelTotalFreeMemSize
	.align	2
	.globl	sceKernelDcacheWritebackInvalidateAll
	.ent	sceKernelDcacheWritebackInvalidateAll
sceKernelDcacheWritebackInvalidateAll:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x20c7
 #NO_APP
	j	$31
	.end	sceKernelDcacheWritebackInvalidateAll
	.size	sceKernelDcacheWritebackInvalidateAll, .-sceKernelDcacheWritebackInvalidateAll
	.align	2
	.globl	sceKernelDcacheWritebackAll
	.ent	sceKernelDcacheWritebackAll
sceKernelDcacheWritebackAll:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x20c6
 #NO_APP
	j	$31
	.end	sceKernelDcacheWritebackAll
	.size	sceKernelDcacheWritebackAll, .-sceKernelDcacheWritebackAll
	.align	2
	.globl	sceKernelIcacheInvalidateAll
	.ent	sceKernelIcacheInvalidateAll
sceKernelIcacheInvalidateAll:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x20cd
 #NO_APP
	j	$31
	.end	sceKernelIcacheInvalidateAll
	.size	sceKernelIcacheInvalidateAll, .-sceKernelIcacheInvalidateAll
	.align	2
	.globl	sceKernelDeleteFpl
	.ent	sceKernelDeleteFpl
sceKernelDeleteFpl:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x204C
 #NO_APP
	j	$31
	.end	sceKernelDeleteFpl
	.size	sceKernelDeleteFpl, .-sceKernelDeleteFpl
	.align	2
	.globl	sceKernelDeleteSema
	.ent	sceKernelDeleteSema
sceKernelDeleteSema:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x2021
 #NO_APP
	j	$31
	.end	sceKernelDeleteSema
	.size	sceKernelDeleteSema, .-sceKernelDeleteSema
	.align	2
	.globl	sceKernelDeleteEventFlag
	.ent	sceKernelDeleteEventFlag
sceKernelDeleteEventFlag:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x2029
 #NO_APP
	j	$31
	.end	sceKernelDeleteEventFlag
	.size	sceKernelDeleteEventFlag, .-sceKernelDeleteEventFlag
	.align	2
	.globl	sceKernelReleaseSubIntrHandler
	.ent	sceKernelReleaseSubIntrHandler
sceKernelReleaseSubIntrHandler:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x2001
 #NO_APP
	j	$31
	.end	sceKernelReleaseSubIntrHandler
	.size	sceKernelReleaseSubIntrHandler, .-sceKernelReleaseSubIntrHandler
	.align	2
	.globl	sceKernelDisableSubIntr
	.ent	sceKernelDisableSubIntr
sceKernelDisableSubIntr:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x2003
 #NO_APP
	j	$31
	.end	sceKernelDisableSubIntr
	.size	sceKernelDisableSubIntr, .-sceKernelDisableSubIntr
	.align	2
	.globl	sceUmdUnRegisterUMDCallBack
	.ent	sceUmdUnRegisterUMDCallBack
sceUmdUnRegisterUMDCallBack:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x21BF
 #NO_APP
	j	$31
	.end	sceUmdUnRegisterUMDCallBack
	.size	sceUmdUnRegisterUMDCallBack, .-sceUmdUnRegisterUMDCallBack
	.align	2
	.globl	scePowerUnregisterCallback
	.ent	scePowerUnregisterCallback
scePowerUnregisterCallback:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x219B
 #NO_APP
	j	$31
	.end	scePowerUnregisterCallback
	.size	scePowerUnregisterCallback, .-scePowerUnregisterCallback
	.align	2
	.globl	sceAudioChRelease
	.ent	sceAudioChRelease
sceAudioChRelease:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x2135
 #NO_APP
	j	$31
	.end	sceAudioChRelease
	.size	sceAudioChRelease, .-sceAudioChRelease
	.align	2
	.globl	sceUtilitySavedataInitStart
	.ent	sceUtilitySavedataInitStart
sceUtilitySavedataInitStart:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x2200
 #NO_APP
	j	$31
	.end	sceUtilitySavedataInitStart
	.size	sceUtilitySavedataInitStart, .-sceUtilitySavedataInitStart
	.align	2
	.globl	sceUtilitySavedataGetStatus
	.ent	sceUtilitySavedataGetStatus
sceUtilitySavedataGetStatus:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x2203
 #NO_APP
	j	$31
	.end	sceUtilitySavedataGetStatus
	.size	sceUtilitySavedataGetStatus, .-sceUtilitySavedataGetStatus
	.align	2
	.globl	sceUtilitySavedataShutdownStart
	.ent	sceUtilitySavedataShutdownStart
sceUtilitySavedataShutdownStart:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x2201
 #NO_APP
	j	$31
	.end	sceUtilitySavedataShutdownStart
	.size	sceUtilitySavedataShutdownStart, .-sceUtilitySavedataShutdownStart
	.align	2
	.globl	sceUtilitySavedataUpdate
	.ent	sceUtilitySavedataUpdate
sceUtilitySavedataUpdate:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
 #APP
	syscall 0x2200
 #NO_APP
	j	$31
	.end	sceUtilitySavedataUpdate
	.size	sceUtilitySavedataUpdate, .-sceUtilitySavedataUpdate
	.align	2
	.globl	flashscreen
	.ent	flashscreen
flashscreen:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	li	$7,10			# 0xa
	li	$6,1142947840			# 0x44200000
	li	$2,1140850688			# 0x44000000
$L98:
	sw	$4,0($2)
$L96:
	addiu	$2,$2,4
	bnel	$2,$6,$L96
	sw	$4,0($2)

	li	$2,1140850688			# 0x44000000
	li	$3,1142947840			# 0x44200000
	sw	$5,0($2)
$L97:
	addiu	$2,$2,4
	bnel	$2,$3,$L97
	sw	$5,0($2)

	addiu	$7,$7,-1
	bne	$7,$0,$L98
	li	$2,1140850688			# 0x44000000

	j	$31
	nop

	.set	macro
	.set	reorder
	.end	flashscreen
	.size	flashscreen, .-flashscreen
	.align	2
	.globl	strlen
	.ent	strlen
strlen:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lb	$2,0($4)
	bne	$2,$0,$L100
	addiu	$4,$4,1

	move	$3,$0
	j	$31
	move	$2,$3

$L100:
	move	$3,$0
$L103:
	lb	$2,0($4)
	addiu	$3,$3,1
	bne	$2,$0,$L103
	addiu	$4,$4,1

	j	$31
	move	$2,$3

	.set	macro
	.set	reorder
	.end	strlen
	.size	strlen, .-strlen
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align	2
$LC0:
	.ascii	"ms0:/gtalog.txt\000"
	.align	2
$LC1:
	.ascii	"\012\000"
	.text
	.align	2
	.globl	dlogi
	.ent	dlogi
dlogi:
	.frame	$sp,24,$31		# vars= 0, regs= 5/0, args= 0, gp= 0
	.mask	0x800f0000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$19,12($sp)
	sw	$18,8($sp)
	sw	$17,4($sp)
	sw	$31,16($sp)
	sw	$16,0($sp)
	lui	$2,%hi(g_loginitialised)
	lw	$3,%lo(g_loginitialised)($2)
	move	$18,$4
	lui	$4,%hi($LC0)
	move	$17,$5
	move	$19,$6
	addiu	$4,$4,%lo($LC0)
	li	$5,770			# 0x302
	bne	$3,$0,$L113
	li	$6,511			# 0x1ff

	lw	$31,16($sp)
	lw	$19,12($sp)
	lw	$18,8($sp)
	lw	$17,4($sp)
	lw	$16,0($sp)
	j	$31
	addiu	$sp,$sp,24

$L113:
	jal	sceIoOpen
	nop

	move	$4,$2
	move	$5,$18
	move	$6,$17
	jal	sceIoWrite
	move	$16,$2

	lui	$5,%hi($LC1)
	addiu	$5,$5,%lo($LC1)
	move	$4,$16
	bne	$19,$0,$L114
	li	$6,1			# 0x1

	move	$4,$16
	lw	$31,16($sp)
	lw	$19,12($sp)
	lw	$18,8($sp)
	lw	$17,4($sp)
	lw	$16,0($sp)
	j	sceIoClose
	addiu	$sp,$sp,24

$L114:
	jal	sceIoWrite
	nop

	move	$4,$16
	lw	$31,16($sp)
	lw	$19,12($sp)
	lw	$18,8($sp)
	lw	$17,4($sp)
	lw	$16,0($sp)
	j	sceIoClose
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	dlogi
	.size	dlogi, .-dlogi
	.align	2
	.globl	dlog
	.ent	dlog
dlog:
	.frame	$sp,8,$31		# vars= 0, regs= 2/0, args= 0, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-8
	sw	$31,4($sp)
	sw	$16,0($sp)
	jal	strlen
	move	$16,$4

	move	$5,$2
	move	$4,$16
	lw	$31,4($sp)
	lw	$16,0($sp)
	li	$6,1			# 0x1
	j	dlogi
	addiu	$sp,$sp,8

	.set	macro
	.set	reorder
	.end	dlog
	.size	dlog, .-dlog
	.align	2
	.globl	dlogi2
	.ent	dlogi2
dlogi2:
	.frame	$sp,24,$31		# vars= 0, regs= 6/0, args= 0, gp= 0
	.mask	0x801f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$20,16($sp)
	sw	$19,12($sp)
	sw	$18,8($sp)
	sw	$17,4($sp)
	sw	$31,20($sp)
	sw	$16,0($sp)
	lui	$2,%hi(g_loginitialised)
	lw	$3,%lo(g_loginitialised)($2)
	move	$18,$4
	lui	$4,%hi($LC0)
	move	$17,$5
	move	$19,$6
	move	$20,$7
	addiu	$4,$4,%lo($LC0)
	li	$5,770			# 0x302
	bne	$3,$0,$L121
	li	$6,511			# 0x1ff

	lw	$31,20($sp)
	lw	$20,16($sp)
	lw	$19,12($sp)
	lw	$18,8($sp)
	lw	$17,4($sp)
	lw	$16,0($sp)
	j	$31
	addiu	$sp,$sp,24

$L121:
	jal	sceIoOpen
	nop

	move	$16,$2
	move	$5,$18
	move	$6,$17
	jal	sceIoWrite
	move	$4,$2

	move	$4,$16
	move	$5,$19
	jal	sceIoWrite
	move	$6,$20

	lui	$5,%hi($LC1)
	move	$4,$16
	addiu	$5,$5,%lo($LC1)
	jal	sceIoWrite
	li	$6,1			# 0x1

	move	$4,$16
	lw	$31,20($sp)
	lw	$20,16($sp)
	lw	$19,12($sp)
	lw	$18,8($sp)
	lw	$17,4($sp)
	lw	$16,0($sp)
	j	sceIoClose
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	dlogi2
	.size	dlogi2, .-dlogi2
	.align	2
	.globl	dlog2
	.ent	dlog2
dlog2:
	.frame	$sp,16,$31		# vars= 0, regs= 4/0, args= 0, gp= 0
	.mask	0x80070000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-16
	sw	$31,12($sp)
	sw	$18,8($sp)
	sw	$17,4($sp)
	move	$18,$5
	sw	$16,0($sp)
	jal	strlen
	move	$17,$4

	move	$4,$18
	jal	strlen
	move	$16,$2

	move	$7,$2
	move	$4,$17
	move	$5,$16
	move	$6,$18
	lw	$31,12($sp)
	lw	$18,8($sp)
	lw	$17,4($sp)
	lw	$16,0($sp)
	j	dlogi2
	addiu	$sp,$sp,16

	.set	macro
	.set	reorder
	.end	dlog2
	.size	dlog2, .-dlog2
	.data
	.align	2
	.type	hex.2098, @object
	.size	hex.2098, 17
hex.2098:
	.ascii	"0123456789ABCDEF\000"
	.text
	.align	2
	.globl	dloghex8
	.ent	dloghex8
dloghex8:
	.frame	$sp,16,$31		# vars= 8, regs= 1/0, args= 0, gp= 0
	.mask	0x80000000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-16
	lui	$2,%hi(hex.2098)
	sw	$31,8($sp)
	addiu	$8,$2,%lo(hex.2098)
	li	$5,28			# 0x1c
	move	$6,$sp
	li	$7,-4			# 0xfffffffffffffffc
$L125:
	sra	$2,$4,$5
	andi	$2,$2,0xf
	addu	$2,$2,$8
	lbu	$3,0($2)
	addiu	$5,$5,-4
	sb	$3,0($6)
	bne	$5,$7,$L125
	addiu	$6,$6,1

	move	$4,$sp
	li	$5,8			# 0x8
	jal	dlogi
	li	$6,1			# 0x1

	lw	$31,8($sp)
	j	$31
	addiu	$sp,$sp,16

	.set	macro
	.set	reorder
	.end	dloghex8
	.size	dloghex8, .-dloghex8
	.data
	.align	2
	.type	hex.2114, @object
	.size	hex.2114, 17
hex.2114:
	.ascii	"0123456789ABCDEF\000"
	.text
	.align	2
	.globl	dlog2hex8
	.ent	dlog2hex8
dlog2hex8:
	.frame	$sp,16,$31		# vars= 8, regs= 2/0, args= 0, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-16
	lui	$2,%hi(hex.2114)
	sw	$16,8($sp)
	sw	$31,12($sp)
	move	$16,$4
	addiu	$8,$2,%lo(hex.2114)
	li	$4,28			# 0x1c
	move	$6,$sp
	li	$7,-4			# 0xfffffffffffffffc
$L131:
	sra	$2,$5,$4
	andi	$2,$2,0xf
	addu	$2,$2,$8
	lbu	$3,0($2)
	addiu	$4,$4,-4
	sb	$3,0($6)
	bne	$4,$7,$L131
	addiu	$6,$6,1

	jal	strlen
	move	$4,$16

	move	$5,$2
	move	$4,$16
	move	$6,$sp
	jal	dlogi2
	li	$7,8			# 0x8

	lw	$31,12($sp)
	lw	$16,8($sp)
	j	$31
	addiu	$sp,$sp,16

	.set	macro
	.set	reorder
	.end	dlog2hex8
	.size	dlog2hex8, .-dlog2hex8
	.align	2
	.globl	clear_dlog
	.ent	clear_dlog
clear_dlog:
	.frame	$sp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x80000000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lui	$4,%hi($LC0)
	addiu	$sp,$sp,-8
	addiu	$4,$4,%lo($LC0)
	li	$5,1538			# 0x602
	sw	$31,0($sp)
	jal	sceIoOpen
	li	$6,511			# 0x1ff

	jal	sceIoClose
	move	$4,$2

	li	$3,1			# 0x1
	lui	$2,%hi(g_loginitialised)
	sw	$3,%lo(g_loginitialised)($2)
	lw	$31,0($sp)
	j	$31
	addiu	$sp,$sp,8

	.set	macro
	.set	reorder
	.end	clear_dlog
	.size	clear_dlog, .-clear_dlog
	.align	2
	.globl	memset
	.ent	memset
memset:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	blez	$6,$L145
	nop

	move	$2,$0
$L141:
	addiu	$2,$2,1
	sw	$5,0($4)
	bne	$6,$2,$L141
	addiu	$4,$4,4

$L145:
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	memset
	.size	memset, .-memset
	.align	2
	.globl	stricmp
	.ent	stricmp
stricmp:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lbu	$3,0($4)
	beq	$3,$0,$L148
	nop

	lbu	$6,0($5)
	sltu	$2,$6,$3
	beql	$2,$0,$L158
	sltu	$2,$3,$6

$L150:
	j	$31
	li	$2,1			# 0x1

$L148:
	j	$31
	move	$2,$0

$L158:
	bne	$2,$0,$L152
	move	$6,$0

	addiu	$6,$6,1
$L159:
	addu	$2,$4,$6
	lbu	$3,0($2)
	beq	$3,$0,$L148
	addu	$7,$5,$6

	lbu	$2,0($7)
	sltu	$7,$3,$2
	sltu	$2,$2,$3
	bne	$2,$0,$L150
	nop

	beq	$7,$0,$L159
	addiu	$6,$6,1

$L152:
	j	$31
	li	$2,-1			# 0xffffffffffffffff

	.set	macro
	.set	reorder
	.end	stricmp
	.size	stricmp, .-stricmp
	.data
	.align	2
	.type	hex.2200, @object
	.size	hex.2200, 17
hex.2200:
	.ascii	"0123456789ABCDEF\000"
	.text
	.align	2
	.globl	numtohex8
	.ent	numtohex8
numtohex8:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lui	$2,%hi(hex.2200)
	addiu	$8,$2,%lo(hex.2200)
	li	$6,28			# 0x1c
	li	$7,-4			# 0xfffffffffffffffc
$L161:
	sra	$2,$5,$6
	andi	$2,$2,0xf
	addu	$2,$2,$8
	lbu	$3,0($2)
	addiu	$6,$6,-4
	sb	$3,0($4)
	bne	$6,$7,$L161
	addiu	$4,$4,1

	j	$31
	nop

	.set	macro
	.set	reorder
	.end	numtohex8
	.size	numtohex8, .-numtohex8
	.align	2
	.globl	clearCache
	.ent	clearCache
clearCache:
	.frame	$sp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x80000000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-8
	sw	$31,0($sp)
	jal	sceKernelDcacheWritebackInvalidateAll
	nop

	jal	sceKernelIcacheInvalidateAll
	nop

	li	$3,9961472			# 0x980000
	ori	$3,$3,0x967f
	move	$2,$0
	addiu	$2,$2,1
$L172:
	bne	$2,$3,$L172
	addiu	$2,$2,1

	lw	$31,0($sp)
	li	$2,9961472			# 0x980000
	ori	$2,$2,0x967f
	j	$31
	addiu	$sp,$sp,8

	.set	macro
	.set	reorder
	.end	clearCache
	.size	clearCache, .-clearCache
	.align	2
	.globl	fillvideo
	.ent	fillvideo
fillvideo:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	li	$2,1140850688			# 0x44000000
	li	$3,1142947840			# 0x44200000
	sw	$4,0($2)
$L179:
	addiu	$2,$2,4
	bnel	$2,$3,$L179
	sw	$4,0($2)

	j	$31
	nop

	.set	macro
	.set	reorder
	.end	fillvideo
	.size	fillvideo, .-fillvideo
	.align	2
	.globl	find_gta_sceResident
	.ent	find_gta_sceResident
find_gta_sceResident:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	move	$9,$5
	beq	$5,$0,$L181
	move	$10,$4

	lui	$2,%hi(hash_values)
	addiu	$11,$2,%lo(hash_values)
	move	$8,$0
	move	$6,$0
	move	$7,$0
	j	$L183
	li	$12,20			# 0x14

$L192:
	move	$8,$0
	move	$7,$0
	addiu	$6,$6,1
$L194:
	beql	$9,$6,$L193
	move	$8,$0

$L183:
	addu	$3,$7,$11
	addu	$2,$6,$10
	lbu	$5,0($2)
	lbu	$4,0($3)
	addiu	$7,$7,1
	bne	$5,$4,$L192
	movz	$8,$6,$8

	bne	$7,$12,$L194
	addiu	$6,$6,1

	j	$31
	move	$2,$8

$L181:
	move	$8,$0
$L193:
	j	$31
	move	$2,$8

	.set	macro
	.set	reorder
	.end	find_gta_sceResident
	.size	find_gta_sceResident, .-find_gta_sceResident
	.align	2
	.globl	findKnownSyscall
	.ent	findKnownSyscall
findKnownSyscall:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lui	$3,%hi(functions)
	lw	$2,%lo(functions)($3)
	beq	$2,$0,$L197
	nop

	beq	$2,$4,$L206
	move	$6,$0

	lui	$2,%hi(functions+16)
	addiu	$2,$2,%lo(functions+16)
	lw	$5,0($2)
$L207:
	addiu	$6,$6,1
	beq	$5,$0,$L197
	addiu	$2,$2,16

	bnel	$5,$4,$L207
	lw	$5,0($2)

	addiu	$3,$3,%lo(functions)
	sll	$2,$6,4
	addu	$2,$2,$3
	j	$31
	lw	$2,12($2)

$L197:
	j	$31
	move	$2,$0

$L206:
	addiu	$3,$3,%lo(functions)
	sll	$2,$6,4
	addu	$2,$2,$3
	j	$31
	lw	$2,12($2)

	.set	macro
	.set	reorder
	.end	findKnownSyscall
	.size	findKnownSyscall, .-findKnownSyscall
	.section	.rodata.str1.4
	.align	2
$LC2:
	.ascii	"Found syscall for NID:\000"
	.align	2
$LC3:
	.ascii	"Replace with syscall: \000"
	.align	2
$LC4:
	.ascii	"Firmware detected: \000"
	.align	2
$LC5:
	.ascii	"Found syscall for missing NID:\000"
	.section	.rodata.cst4,"aM",@progbits,4
	.align	2
$LC6:
	.word	-16777201
	.text
	.align	2
	.globl	findNIDs
	.ent	findNIDs
findNIDs:
	.frame	$sp,64,$31		# vars= 24, regs= 10/0, args= 0, gp= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-64
	sw	$23,52($sp)
	sw	$22,48($sp)
	sw	$20,40($sp)
	sw	$31,60($sp)
	sw	$fp,56($sp)
	sw	$21,44($sp)
	sw	$19,36($sp)
	sw	$18,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	lui	$2,%hi(functions)
	lw	$3,%lo(functions)($2)
	move	$20,$4
	move	$23,$6
	beq	$3,$0,$L209
	move	$22,$7

	lui	$2,%hi(functions+16)
	addiu	$19,$2,%lo(functions+16)
	sltu	$2,$6,$7
	j	$L211
	sw	$2,8($sp)

$L226:
$L271:
	beq	$2,$0,$L209
	addiu	$19,$19,16

$L211:
	lw	$4,8($sp)
	beql	$4,$0,$L271
	lw	$2,0($19)

	addu	$16,$20,$23
	j	$L212
	move	$17,$23

$L213:
	addiu	$17,$17,20
	sltu	$2,$17,$22
	beql	$2,$0,$L271
	lw	$2,0($19)

$L212:
	lw	$4,0($16)
	li	$18,-142606336			# 0xfffffffff7800000
	lw	$5,-8($19)
	addu	$4,$4,$18
	jal	stricmp
	addu	$4,$20,$4

	bne	$2,$0,$L213
	addiu	$16,$16,20

	addu	$2,$20,$17
	lw	$3,16($2)
	lw	$4,12($2)
	lhu	$fp,10($2)
	addu	$2,$3,$18
	beq	$fp,$0,$L215
	addu	$3,$4,$18

	addu	$17,$3,$20
	addu	$18,$20,$2
	addiu	$3,$19,-16
	addiu	$4,$19,-4
	addiu	$2,$19,-12
	move	$21,$0
	move	$8,$0
	sw	$3,4($sp)
	sw	$4,12($sp)
	j	$L217
	sw	$2,16($sp)

$L218:
	addiu	$21,$21,1
$L272:
	sltu	$2,$21,$fp
	addiu	$17,$17,4
	beq	$2,$0,$L261
	addiu	$18,$18,8

$L217:
	lw	$3,0($17)
	lw	$2,-16($19)
	bnel	$3,$2,$L272
	addiu	$21,$21,1

	lw	$3,4($18)
	sw	$3,0($sp)
	lw	$4,0($sp)
	lw	$3,-12($19)
	srl	$2,$4,6
	bne	$3,$0,$L220
	sw	$2,-4($19)

	addiu	$21,$21,1
	sltu	$2,$21,$fp
	li	$8,1			# 0x1
	addiu	$17,$17,4
	bne	$2,$0,$L217
	addiu	$18,$18,8

$L261:
	bnel	$8,$0,$L226
	lw	$2,0($19)

$L215:
	lui	$5,%hi(numfailed)
	lw	$4,%lo(numfailed)($5)
	lw	$6,-16($19)
	lui	$2,%hi(failednids)
	sll	$3,$4,2
	addiu	$2,$2,%lo(failednids)
	addu	$3,$3,$2
	sw	$6,0($3)
	lw	$2,0($19)
	addiu	$4,$4,1
	sw	$4,%lo(numfailed)($5)
	bne	$2,$0,$L211
	addiu	$19,$19,16

$L209:
	li	$4,691732480			# 0x293b0000
	jal	findKnownSyscall
	ori	$4,$4,0x45b8

	li	$3,8315			# 0x207b
	beql	$2,$3,$L262
	li	$4,89587712			# 0x5570000

	li	$4,-74579968			# 0xfffffffffb8e0000
	jal	findKnownSyscall
	ori	$4,$4,0x22ec

	li	$4,-905707520			# 0xffffffffca040000
	move	$16,$2
	jal	findKnownSyscall
	ori	$4,$4,0xa2b9

	addiu	$16,$16,-2
	beq	$2,$16,$L263
	li	$2,260			# 0x104

	lui	$22,%hi(gDetectedFirmware)
	sw	$2,%lo(gDetectedFirmware)($22)
$L232:
	lw	$5,%lo(gDetectedFirmware)($22)
$L270:
	lui	$4,%hi($LC4)
	jal	dlog2hex8
	addiu	$4,$4,%lo($LC4)

	lui	$2,%hi(missing_functions)
	lw	$3,%lo(missing_functions)($2)
	bne	$3,$0,$L264
	lui	$2,%hi(missing_functions+8)

$L251:
	lw	$31,60($sp)
	lw	$fp,56($sp)
	lw	$23,52($sp)
	lw	$22,48($sp)
	lw	$21,44($sp)
	lw	$20,40($sp)
	lw	$19,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	j	$31
	addiu	$sp,$sp,64

$L264:
	addiu	$17,$2,%lo(missing_functions+8)
	lw	$4,0($17)
	li	$3,-16777216			# 0xffffffffff000000
	lui	$2,%hi(failednids)
	ori	$20,$3,0xf
	jal	findKnownSyscall
	addiu	$21,$2,%lo(failednids)

	lw	$3,%lo(gDetectedFirmware)($22)
	move	$16,$2
	li	$2,250			# 0xfa
	lui	$19,%hi(numfailed)
	beq	$3,$2,$L241
	move	$18,$17

	slt	$2,$3,251
$L273:
	beq	$2,$0,$L245
	li	$2,260			# 0x104

	li	$2,200			# 0xc8
	beq	$3,$2,$L239
	li	$2,205			# 0xcd

	beql	$3,$2,$L265
	lw	$2,16($17)

$L238:
	lw	$2,-4($17)
$L274:
	li	$3,1073741824			# 0x40000000
	sll	$4,$16,6
	or	$2,$2,$3
	move	$3,$2
	ori	$4,$4,0xc
	move	$7,$20
	li	$6,12			# 0xc
	addiu	$5,$2,80
	j	$L246
	sw	$2,-4($17)

$L266:
	beq	$5,$3,$L260
	lw	$2,%lo(numfailed)($19)

$L246:
	lw	$2,0($3)
	and	$2,$2,$7
	bnel	$2,$6,$L266
	addiu	$3,$3,4

	sw	$4,0($3)
	lw	$5,-8($17)
	lui	$4,%hi($LC5)
	jal	dlog2hex8
	addiu	$4,$4,%lo($LC5)

	lui	$4,%hi($LC3)
	addiu	$4,$4,%lo($LC3)
	jal	dlog2hex8
	move	$5,$16

$L249:
	lw	$2,28($18)
	beq	$2,$0,$L251
	addiu	$17,$17,36

	lw	$4,0($17)
	jal	findKnownSyscall
	move	$18,$17

	lw	$3,%lo(gDetectedFirmware)($22)
	move	$16,$2
	li	$2,250			# 0xfa
	bne	$3,$2,$L273
	slt	$2,$3,251

$L241:
	lw	$2,8($17)
	j	$L238
	addu	$16,$16,$2

$L260:
	lw	$4,-8($17)
	sll	$3,$2,2
	addu	$3,$3,$21
	addiu	$2,$2,1
	sw	$4,0($3)
	j	$L249
	sw	$2,%lo(numfailed)($19)

$L245:
	beq	$3,$2,$L243
	li	$2,265			# 0x109

	beq	$3,$2,$L244
	li	$2,255			# 0xff

	bnel	$3,$2,$L274
	lw	$2,-4($17)

	lw	$2,20($17)
	j	$L238
	addu	$16,$16,$2

$L239:
	lw	$2,4($17)
	j	$L238
	addu	$16,$16,$2

$L243:
	lw	$2,12($17)
	j	$L238
	addu	$16,$16,$2

$L244:
	lw	$2,24($17)
	j	$L238
	addu	$16,$16,$2

$L265:
	j	$L238
	addu	$16,$16,$2

$L262:
	jal	findKnownSyscall
	ori	$4,$4,0x2a5f

	li	$3,8434			# 0x20f2
	beq	$2,$3,$L268
	li	$2,205			# 0xcd

	lui	$22,%hi(gDetectedFirmware)
	move	$4,$0
	li	$5,-1			# 0xffffffffffffffff
	jal	flashscreen
	sw	$2,%lo(gDetectedFirmware)($22)

	j	$L270
	lw	$5,%lo(gDetectedFirmware)($22)

$L220:
	li	$2,1073741824			# 0x40000000
	or	$4,$3,$2
	lui	$3,%hi($LC6)
	addiu	$3,$3,%lo($LC6)
	lw	$7,0($3)
	move	$16,$0
	li	$6,12			# 0xc
	li	$5,80			# 0x50
	j	$L222
	sw	$4,-12($19)

$L269:
	beql	$16,$5,$L272
	addiu	$21,$21,1

$L222:
	addu	$3,$4,$16
	lw	$2,0($3)
	and	$2,$2,$7
	bnel	$2,$6,$L269
	addiu	$16,$16,4

	lw	$2,4($sp)
	lui	$3,%hi($LC2)
	addiu	$4,$3,%lo($LC2)
	jal	dlog2hex8
	lw	$5,0($2)

	lw	$4,12($sp)
	lui	$2,%hi($LC3)
	lw	$5,0($4)
	jal	dlog2hex8
	addiu	$4,$2,%lo($LC3)

	lw	$3,16($sp)
	lw	$4,0($sp)
	li	$8,1			# 0x1
	lw	$2,0($3)
	addu	$2,$16,$2
	j	$L218
	sw	$4,0($2)

$L263:
	lui	$22,%hi(gDetectedFirmware)
	li	$2,250			# 0xfa
	j	$L232
	sw	$2,%lo(gDetectedFirmware)($22)

$L268:
	lui	$22,%hi(gDetectedFirmware)
	li	$2,200			# 0xc8
	j	$L232
	sw	$2,%lo(gDetectedFirmware)($22)

	.set	macro
	.set	reorder
	.end	findNIDs
	.size	findNIDs, .-findNIDs
	.align	2
	.globl	listNIDs
	.ent	listNIDs
listNIDs:
	.frame	$sp,32,$31		# vars= 0, regs= 8/0, args= 0, gp= 0
	.mask	0x807f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-32
	sltu	$7,$6,$7
	sw	$31,28($sp)
	sw	$22,24($sp)
	sw	$21,20($sp)
	sw	$20,16($sp)
	sw	$19,12($sp)
	sw	$18,8($sp)
	sw	$17,4($sp)
	sw	$16,0($sp)
	beq	$7,$0,$L280
	move	$8,$4

	addu	$2,$4,$6
	lw	$4,16($2)
	lw	$5,12($2)
	lhu	$20,10($2)
	li	$3,-142606336			# 0xfffffffff7800000
	addu	$4,$4,$3
	bne	$20,$0,$L282
	addu	$2,$5,$3

$L280:
	lw	$31,28($sp)
	lw	$22,24($sp)
	lw	$21,20($sp)
	lw	$20,16($sp)
	lw	$19,12($sp)
	lw	$18,8($sp)
	lw	$17,4($sp)
	lw	$16,0($sp)
	j	$31
	addiu	$sp,$sp,32

$L282:
	addu	$18,$8,$4
	addu	$17,$8,$2
	move	$19,$0
	lui	$22,%hi($LC2)
	lui	$21,%hi($LC3)
$L279:
	lw	$16,4($18)
	lw	$5,0($17)
	addiu	$4,$22,%lo($LC2)
	jal	dlog2hex8
	srl	$16,$16,6

	addiu	$19,$19,1
	move	$5,$16
	jal	dlog2hex8
	addiu	$4,$21,%lo($LC3)

	sltu	$2,$19,$20
	addiu	$17,$17,4
	bne	$2,$0,$L279
	addiu	$18,$18,8

	lw	$31,28($sp)
	lw	$22,24($sp)
	lw	$21,20($sp)
	lw	$20,16($sp)
	lw	$19,12($sp)
	lw	$18,8($sp)
	lw	$17,4($sp)
	lw	$16,0($sp)
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	listNIDs
	.size	listNIDs, .-listNIDs
	.align	2
	.globl	initial_setup
	.ent	initial_setup
initial_setup:
	.frame	$sp,16,$31		# vars= 0, regs= 3/0, args= 0, gp= 0
	.mask	0x80030000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-16
	sw	$17,4($sp)
	sw	$16,0($sp)
	move	$17,$4
	sw	$31,8($sp)
	jal	find_gta_sceResident
	move	$16,$5

	addu	$3,$17,$2
	li	$8,-142606336			# 0xfffffffff7800000
	move	$4,$17
	bne	$2,$0,$L287
	move	$5,$16

	lw	$31,8($sp)
	lw	$17,4($sp)
	lw	$16,0($sp)
	j	$31
	addiu	$sp,$sp,16

$L287:
	lw	$2,28($3)
	lw	$31,8($sp)
	lw	$16,0($sp)
	addu	$2,$2,$8
	addu	$2,$17,$2
	lw	$7,48($2)
	lw	$6,44($2)
	lw	$17,4($sp)
	addu	$7,$7,$8
	addu	$6,$6,$8
	j	findNIDs
	addiu	$sp,$sp,16

	.set	macro
	.set	reorder
	.end	initial_setup
	.size	initial_setup, .-initial_setup
	.section	.rodata.str1.4
	.align	2
$LC7:
	.ascii	"Clear cache\000"
	.align	2
$LC8:
	.ascii	"Create thread\000"
	.align	2
$LC9:
	.ascii	"loader_thread\000"
	.align	2
$LC10:
	.ascii	"Error - Couldn't start loader thread!\000"
	.text
	.align	2
	.globl	main
	.ent	main
main:
	.frame	$sp,8,$31		# vars= 0, regs= 2/0, args= 0, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-8
	sw	$31,4($sp)
	jal	clear_dlog
	sw	$16,0($sp)

	li	$5,25165824			# 0x1800000
	jal	initial_setup
	li	$4,142606336			# 0x8800000

	lui	$4,%hi($LC7)
	jal	dlog
	addiu	$4,$4,%lo($LC7)

	jal	clearCache
	nop

	lui	$4,%hi($LC8)
	jal	dlog
	addiu	$4,$4,%lo($LC8)

	lui	$4,%hi($LC9)
	lui	$5,%hi(LoaderThread)
	addiu	$4,$4,%lo($LC9)
	addiu	$5,$5,%lo(LoaderThread)
	li	$6,17			# 0x11
	li	$7,4096			# 0x1000
	move	$8,$0
	jal	sceKernelCreateThread
	move	$9,$0

	bltz	$2,$L289
	move	$5,$0

	move	$4,$2
	jal	sceKernelStartThread
	move	$6,$0

	li	$16,29949952			# 0x1c90000
$L291:
	jal	sceKernelDelayThread
	ori	$4,$16,0xc380

	jal	sceKernelDelayThread
	ori	$4,$16,0xc380

	j	$L291
	nop

$L289:
	lui	$4,%hi($LC10)
	jal	dlog
	addiu	$4,$4,%lo($LC10)

	jal	sceKernelExitGame
	nop

	move	$2,$16
	lw	$31,4($sp)
	lw	$16,0($sp)
	j	$31
	addiu	$sp,$sp,8

	.set	macro
	.set	reorder
	.end	main
	.size	main, .-main
	.section	.text.start,"ax",@progbits
	.align	2
	.globl	_start
	.ent	_start
_start:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	j	main
	nop

	.set	macro
	.set	reorder
	.end	_start
	.size	_start, .-_start
	.section	.rodata.str1.4
	.align	2
$LC11:
	.ascii	"Deleted Thread: \000"
	.text
	.align	2
	.globl	DeleteAllThreads
	.ent	DeleteAllThreads
DeleteAllThreads:
	.frame	$sp,136,$31		# vars= 104, regs= 7/0, args= 0, gp= 0
	.mask	0x803f0000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-136
	sw	$18,112($sp)
	sw	$16,104($sp)
	lui	$18,%hi(nthreads)
	lui	$16,%hi(threads)
	sw	$31,128($sp)
	sw	$19,116($sp)
	sw	$21,124($sp)
	sw	$20,120($sp)
	jal	sceKernelGetThreadId
	sw	$17,108($sp)

	li	$4,1			# 0x1
	addiu	$5,$16,%lo(threads)
	li	$6,100			# 0x64
	addiu	$7,$18,%lo(nthreads)
	move	$19,$2
	jal	sceKernelGetThreadmanIdList
	sw	$0,%lo(nthreads)($18)

	lw	$3,%lo(nthreads)($18)
	bgtz	$3,$L305
	lw	$31,128($sp)

	lw	$21,124($sp)
	lw	$20,120($sp)
	lw	$19,116($sp)
	lw	$18,112($sp)
	lw	$17,108($sp)
	lw	$16,104($sp)
	j	$31
	addiu	$sp,$sp,136

$L305:
	addiu	$16,$16,%lo(threads)
	move	$17,$0
	li	$20,104			# 0x68
	lui	$21,%hi($LC11)
$L298:
	lw	$4,0($16)
	move	$5,$sp
	addiu	$17,$17,1
	jal	sceKernelReferThreadStatus
	sw	$20,0($sp)

	bltz	$2,$L306
	lw	$2,%lo(nthreads)($18)

	lw	$2,0($16)
	beq	$19,$2,$L299
	move	$4,$2

	jal	sceKernelTerminateDeleteThread
	nop

	addiu	$4,$21,%lo($LC11)
	bltz	$2,$L299
	addiu	$5,$sp,4

	jal	dlog2
	nop

$L299:
	lw	$2,%lo(nthreads)($18)
$L306:
	slt	$2,$17,$2
	bne	$2,$0,$L298
	addiu	$16,$16,4

	lw	$31,128($sp)
	lw	$21,124($sp)
	lw	$20,120($sp)
	lw	$19,116($sp)
	lw	$18,112($sp)
	lw	$17,108($sp)
	lw	$16,104($sp)
	j	$31
	addiu	$sp,$sp,136

	.set	macro
	.set	reorder
	.end	DeleteAllThreads
	.size	DeleteAllThreads, .-DeleteAllThreads
	.section	.rodata.str1.4
	.align	2
$LC12:
	.ascii	"Get threadman callback IDs\000"
	.align	2
$LC13:
	.ascii	"Error Cancelling Callback: \000"
	.align	2
$LC14:
	.ascii	"Cancelled Callback: \000"
	.align	2
$LC15:
	.ascii	"Deleted Callback: \000"
	.align	2
$LC16:
	.ascii	"Error deleting Callback: \000"
	.text
	.align	2
	.globl	DeleteAllCallbacks
	.ent	DeleteAllCallbacks
DeleteAllCallbacks:
	.frame	$sp,360,$31		# vars= 320, regs= 9/0, args= 0, gp= 0
	.mask	0x80ff0000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-360
	lui	$4,%hi($LC12)
	sw	$16,320($sp)
	addiu	$4,$4,%lo($LC12)
	addiu	$16,$sp,60
	sw	$31,352($sp)
	sw	$23,348($sp)
	sw	$22,344($sp)
	sw	$21,340($sp)
	sw	$20,336($sp)
	sw	$19,332($sp)
	sw	$18,328($sp)
	jal	dlog
	sw	$17,324($sp)

	li	$2,56			# 0x38
	li	$4,8			# 0x8
	move	$5,$16
	li	$6,1024			# 0x400
	move	$7,$sp
	sw	$2,4($sp)
	sw	$0,8($sp)
	sw	$0,12($sp)
	sw	$0,16($sp)
	sw	$0,20($sp)
	sw	$0,24($sp)
	sw	$0,28($sp)
	sw	$0,32($sp)
	sw	$0,36($sp)
	sw	$0,40($sp)
	sw	$0,44($sp)
	sw	$0,48($sp)
	sw	$0,52($sp)
	sw	$0,56($sp)
	jal	sceKernelGetThreadmanIdList
	sw	$0,0($sp)

	lw	$3,0($sp)
	bgtz	$3,$L320
	lw	$31,352($sp)

	lw	$23,348($sp)
	lw	$22,344($sp)
	lw	$21,340($sp)
	lw	$20,336($sp)
	lw	$19,332($sp)
	lw	$18,328($sp)
	lw	$17,324($sp)
	lw	$16,320($sp)
	j	$31
	addiu	$sp,$sp,360

$L320:
	move	$17,$0
	addiu	$19,$sp,4
	lui	$22,%hi($LC13)
	addiu	$18,$sp,8
	lui	$20,%hi($LC14)
	lui	$21,%hi($LC15)
	lui	$23,%hi($LC16)
$L310:
	lw	$4,0($16)
	jal	sceKernelReferCallbackStatus
	move	$5,$19

	bltz	$2,$L323
	lw	$2,0($sp)

	jal	sceKernelCancelCallback
	lw	$4,0($16)

	move	$5,$18
	bltz	$2,$L321
	addiu	$4,$20,%lo($LC14)

	jal	dlog2
	nop

	jal	sceKernelDeleteCallback
	lw	$4,0($16)

	addiu	$4,$21,%lo($LC15)
	bltz	$2,$L322
	move	$5,$18

$L319:
	jal	dlog2
	nop

	lw	$2,0($sp)
$L323:
	addiu	$17,$17,1
	slt	$2,$17,$2
	bne	$2,$0,$L310
	addiu	$16,$16,4

	lw	$31,352($sp)
	lw	$23,348($sp)
	lw	$22,344($sp)
	lw	$21,340($sp)
	lw	$20,336($sp)
	lw	$19,332($sp)
	lw	$18,328($sp)
	lw	$17,324($sp)
	lw	$16,320($sp)
	j	$31
	addiu	$sp,$sp,360

$L322:
	j	$L319
	addiu	$4,$23,%lo($LC16)

$L321:
	addiu	$4,$22,%lo($LC13)
	jal	dlog2
	addiu	$5,$sp,8

	j	$L323
	lw	$2,0($sp)

	.set	macro
	.set	reorder
	.end	DeleteAllCallbacks
	.size	DeleteAllCallbacks, .-DeleteAllCallbacks
	.section	.rodata.str1.4
	.align	2
$LC17:
	.ascii	"Deleted Fpl: \000"
	.text
	.align	2
	.globl	DeleteAllFpls
	.ent	DeleteAllFpls
DeleteAllFpls:
	.frame	$sp,280,$31		# vars= 264, regs= 4/0, args= 0, gp= 0
	.mask	0x80070000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-280
	sw	$16,264($sp)
	addiu	$16,$sp,4
	li	$4,6			# 0x6
	move	$5,$16
	li	$6,1024			# 0x400
	move	$7,$sp
	sw	$31,276($sp)
	sw	$18,272($sp)
	sw	$17,268($sp)
	jal	sceKernelGetThreadmanIdList
	sw	$0,0($sp)

	lw	$3,0($sp)
	bgtz	$3,$L332
	lw	$31,276($sp)

	lw	$18,272($sp)
	lw	$17,268($sp)
	lw	$16,264($sp)
	j	$31
	addiu	$sp,$sp,280

$L332:
	move	$17,$0
	lui	$18,%hi($LC17)
$L327:
	lw	$4,0($16)
	jal	sceKernelDeleteFpl
	addiu	$17,$17,1

	bltz	$2,$L328
	addiu	$4,$18,%lo($LC17)

	jal	dlog2hex8
	lw	$5,0($16)

$L328:
	lw	$2,0($sp)
	slt	$2,$17,$2
	bne	$2,$0,$L327
	addiu	$16,$16,4

	lw	$31,276($sp)
	lw	$18,272($sp)
	lw	$17,268($sp)
	lw	$16,264($sp)
	j	$31
	addiu	$sp,$sp,280

	.set	macro
	.set	reorder
	.end	DeleteAllFpls
	.size	DeleteAllFpls, .-DeleteAllFpls
	.section	.rodata.str1.4
	.align	2
$LC18:
	.ascii	"Deleted Sema: \000"
	.text
	.align	2
	.globl	DeleteAllSemas
	.ent	DeleteAllSemas
DeleteAllSemas:
	.frame	$sp,280,$31		# vars= 264, regs= 4/0, args= 0, gp= 0
	.mask	0x80070000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-280
	sw	$16,264($sp)
	addiu	$16,$sp,4
	li	$4,2			# 0x2
	move	$5,$16
	li	$6,1024			# 0x400
	move	$7,$sp
	sw	$31,276($sp)
	sw	$18,272($sp)
	sw	$17,268($sp)
	jal	sceKernelGetThreadmanIdList
	sw	$0,0($sp)

	lw	$3,0($sp)
	bgtz	$3,$L341
	lw	$31,276($sp)

	lw	$18,272($sp)
	lw	$17,268($sp)
	lw	$16,264($sp)
	j	$31
	addiu	$sp,$sp,280

$L341:
	move	$17,$0
	lui	$18,%hi($LC18)
$L336:
	lw	$4,0($16)
	jal	sceKernelDeleteSema
	addiu	$17,$17,1

	bltz	$2,$L337
	addiu	$4,$18,%lo($LC18)

	jal	dlog2hex8
	lw	$5,0($16)

$L337:
	lw	$2,0($sp)
	slt	$2,$17,$2
	bne	$2,$0,$L336
	addiu	$16,$16,4

	lw	$31,276($sp)
	lw	$18,272($sp)
	lw	$17,268($sp)
	lw	$16,264($sp)
	j	$31
	addiu	$sp,$sp,280

	.set	macro
	.set	reorder
	.end	DeleteAllSemas
	.size	DeleteAllSemas, .-DeleteAllSemas
	.section	.rodata.str1.4
	.align	2
$LC19:
	.ascii	"Deleted EventFlag: \000"
	.text
	.align	2
	.globl	DeleteAllEventFlags
	.ent	DeleteAllEventFlags
DeleteAllEventFlags:
	.frame	$sp,280,$31		# vars= 264, regs= 4/0, args= 0, gp= 0
	.mask	0x80070000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-280
	sw	$16,264($sp)
	addiu	$16,$sp,4
	li	$4,3			# 0x3
	move	$5,$16
	li	$6,1024			# 0x400
	move	$7,$sp
	sw	$31,276($sp)
	sw	$18,272($sp)
	sw	$17,268($sp)
	jal	sceKernelGetThreadmanIdList
	sw	$0,0($sp)

	lw	$3,0($sp)
	bgtz	$3,$L350
	lw	$31,276($sp)

	lw	$18,272($sp)
	lw	$17,268($sp)
	lw	$16,264($sp)
	j	$31
	addiu	$sp,$sp,280

$L350:
	move	$17,$0
	lui	$18,%hi($LC19)
$L345:
	lw	$4,0($16)
	jal	sceKernelDeleteEventFlag
	addiu	$17,$17,1

	bltz	$2,$L346
	addiu	$4,$18,%lo($LC19)

	jal	dlog2hex8
	lw	$5,0($16)

$L346:
	lw	$2,0($sp)
	slt	$2,$17,$2
	bne	$2,$0,$L345
	addiu	$16,$16,4

	lw	$31,276($sp)
	lw	$18,272($sp)
	lw	$17,268($sp)
	lw	$16,264($sp)
	j	$31
	addiu	$sp,$sp,280

	.set	macro
	.set	reorder
	.end	DeleteAllEventFlags
	.size	DeleteAllEventFlags, .-DeleteAllEventFlags
	.section	.rodata.str1.4
	.align	2
$LC20:
	.ascii	"disable vblank_int returned: \000"
	.align	2
$LC21:
	.ascii	"release vblank_int returned: \000"
	.text
	.align	2
	.globl	DisableVBlanks
	.ent	DisableVBlanks
DisableVBlanks:
	.frame	$sp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x80000000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-8
	li	$4,30			# 0x1e
	sw	$31,0($sp)
	jal	sceKernelDisableSubIntr
	li	$5,30			# 0x1e

	lui	$4,%hi($LC20)
	move	$5,$2
	jal	dlog2hex8
	addiu	$4,$4,%lo($LC20)

	li	$4,196608			# 0x30000
	jal	sceKernelDelayThread
	ori	$4,$4,0xd40

	li	$4,30			# 0x1e
	jal	sceKernelReleaseSubIntrHandler
	li	$5,30			# 0x1e

	lui	$4,%hi($LC21)
	lw	$31,0($sp)
	addiu	$4,$4,%lo($LC21)
	move	$5,$2
	j	dlog2hex8
	addiu	$sp,$sp,8

	.set	macro
	.set	reorder
	.end	DisableVBlanks
	.size	DisableVBlanks, .-DisableVBlanks
	.section	.rodata.str1.4
	.align	2
$LC22:
	.ascii	"disable ge_int returned: \000"
	.align	2
$LC23:
	.ascii	"release ge_int returned: \000"
	.text
	.align	2
	.globl	DisableGeInterrupt
	.ent	DisableGeInterrupt
DisableGeInterrupt:
	.frame	$sp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x80000000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-8
	li	$4,30			# 0x1e
	sw	$31,0($sp)
	jal	sceKernelDisableSubIntr
	li	$5,15			# 0xf

	lui	$4,%hi($LC22)
	move	$5,$2
	jal	dlog2hex8
	addiu	$4,$4,%lo($LC22)

	li	$4,196608			# 0x30000
	jal	sceKernelDelayThread
	ori	$4,$4,0xd40

	li	$4,30			# 0x1e
	jal	sceKernelReleaseSubIntrHandler
	li	$5,15			# 0xf

	lui	$4,%hi($LC23)
	lw	$31,0($sp)
	addiu	$4,$4,%lo($LC23)
	move	$5,$2
	j	dlog2hex8
	addiu	$sp,$sp,8

	.set	macro
	.set	reorder
	.end	DisableGeInterrupt
	.size	DisableGeInterrupt, .-DisableGeInterrupt
	.align	2
	.globl	ReleaseAudioChannels
	.ent	ReleaseAudioChannels
ReleaseAudioChannels:
	.frame	$sp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x80000000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-8
	sw	$31,0($sp)
	jal	sceAudioChRelease
	move	$4,$0

	jal	sceAudioChRelease
	li	$4,1			# 0x1

	jal	sceAudioChRelease
	li	$4,2			# 0x2

	jal	sceAudioChRelease
	li	$4,3			# 0x3

	jal	sceAudioChRelease
	li	$4,4			# 0x4

	jal	sceAudioChRelease
	li	$4,5			# 0x5

	jal	sceAudioChRelease
	li	$4,6			# 0x6

	lw	$31,0($sp)
	li	$4,7			# 0x7
	j	sceAudioChRelease
	addiu	$sp,$sp,8

	.set	macro
	.set	reorder
	.end	ReleaseAudioChannels
	.size	ReleaseAudioChannels, .-ReleaseAudioChannels
	.section	.rodata.str1.4
	.align	2
$LC24:
	.ascii	"ms0:/GTADUMP.BIN\000"
	.text
	.align	2
	.globl	DumpMem
	.ent	DumpMem
DumpMem:
	.frame	$sp,8,$31		# vars= 0, regs= 2/0, args= 0, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lui	$4,%hi($LC24)
	addiu	$sp,$sp,-8
	addiu	$4,$4,%lo($LC24)
	li	$5,1538			# 0x602
	li	$6,511			# 0x1ff
	sw	$31,4($sp)
	jal	sceIoOpen
	sw	$16,0($sp)

	move	$16,$2
	move	$4,$2
	li	$5,138412032			# 0x8400000
	jal	sceIoWrite
	li	$6,29360128			# 0x1c00000

	move	$4,$16
	lw	$31,4($sp)
	lw	$16,0($sp)
	j	sceIoClose
	addiu	$sp,$sp,8

	.set	macro
	.set	reorder
	.end	DumpMem
	.size	DumpMem, .-DumpMem
	.align	2
	.globl	clear_words
	.ent	clear_words
clear_words:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	sltu	$2,$4,$5
	beq	$2,$0,$L366
	addiu	$3,$4,4

	sw	$0,0($4)
$L365:
	move	$4,$3
	addiu	$3,$3,4
	addiu	$2,$3,-4
	sltu	$2,$2,$5
	bnel	$2,$0,$L365
	sw	$0,0($4)

$L366:
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	clear_words
	.size	clear_words, .-clear_words
	.section	.rodata.str1.4
	.align	2
$LC25:
	.ascii	"Failed to resolve some NIDs:\000"
	.align	2
$LC26:
	.ascii	"Failed NID: \000"
	.align	2
$LC27:
	.ascii	"lfirm: \000"
	.align	2
$LC28:
	.ascii	"firmware: \000"
	.align	2
$LC29:
	.ascii	"My stack: \000"
	.align	2
$LC30:
	.ascii	"Syscall for 0x237dbd4f: \000"
	.align	2
$LC31:
	.ascii	"Detected firmware: \000"
	.align	2
$LC32:
	.ascii	"tot free mem:\000"
	.align	2
$LC33:
	.ascii	"max free mem:\000"
	.align	2
$LC34:
	.ascii	"Removed Threads\000"
	.align	2
$LC35:
	.ascii	"Removed Callbacks\000"
	.align	2
$LC36:
	.ascii	"Removed FPLs\000"
	.align	2
$LC37:
	.ascii	"Removed Semas\000"
	.align	2
$LC38:
	.ascii	"Disabled VBlanks\000"
	.align	2
$LC39:
	.ascii	"Disabled Ge interrupt\000"
	.align	2
$LC40:
	.ascii	"Released AudioChannels\000"
	.align	2
$LC41:
	.ascii	"tot free mem post cleanup:\000"
	.align	2
$LC42:
	.ascii	"max free mem post cleanup:\000"
	.align	2
$LC43:
	.ascii	"loader\000"
	.align	2
$LC44:
	.ascii	"Loader code block not at expected addr!\000"
	.align	2
$LC45:
	.ascii	"ms0:/psp/savedata/ulus10041s5/kernel.bin\000"
	.align	2
$LC46:
	.ascii	"failed us, fd: \000"
	.align	2
$LC47:
	.ascii	"ms0:/psp/savedata/ules00151s5/kernel.bin\000"
	.align	2
$LC48:
	.ascii	"failed eu, fd: \000"
	.align	2
$LC49:
	.ascii	"ms0:/psp/savedata/ules00182s5/kernel.bin\000"
	.align	2
$LC50:
	.ascii	"failed de, fd: \000"
	.align	2
$LC51:
	.ascii	"fd: \000"
	.align	2
$LC52:
	.ascii	"Savedata status 1: \000"
	.align	2
$LC53:
	.ascii	"Status loop: \000"
	.align	2
$LC54:
	.ascii	"Savedata status: \000"
	.align	2
$LC55:
	.ascii	"Shutdown returned: \000"
	.align	2
$LC56:
	.ascii	"Call code at 0x09EFD004\000"
	.align	2
$LC57:
	.ascii	"chhikr hitchhikr hitchhikr hitchhikr hitchik\000"
	.align	2
$LC58:
	.ascii	"ms0:/odd.BIN\000"
	.align	2
$LC59:
	.ascii	"Error - Couldn't alloc memory for loader bin!\000"
	.text
	.align	2
	.globl	run_loader
	.ent	run_loader
run_loader:
	.frame	$sp,280,$31		# vars= 256, regs= 6/0, args= 0, gp= 0
	.mask	0x801f0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-280
	sw	$18,264($sp)
	sw	$31,276($sp)
	sw	$20,272($sp)
	sw	$19,268($sp)
	sw	$17,260($sp)
	sw	$16,256($sp)
	lui	$18,%hi(numfailed)
	lw	$2,%lo(numfailed)($18)
	.set	noreorder
	.set	nomacro
	blezl	$2,$L397
	lui	$17,%hi(gDetectedFirmware)
	.set	macro
	.set	reorder

	lui	$4,%hi($LC25)
	.set	noreorder
	.set	nomacro
	jal	dlog
	addiu	$4,$4,%lo($LC25)
	.set	macro
	.set	reorder

	lw	$2,%lo(numfailed)($18)
	.set	noreorder
	.set	nomacro
	bgtz	$2,$L395
	lui	$2,%hi(failednids)
	.set	macro
	.set	reorder

	lui	$17,%hi(gDetectedFirmware)
$L397:
	lw	$18,%lo(gDetectedFirmware)($17)
	lui	$19,%hi($LC27)
	addiu	$4,$19,%lo($LC27)
	.set	noreorder
	.set	nomacro
	jal	dlog2hex8
	move	$5,$18
	.set	macro
	.set	reorder

	lw	$5,%lo(gDetectedFirmware)($17)
	lui	$20,%hi($LC28)
	.set	noreorder
	.set	nomacro
	jal	dlog2hex8
	addiu	$4,$20,%lo($LC28)
	.set	macro
	.set	reorder

	lui	$4,%hi($LC29)
	addiu	$4,$4,%lo($LC29)
 #APP
	move $5, $29
 #NO_APP
	.set	noreorder
	.set	nomacro
	jal	dlog2hex8
	li	$16,166658048			# 0x9ef0000
	.set	macro
	.set	reorder

	li	$4,595394560			# 0x237d0000
	.set	noreorder
	.set	nomacro
	jal	findKnownSyscall
	ori	$4,$4,0xbd4f
	.set	macro
	.set	reorder

	lui	$4,%hi($LC30)
	move	$5,$2
	.set	noreorder
	.set	nomacro
	jal	dlog2hex8
	addiu	$4,$4,%lo($LC30)
	.set	macro
	.set	reorder

	lw	$5,%lo(gDetectedFirmware)($17)
	lui	$4,%hi($LC31)
	.set	noreorder
	.set	nomacro
	jal	dlog2hex8
	addiu	$4,$4,%lo($LC31)
	.set	macro
	.set	reorder

	jal	sceKernelTotalFreeMemSize
	lui	$4,%hi($LC32)
	addiu	$4,$4,%lo($LC32)
	.set	noreorder
	.set	nomacro
	jal	dlog2hex8
	move	$5,$2
	.set	macro
	.set	reorder

	jal	sceKernelMaxFreeMemSize
	lui	$4,%hi($LC33)
	move	$5,$2
	.set	noreorder
	.set	nomacro
	jal	dlog2hex8
	addiu	$4,$4,%lo($LC33)
	.set	macro
	.set	reorder

	jal	DeleteAllThreads
	lui	$4,%hi($LC34)
	.set	noreorder
	.set	nomacro
	jal	dlog
	addiu	$4,$4,%lo($LC34)
	.set	macro
	.set	reorder

	jal	DeleteAllCallbacks
	lui	$4,%hi($LC35)
	.set	noreorder
	.set	nomacro
	jal	dlog
	addiu	$4,$4,%lo($LC35)
	.set	macro
	.set	reorder

	jal	DeleteAllFpls
	lui	$4,%hi($LC36)
	.set	noreorder
	.set	nomacro
	jal	dlog
	addiu	$4,$4,%lo($LC36)
	.set	macro
	.set	reorder

	jal	DeleteAllSemas
	lui	$4,%hi($LC37)
	.set	noreorder
	.set	nomacro
	jal	dlog
	addiu	$4,$4,%lo($LC37)
	.set	macro
	.set	reorder

	jal	DisableVBlanks
	lui	$4,%hi($LC38)
	.set	noreorder
	.set	nomacro
	jal	dlog
	addiu	$4,$4,%lo($LC38)
	.set	macro
	.set	reorder

	jal	DisableGeInterrupt
	lui	$4,%hi($LC39)
	.set	noreorder
	.set	nomacro
	jal	dlog
	addiu	$4,$4,%lo($LC39)
	.set	macro
	.set	reorder

	jal	ReleaseAudioChannels
	lui	$4,%hi($LC40)
	.set	noreorder
	.set	nomacro
	jal	dlog
	addiu	$4,$4,%lo($LC40)
	.set	macro
	.set	reorder

	jal	sceKernelTotalFreeMemSize
	lui	$4,%hi($LC41)
	addiu	$4,$4,%lo($LC41)
	.set	noreorder
	.set	nomacro
	jal	dlog2hex8
	move	$5,$2
	.set	macro
	.set	reorder

	jal	sceKernelMaxFreeMemSize
	lui	$4,%hi($LC42)
	move	$5,$2
	.set	noreorder
	.set	nomacro
	jal	dlog2hex8
	addiu	$4,$4,%lo($LC42)
	.set	macro
	.set	reorder

	lui	$5,%hi($LC43)
	addiu	$5,$5,%lo($LC43)
	li	$4,2			# 0x2
	li	$6,2			# 0x2
	li	$7,524288			# 0x80000
	.set	noreorder
	.set	nomacro
	jal	sceKernelAllocPartitionMemory
	ori	$8,$16,0xd000
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bltz	$2,$L372
	lui	$4,%hi($LC59)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	sceKernelGetBlockHeadAddr
	move	$4,$2
	.set	macro
	.set	reorder

	ori	$3,$16,0xd000
	.set	noreorder
	.set	nomacro
	beql	$2,$3,$L399
	addiu	$4,$19,%lo($LC27)
	.set	macro
	.set	reorder

	lui	$4,%hi($LC44)
	.set	noreorder
	.set	nomacro
	jal	dlog
	addiu	$4,$4,%lo($LC44)
	.set	macro
	.set	reorder

	jal	sceKernelExitGame
	addiu	$4,$19,%lo($LC27)
$L399:
	.set	noreorder
	.set	nomacro
	jal	dlog2hex8
	move	$5,$18
	.set	macro
	.set	reorder

	lw	$5,%lo(gDetectedFirmware)($17)
	.set	noreorder
	.set	nomacro
	jal	dlog2hex8
	addiu	$4,$20,%lo($LC28)
	.set	macro
	.set	reorder

	lui	$4,%hi($LC45)
	addiu	$4,$4,%lo($LC45)
	li	$5,1			# 0x1
	.set	noreorder
	.set	nomacro
	jal	sceIoOpen
	li	$6,511			# 0x1ff
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bltz	$2,$L396
	move	$16,$2
	.set	macro
	.set	reorder

$L376:
	lui	$4,%hi($LC51)
$L398:
	move	$5,$16
	.set	noreorder
	.set	nomacro
	jal	dlog2hex8
	addiu	$4,$4,%lo($LC51)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	fillvideo
	move	$4,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	clearCache
	lui	$20,%hi($LC53)
	.set	macro
	.set	reorder

	li	$5,1240399872			# 0x49ef0000
	li	$6,524288			# 0x80000
	move	$4,$16
	.set	noreorder
	.set	nomacro
	jal	sceIoRead
	ori	$5,$5,0xd000
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	clearCache
	lui	$19,%hi($LC54)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	sceIoClose
	move	$4,$16
	.set	macro
	.set	reorder

	move	$4,$0
	.set	noreorder
	.set	nomacro
	jal	flashscreen
	move	$5,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	sceUtilitySavedataGetStatus
	move	$16,$0
	.set	macro
	.set	reorder

	lui	$4,%hi($LC52)
	addiu	$4,$4,%lo($LC52)
	move	$5,$2
	.set	noreorder
	.set	nomacro
	jal	dlog2hex8
	move	$17,$2
	.set	macro
	.set	reorder

	li	$18,10			# 0xa
	move	$5,$16
$L400:
	.set	noreorder
	.set	nomacro
	jal	dlog2hex8
	addiu	$4,$20,%lo($LC53)
	.set	macro
	.set	reorder

	addiu	$16,$16,1
	addiu	$4,$19,%lo($LC54)
	.set	noreorder
	.set	nomacro
	jal	dlog2hex8
	move	$5,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bnel	$16,$18,$L400
	move	$5,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	sceUtilitySavedataShutdownStart
	move	$16,$0
	.set	macro
	.set	reorder

	lui	$4,%hi($LC55)
	addiu	$4,$4,%lo($LC55)
	.set	noreorder
	.set	nomacro
	jal	dlog2hex8
	move	$5,$2
	.set	macro
	.set	reorder

	li	$18,10			# 0xa
	move	$5,$16
$L401:
	.set	noreorder
	.set	nomacro
	jal	dlog2hex8
	addiu	$4,$20,%lo($LC53)
	.set	macro
	.set	reorder

	addiu	$16,$16,1
	addiu	$4,$19,%lo($LC54)
	.set	noreorder
	.set	nomacro
	jal	dlog2hex8
	move	$5,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$16,$18,$L401
	move	$5,$16
	.set	macro
	.set	reorder

	lui	$4,%hi($LC56)
	.set	noreorder
	.set	nomacro
	jal	dlog
	addiu	$4,$4,%lo($LC56)
	.set	macro
	.set	reorder

	jal	sceKernelDcacheWritebackAll
	move	$4,$sp
	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	memset
	li	$6,256			# 0x100
	.set	macro
	.set	reorder

	lui	$3,%hi($LC57)
	lui	$2,%hi($LC57+44)
	addiu	$3,$3,%lo($LC57)
	addiu	$5,$2,%lo($LC57+44)
	move	$4,$sp
$L384:
	lbu	$2,0($3)
	addiu	$3,$3,1
	sb	$2,0($4)
	.set	noreorder
	.set	nomacro
	bne	$3,$5,$L384
	addiu	$4,$4,1
	.set	macro
	.set	reorder

	lui	$4,%hi($LC58)
	li	$2,166658048			# 0x9ef0000
	li	$3,58
	addiu	$4,$4,%lo($LC58)
	li	$6,511			# 0x1ff
	li	$5,1538			# 0x602
	ori	$2,$2,0xd004
	sb	$3,48($sp)
	sw	$2,44($sp)
	.set	noreorder
	.set	nomacro
	jal	sceIoOpen
	sb	$0,49($sp)
	.set	macro
	.set	reorder

	move	$16,$2
	addiu	$5,$sp,44
	li	$6,4			# 0x4
	.set	noreorder
	.set	nomacro
	jal	sceIoWrite
	move	$4,$2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	sceIoClose
	move	$4,$16
	.set	macro
	.set	reorder

	move	$4,$sp
	.set	noreorder
	.set	nomacro
	jal	sceKernelLoadExec
	move	$5,$0
	.set	macro
	.set	reorder

	lw	$31,276($sp)
	lw	$20,272($sp)
	lw	$19,268($sp)
	lw	$18,264($sp)
	lw	$17,260($sp)
	lw	$16,256($sp)
	move	$2,$0
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,280
	.set	macro
	.set	reorder

$L372:
	.set	noreorder
	.set	nomacro
	jal	dlog
	addiu	$4,$4,%lo($LC59)
	.set	macro
	.set	reorder

	jal	sceKernelExitGame
	lw	$31,276($sp)
	lw	$20,272($sp)
	lw	$19,268($sp)
	lw	$18,264($sp)
	lw	$17,260($sp)
	lw	$16,256($sp)
	move	$2,$0
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,280
	.set	macro
	.set	reorder

$L395:
	addiu	$17,$2,%lo(failednids)
	move	$16,$0
	lui	$19,%hi($LC26)
$L371:
	lw	$5,0($17)
	addiu	$4,$19,%lo($LC26)
	.set	noreorder
	.set	nomacro
	jal	dlog2hex8
	addiu	$16,$16,1
	.set	macro
	.set	reorder

	lw	$2,%lo(numfailed)($18)
	slt	$2,$16,$2
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L371
	addiu	$17,$17,4
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L397
	lui	$17,%hi(gDetectedFirmware)
	.set	macro
	.set	reorder

$L396:
	lui	$4,%hi($LC46)
	move	$5,$2
	.set	noreorder
	.set	nomacro
	jal	dlog2hex8
	addiu	$4,$4,%lo($LC46)
	.set	macro
	.set	reorder

	lui	$4,%hi($LC47)
	addiu	$4,$4,%lo($LC47)
	li	$5,1			# 0x1
	.set	noreorder
	.set	nomacro
	jal	sceIoOpen
	li	$6,511			# 0x1ff
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bgez	$2,$L376
	move	$16,$2
	.set	macro
	.set	reorder

	lui	$4,%hi($LC48)
	move	$5,$2
	.set	noreorder
	.set	nomacro
	jal	dlog2hex8
	addiu	$4,$4,%lo($LC48)
	.set	macro
	.set	reorder

	lui	$4,%hi($LC49)
	addiu	$4,$4,%lo($LC49)
	li	$5,1			# 0x1
	.set	noreorder
	.set	nomacro
	jal	sceIoOpen
	li	$6,511			# 0x1ff
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bgez	$2,$L376
	move	$16,$2
	.set	macro
	.set	reorder

	lui	$4,%hi($LC50)
	addiu	$4,$4,%lo($LC50)
	.set	noreorder
	.set	nomacro
	jal	dlog2hex8
	move	$5,$2
	.set	macro
	.set	reorder

	jal	sceKernelExitGame
	.set	noreorder
	.set	nomacro
	j	$L398
	lui	$4,%hi($LC51)
	.set	macro
	.set	reorder

	.end	run_loader
	.size	run_loader, .-run_loader
	.align	2
	.globl	LoaderThread
	.ent	LoaderThread
LoaderThread:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	j	run_loader
	nop

	.set	macro
	.set	reorder
	.end	LoaderThread
	.size	LoaderThread, .-LoaderThread

	.comm	nthreads,4,4

	.comm	threads,100,4

	.comm	failednids,200,4

	.comm	gDetectedFirmware,4,4
	.ident	"GCC: (GNU) 4.0.1 (PSPDEV 20050827)"
