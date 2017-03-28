#include <pspsdk.h>
#include <stdio.h>
#include <string.h>

#include "psp_uart.h"
#include "fat.h"
#include "syscon.h"

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000

#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a); 
#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a); 

int (* Reboot)(void *, void *, void*, void *) = (void *)0x88c00000;
int (* Kprintf)(const char *format, ...) = (void *)0x88C01BB0;
int (* Dcache)(void) = (void *)0x88C03F50;
int (* Icache)(void) = (void *)0x88C03B80;

void ClearCaches()
{
	Dcache();
	Icache();
}

int SysMemStart(SceSize args, void *argp, int (* module_start)(SceSize, void *))
{
	Kprintf("Starting sysmem...\n");
	return module_start(4, argp);
}

int LoadCoreStart(SceSize args, void *argp, int (* module_start)(SceSize, void *))
{
	u32 text_addr = ((u32)module_start) - 0x0AB8;

	// NoPlainModuleCheckPatch (mfc0 $t5, $22 -> li $t5, 1)
	_sw(0x340D0001, text_addr+0x2FE0);
	ClearCaches();
	
	Kprintf("Starting loadcore...\n");
	return module_start(8, argp);
}

char path[260];

char *strcpy(char *dest, const char *src)
{
	const char *p;
	char *q; 
 
	for(p = src, q = dest; *p; p++, q++)
		*q = *p;
 
	*q = 0;
 
	return dest;
}

char *strcat(char *dest, const char *src)
{
	size_t dest_len = strlen(dest);
	size_t i;

	for (i = 0 ; src[i]; i++)
		dest[dest_len + i] = src[i];
	
	dest[dest_len + i] = 0;

	return dest;
}

int sceBootLfatOpen(char *file)
{
	strcpy(path, "/TM/DC5");
	strcat(path, file);
	
	return MsFatOpen(path);
}

int entry(void *a0, void *a1, void *a2, void *a3)
{	
	// jal sysmem::module_start -> jal SysMemStart
	// li  $a0, 4 -> mov $a2, $s1  (a0 = 4 -> a2 = module_start)
	MAKE_CALL(0x88c01014, SysMemStart);
	_sw(0x02203021, 0x88c01018);
	
	// mov $a0, $s5 -> mov $a2, $v0 (a0 = 8 -> a2 = module_start)
	// mov $a1, $sp
	// jr  loadcore::module_start -> j LoadCoreStart 
	// mov $sp, $t3
	_sw(0x00403021, 0x88c00ff0);
	MAKE_JUMP(0x88c00ff8, LoadCoreStart);

	/* Kprintf stuff */
	// Change default putc handler
	u32 addr = (u32)uart_dbg_putc;
	_sw(addr, 0x88c16d54);
	_sh(addr >> 16, 0x88C0023C);
	_sw(0x34840000 | (addr & 0xFFFF), 0x88c00244);
	// Patch removeByDebugSection, make it return 1	
	_sw(0x03e00008, 0x88C01D20);
	_sw(0x24020001, 0x88C01D24);

	/* File open redirection */
	// Redirect sceBootLfatfsMount to MsFatMount
	MAKE_CALL(0x88c00074, MsFatMount);
	// Redirect sceBootLfatOpen to MsFatOpen
	MAKE_CALL(0x88C00084, sceBootLfatOpen);
	// Redirect sceBootLfatRead to MsFatRead
	MAKE_CALL(0x88C000B4, MsFatRead);
	// Redirect sceBootLfatClose to MsFatClose
	MAKE_CALL(0x88C000E0, MsFatClose);

	ClearCaches();		

	// GPIO enable ?
	REG32(0xbc100058) |= 0x02;

	// GPIO enable
	REG32(0xbc10007c) |= 0xc8;
	asm("sync"::);
	
	pspSyscon_init();
	pspSysconCrlMsPower(1);
	pspSysconCrlHpPower(1);		

	return Reboot(a0, a1, a2, a3);	
}

int main(){ return 0; }

