/* NO-KXploit Patch, moonlight */

	.set noreorder

	.extern hooked_LoadExecForKernel_28D0D249
	.extern copyBootMem

	.global Bootstrap
	.ent    Bootstrap

Bootstrap:
	/* We are the new system bootstrap ;) */

	addiu		$sp, $sp, -4
	sw		$ra, 0($sp)
	
	/* sw(j RedirectLoaderCoreInst, 0x88c00ff8) */

/*
 Copy across 0x88c00000 to another memory region so that it can be dumped later
 Maybe try 0x9E00000 to store it in
*/
	jal	copyBootMem
	nop
/*	li		$t1, 0x88c00ff8
	la		$t2, RedirectLoaderCoreInst
	li		$t3, 0x3FFFFFFF
	and		$t2, $t2, $t3
	srl		$t2, $t2, 2
	lui		$t3, 0x0800
	or		$t2, $t3, $t2
	sw		$t2, 0($t1)
*/
	/* Call to the real system bootstrap */

	lui		$t1, 0x88c0
	jalr	$ra, $t1
	nop
	lw		$ra, 0($sp)
	jr		$ra
	addiu	$sp, $sp, 4

	.end Bootstrap


	.global RedirectLoaderCoreInst
	.ent RedirectLoaderCoreInst

RedirectLoaderCoreInst:

	/* Now sceLoaderCoreTool has been already loaded. Next step,
	   redirect an instruction os sceLoaderCoreTool (after that
	   instruction, IoFileMgr should have been loaded */
	
	/* sw(j PatcherInit,0x8801882C) */ 
	li		$t1, 0x8801882C
	la		$t2, PatcherInit
	li		$t3, 0x3FFFFFFF
	and		$t2, $t2, $t3
	srl		$t2, $t2, 2
	lui		$t3, 0x0800
	or		$t2, $t3, $t2
	sw		$t2, 0($t1)
	
	/* Original instruction. "We did nothing" ;) */
	jr		$v0
	nop

	.end RedirectLoaderCoreInst


	.global PatcherInit
	.ent PatcherInit

PatcherInit:

	/* IoFileMgr is now loaded.
	 * Patch a very often called function (sceIoOpen) to do
	 * our Re-patch stuff
	*/

	/* sw(j Repatch, 0x8804f274=sceIoOpen) */
	li		$t1, 0x8804f274
	la		$t2, Repatch
	li		$t3, 0x3FFFFFFF
	and		$t2, $t2, $t3
	srl		$t2, $t2, 2
	lui		$t3, 0x0800
	or		$t2, $t3, $t2
	sw		$t2, 0($t1)
	sw		$zero, 4($t1)

	/* Mimic the original instruction at 0x8801882C */
	jalr	$ra, $v0
	nop
	
	/* Go to original instruction plus two */
	li		$t1, 0x88018830
	jr		$t1
	nop	
	
	.end PatcherInit


	.global Repatch
	.ent    Repatch
Repatch:

	/* Save $ra and parameters of sceIoOpen */
	addiu	$sp, $sp, -0x10
	sw		$ra, 0($sp)
	sw		$a0, 4($sp)
	sw		$a1, 8($sp)
	sw		$a2, 12($sp)

	/* Check the address where sceLoadExec is loaded to know
	   if we are in "game" mode or "vsh"/"updater" modes. */
	la		$a0, LOADEXEC_STR
	jal		sceKernelFindModuleByName
	nop
	beq		$v0, $zero, patch_sceLoadExec_2
	nop
	/* t0 = module->text_addr */
	lw		$t0, 108($v0)
	
	/* Ok, if we are in "vsh" or "updater" modes we will skip
	   the first patch since it's not necessary here and it would 
	   crash the psp in sleep mode */
	li		$t1, 0x88067300 
	sw		$t0, debug_var
	bne		$t0, $t1, patch_sceLoadExec_2
	nop	

	/* Repatch system bootstrap (module sceLoadExec, "game" mode) */
	/* (Redirect it again to 0x883e0000) */
	li		$t0, 0x88065658
	addiu	$t1, $zero, 0x883e
	sh		$t1, 0($t0)

/* The same for "vsh" & "updater" modes */

patch_sceLoadExec_2:
	li		$t0, 0x880BEA58
	addiu	$t1, $zero, 0x883e
	sh		$t1, 0($t0)
	
patch_LoadExecForKernel_28D0D249:

	/* Patch LoadExecForKernel_28D0D249, "the holy function of homebrew" */
	
	/*	sw(j hooked_LoadExecForKernel_28d0d249, 0x880bc274=LoadExecForKernel_28D0D249) */
/*
	li		$t0, 0x880bc274
	la		$t2, hooked_LoadExecForKernel_28D0D249
	li		$t3, 0x3FFFFFFF
	and		$t2, $t2, $t3
	srl		$t2, $t2, 2
	lui		$t3, 0x0800
	or		$t2, $t3, $t2 
	sw		$t2, 0($t0)
*/

	/* Restore $ra and parameters of sceIoOpen */
	lw		$ra, 0($sp)
	lw		$a0, 4($sp)
	lw		$a1, 8($sp)
	lw		$a2, 12($sp)
	addiu	$sp, $sp, 0x10

/* First two original instructions of sceIoOpen */
	addiu	$sp, $sp, 0xfff0
	sw		$ra, 0($sp)

/* Go to third instruction of sceIoOpen */
	li		$v0, 0x8804f282
	jr		$v0
	nop

	.end	Repatch


	.global sceIoOpen
	.ent    sceIoOpen
sceIoOpen:

	jr		$ra
	syscall	0x2103

	.end sceIoOpen


	.global sceIoWrite
	.ent    sceIoWrite
sceIoWrite:

	jr		$ra
	syscall	0x2109

	.end sceIoWrite


	.global sceIoRead
	.ent    sceIoRead
sceIoRead:

	jr		$ra
	syscall	0x210F

	.end sceIoRead


	.global sceIoLseek32
	.ent    sceIoLseek32
sceIoLseek32:

	jr		$ra
	syscall	0x210E

	.end sceIoLseek32


	.global sceIoClose
	.ent    sceIoClose
sceIoClose:

	jr		$ra
	syscall	0x2113

	.end sceIoClose


	.global sceKernelFindModuleByName
	.ent    sceKernelFindModuleByName
sceKernelFindModuleByName:

	j		0x801c36c
	nop

	.end sceKernelFindModuleByName


	.align	4
LOADEXEC_STR:
	.asciiz "sceLoadExec"
	
	.align 4
debug_var:
	.word 0

	.set reorder
