#include <pspsdk.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspctrl.h>
#include <psploadexec.h>
#include <stdio.h>
#include <string.h>

#include "reboot.h"


PSP_MODULE_INFO("SystemControl150", 0x3007, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000
#define SC_OPCODE	0x0000000C
#define JR_RA		0x03e00008

#define NOP	0x00000000

#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) & 0x3FFFFFFF) >> 2), a); 
#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x3FFFFFFF) >> 2), a); 
#define MAKE_SYSCALL(a, n) _sw(SC_OPCODE | (n << 6), a);
#define JUMP_TARGET(x) ((x & 0x3FFFFFFF) << 2)

int (* DoReboot)(int apitype, void *a1, void *a2, void *a3, void *t0);

int reboot310 = 0;

void ClearCaches()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheInvalidateAll();
}

int sceKernelGzipDecompressPatched(u8 *dest, u32 destSize, const u8 *src, void *unknown)
{
	/*u8 *output = (u8 *)0x88fc0000;
	int i;
	
	for (i = 0; i < (sizeof(rebootex)-0x10); i++)
	{
		output[i] = rebootex[i+0x10];		
	}*/
	sceKernelGzipDecompress((void *)0x88fc0000, 0x4000, rebootex+0x10, NULL);

	if (reboot310)
	{
		return sceKernelDeflateDecompress((void *)0x88600000, destSize, reboot+0x10, unknown);
	}

	return sceKernelGzipDecompress(dest, destSize, src, unknown);
}

int DoRebootPatched(int apitype, void *a1, void *a2, void *a3, void *t0)
{
	if ((apitype & 0x200) == 0x200) /* vsh */
	{
		reboot310 = 1;
	}

	return DoReboot(apitype, a1, a2, a3, t0);
}

int sceKernelMemsetPatched(void *buf, int ch, int size)
{
	if (reboot310)
	{
		buf = (void *)0x88600000;
		size = 0x200000;

		sceKernelSetDdrMemoryProtection((void *)0x88400000, 0x00400000, 0xC);
	}

	return sceKernelMemset(buf, ch, size);
}

#define RECOVERY "flash0:/kd/recovery.prx"
#define BOOT	 "ms0:/PSP/GAME/BOOT/EBOOT.PBP"


int module_start(SceSize args, void *argp)
{
	u32 *mod, text_addr;
	
	pspSdkInstallNoDeviceCheckPatch();
	pspSdkInstallNoPlainModuleCheckPatch(); 

	mod = (u32 *)sceKernelFindModuleByName("sceModuleManager");

	if (mod)
	{
		text_addr = *(mod+27);
		/* bne t4, zero, +43 -> beq zero, zero, +43 : 
		   Force always to take the size of the data.psp instead of
		   the size of the PBP to avoid the error 0x80020001 */
		_sw(0x1000002A, text_addr+0x3F28);				
	}

	mod = (u32 *)sceKernelFindModuleByName("sceLoadExec");

	if (mod)
	{
		/* Patch reboot */
		text_addr = *(mod+27);
		DoReboot = (void *)(text_addr+0x2138);
		MAKE_CALL(text_addr+0x2090, DoRebootPatched);
		MAKE_CALL(text_addr+0x2344, sceKernelGzipDecompressPatched);
		MAKE_CALL(text_addr+0x232C, sceKernelMemsetPatched);
		_sw(0x3C0188FC, text_addr+0x2384);				
	}

	ClearCaches();

	if (sceKernelFindModuleByName("lcpatcher"))
	{
		SceCtrlData pad;
		struct SceKernelLoadExecParam param;

		sceCtrlPeekBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_RTRIGGER)
		{
			memset(&param, 0, sizeof(param));

			param.size = sizeof(param);
			param.key = "updater";

			sceKernelLoadExec(RECOVERY, &param);
		}
		else
		{
			asm("nop\n"
				"nop\n");

			sceKernelExitVSHVSH(NULL);
			
			memset(&param, 0, sizeof(param));

			param.size = sizeof(param);
			param.key = "game";
			param.args = strlen(BOOT)+1;
			param.argp = BOOT;
			
			sceKernelLoadExec(BOOT, &param);
		}
	}

	mod = (u32 *)sceKernelFindModuleByName("sceWlan_Driver");
	text_addr = *(mod+27);

	int (* SwitchCallback)(int on, void *arg);

	SwitchCallback = (void *)(text_addr+0x1B5C);
	SwitchCallback(0, 0x881b3c28);
	SwitchCallback(1, 0x881b3c28);


	
	return 0;
}

int module_stop(void)
{
	return 0;
}

