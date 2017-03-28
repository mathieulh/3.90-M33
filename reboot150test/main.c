#include <pspsdk.h>
#include <pspkernel.h>

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
	//u8 *output = (u8 *)0x88fc0000;
	//int i;

	/*for (i = 0; i < (sizeof(rebootex)-0x10); i++)
	{
		output[i] = rebootex[i+0x10];		
	}*/
	sceKernelGzipDecompress((void *)0x88fc0000, 0x4000, rebootex+0x10, 0);

	return sceKernelGzipDecompress((void *)0x88c00000, destSize, reboot+0x10, 0);	
}

int sceKernelMemsetPatched(void *buf, int ch, int size)
{
	return sceKernelMemset((void *)0x88c00000, ch, 0x400000);
}


void ClearCaches()
{
	sceKernelDcacheWBinvAll();
	sceKernelIcacheClearAll();
}

int module_start(SceSize args, void *argp)
{	
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceLoadExec");

	if (mod)
	{
		u32 text_addr = *(mod+27);

		MAKE_CALL(text_addr+0x1958, LoadRebootex);
		MAKE_CALL(text_addr+0x1940, sceKernelMemsetPatched);
		
		ClearCaches();

		int (* x)(void);

		x = sctrlHENFindFunction("sceWlan_Driver", "sceWlanDrv_driver", 0xC9A8CAB7);
		x();
		
		x = sctrlHENFindFunction("sceWlan_Driver", "sceWlanDrv_driver", 0xEC9232F0);
		x();

		x = sctrlHENFindFunction("scePower_Service", "scePower_driver", 0x23BB0A60);
		x();

	}	

	return 0;
}

int module_stop(void)
{
	return 0;
}

