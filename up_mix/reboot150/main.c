#include <pspsdk.h>
#include <pspkernel.h>
#include <pspnand_driver.h>

#include <stdio.h>
#include <string.h>

#include "reboot.h"


PSP_MODULE_INFO("Reboot150", 0x1000, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000
#define SC_OPCODE	0x0000000C
#define JR_RA		0x03e00008

#define NOP	0x00000000

#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) & 0x3FFFFFFF) >> 2), a); 
#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x3FFFFFFF) >> 2), a); 

int sceKernelDcacheWBinvAll();

int sceKernelGzipDecompress(u8 *dest, u32 destSize, const u8 *src, void *unk);

int LoadRebootex(u8 *dest, u32 destSize, const u8 *src, void *unk, void *unk2)
{
	u8 *output = (u8 *)0x88fc0000;
	int i;

	for (i = 0; i < (sizeof(rebootex)-0x10); i++)
	{
		output[i] = rebootex[i+0x10];		
	}

	return sceKernelGzipDecompress(dest, destSize, reboot+0x10, 0);	
}


void ClearCaches()
{
	sceKernelDcacheWBinvAll();
	sceKernelIcacheClearAll();
}

#define NAND_STATUS (*((volatile unsigned *)0xBD101004))
#define NAND_COMMAND (*((volatile unsigned *)0xBD101008))
#define NAND_ADDRESS (*((volatile unsigned *)0xBD10100C))
#define NAND_READDATA (*((volatile unsigned *)0xBD101300))
#define NAND_ENDTRANS (*((volatile unsigned *)0xBD101014))

u32 commands[20] =
{
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 
	0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01,
	0x00, 0x00, 0x01, 0xFF 
};

void SetActiveNand(u32 unit)
{
	// unit 0 -> UP, unit 1 -> internal nand
	int i;
	
	commands[19] = unit;

	for (i = 0; i < 20; i++)
	{
		NAND_COMMAND = commands[i];
		NAND_ADDRESS = 0;
		NAND_ENDTRANS = 1;
	}
}

int module_start(SceSize args, void *argp)
{	
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceLoadExec");

	if (mod)
	{
		u32 text_addr = *(mod+27);

		_sw(0x3C0188fc, text_addr+0x16DC);
		MAKE_CALL(text_addr+0x16A0, LoadRebootex);
		ClearCaches();
	}	

	SetActiveNand(1);

	return 0;
}

int module_stop(void)
{
	return 0;
}

