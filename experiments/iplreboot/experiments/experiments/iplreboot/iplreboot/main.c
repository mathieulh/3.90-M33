#include <pspsdk.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspctrl.h>
#include <psploadexec.h>
#include <stdio.h>
#include <string.h>

#include "ipl.h"


PSP_MODULE_INFO("IplReboot", 0x3007, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000
#define SC_OPCODE	0x0000000C
#define JR_RA		0x03e00008
#define NOP	0x00000000

#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a); 
#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a);  

int (* DoReboot)(int apitype, void *a1, void *a2, void *a3, void *t0);


/* Just to override SE/OE patch and make it compatible with them */
int DoRebootPatched(int apitype, void *a1, void *a2, void *a3, void *t0)
{
	return DoReboot(apitype, a1, a2, a3, t0);
}

void ClearCaches()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheInvalidateAll();
}

int sceKernelGzipDecompressPatched(u8 *dest, u32 destSize, const u8 *src, void *unknown)
{
	u8 *output = (u8 *)0x040f0000;

	return sceKernelGzipDecompress(output, 500*1024, ipl, unknown);
}

int module_start(SceSize args, void *argp)
{
	u32 *mod, text_addr;
	
	mod = (u32 *)sceKernelFindModuleByName("sceLoadExec");

	if (mod)
	{
		/* Patch reboot */
		text_addr = *(mod+27);
		DoReboot = (void *)(text_addr+0x2138);
		MAKE_CALL(text_addr+0x2090, DoRebootPatched);
		MAKE_CALL(text_addr+0x2344, sceKernelGzipDecompressPatched);
		_sw(0x3C01040f, text_addr+0x2384);				
	}

	ClearCaches();

	return 0;
}

int module_stop(void)
{
	return 0;
}

