#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsyscon.h>

#include <stdio.h>
#include <string.h>

#include "reboot.h"


PSP_MODULE_INFO("Reboot150", 0x1000, 1, 0);
PSP_MODULE_SDK_VERSION(0x03060010);

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000
#define SC_OPCODE	0x0000000C
#define JR_RA		0x03e00008

#define NOP	0x00000000

#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a); 
#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a); 

int sceKernelGzipDecompress(u8 *dest, u32 destSize, const u8 *src, void *unk);

int LoadRebootex(u8 *dest, u32 destSize, const u8 *src, void *unk, void *unk2)
{
	sceKernelGzipDecompress((void *)0x88fc0000, 0x4000, rebootex+0x10, 0);

	return sceKernelGzipDecompress((void *)0x88c00000, destSize, reboot+0x10, 0);	
}

int sceKernelMemsetPatched(void *buf, int ch, int size)
{
	return sceKernelMemset((void *)0x88c00000, ch, 0x400000);
}

void ClearCaches()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}

/*void WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	sceIoWrite(fd, buf, size);
	sceIoClose(fd);
}*/

int module_start(SceSize args, void *argp)
{	
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceLoadExec");	

	u32 text_addr = *(mod+27);

	MAKE_CALL(text_addr+0x1E3C, LoadRebootex);
	MAKE_CALL(text_addr+0x1E24, sceKernelMemsetPatched);
	
	ClearCaches();

	if (sceSysconGetWlanSwitch())
	{
		sceSysregMsifResetEnable(1);
		sceSysregMsifIoDisable(1);
		sceSysregMsifClkDisable(1);
		sceSysregMsifBusClockDisable(1);
		sceSysregMsifClkSelect(1, 0);
		sceSysregMsifDelaySelect(1, 4);
		sceSysregMsifResetDisable(1);
		sceSysconCtrlWlanPower(0);
	}
	
	return 0;
}

int module_stop(void)
{
	return 0;
}

