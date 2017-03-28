#include <pspsdk.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>

#include <stdio.h>
#include <string.h>

#include "flashemu.h"
#include "rebootex.h"


PSP_MODULE_INFO("FlashEmu", 0x3007, 1, 0);
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
#define REDIRECT_FUNCTION(a, f) _sw(J_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a);  _sw(NOP, a+4);
#define MAKE_DUMMY_FUNCTION0(a) _sw(0x03e00008, a); _sw(0x00001021, a+4);
#define MAKE_DUMMY_FUNCTION1(a) _sw(0x03e00008, a); _sw(0x24020001, a+4);

void ClearCaches()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheInvalidateAll();
}

int sceKernelGzipDecompressPatched(u8 *dest, u32 destSize, const u8 *src, void *unknown)
{
	u8 *output = (u8 *)0x88fc0000;
	int i;	
	
	for (i = 0; i < sizeof(rebootex); i++)
	{
		output[i] = rebootex[i];		
	}

	return sceKernelGzipDecompress(dest, destSize, src, unknown);
}

int (* GetMsSize)(void);


int ValidateSeekPatched(u32 *drv_str, SceOff ofs)
{
	if ((ofs & 0x1FF))
		return 0;

	SceOff max_size = GetMsSize(); // size of partition in sectors
	max_size *= 512; // size in bytes

	if (ofs >= max_size)
		return 0;

	return 1;
}

int ValidateSeekP1Patched(u32 *drv_str, SceOff ofs)
{
	if ((ofs & 0x1FF))
		return 0;

	u32 *p = (u32 *)drv_str[0x10/4];
	u32 *q = (u32 *)p[8/4];
	SceOff max_size = (SceOff)q[4/4]; // partition_start

	max_size = GetMsSize() - max_size; // size of partition in sectors
	max_size *= 512; // size in bytes

	if (ofs >= max_size)
		return 0;

	return 1;
}

int module_start(SceSize args, void *argp)
{
	u32 *mod, text_addr;
	
	pspSdkInstallNoDeviceCheckPatch();
	
	mod = (u32 *)sceKernelFindModuleByName("sceModuleManager");
	text_addr = *(mod+27);
	/* bne t4, zero, +43 -> beq zero, zero, +43 : 
		Force always to take the size of the data.psp instead of
		the size of the PBP to avoid the error 0x80020001 */
	_sw(0x1000002A, text_addr+0x3F28);				
	
	mod = (u32 *)sceKernelFindModuleByName("sceLoadExec");
	/* Patch reboot */
	text_addr = *(mod+27);		
	MAKE_CALL(text_addr+0x2344, sceKernelGzipDecompressPatched);
	_sw(0x3C0188FC, text_addr+0x2384);	
	
	mod = (u32 *)sceKernelFindModuleByName("sceMSstor_Driver");
	text_addr = *(mod+27);
	REDIRECT_FUNCTION(text_addr+0x5138, ValidateSeekPatched);
	REDIRECT_FUNCTION(text_addr+0x51BC, ValidateSeekP1Patched);
	GetMsSize = (void *)(text_addr+0x288);
	
	mod = (u32 *)sceKernelFindModuleByName("sceDisplay_Service");
	text_addr = *(mod+27);
	_sw(0x03E00008, text_addr+0x0000);
	_sw(0x00001021, text_addr+0x0004);

	/*pspDebugSioInit();
	pspDebugSioSetBaud(115200);
	pspDebugSioInstallKprintf();*/



	InstallFlashEmu();

	ClearCaches();

	
	
	return 0;
}

int module_stop(void)
{
	return 0;
}

