#include <pspsdk.h>
#include <pspkernel.h>
#include <pspinit.h>
#include <psploadexec_kernel.h>
#include <psputilsforkernel.h>
#include <pspreg.h>
#include <pspmediaman.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "sysmodpatches.h"
#include "main.h"
#include "virtualpbpmgr.h"
#include "systemctrl_se.h"

void PatchUmdMan(char *buf)
{
	int intr = sceKernelCpuSuspendIntr();
	u32 text_addr = (u32)buf+0xA0;

	if ((sceKernelInitApitype() & 0x200) != 0x200)
	{
		// Replace call to sceKernelBootFrom with return PSP_BOOT_DISC
		_sw(0x24020020, text_addr+0x1400);
	}

	sceKernelCpuResumeIntr(intr);
	ClearCaches();
}

u32 vshmain_args[0x0400/4];

int (* sceKernelExitVSHKernel)(struct SceKernelLoadExecVSHParam *param);


void RebootVSHWithError(u32 error)
{
	struct SceKernelLoadExecVSHParam param;	
	
	memset(&param, 0, sizeof(param));
	memset(vshmain_args, 0, sizeof(vshmain_args));

	vshmain_args[0/4] = 0x0400;
	vshmain_args[4/4] = 0x20;
	vshmain_args[0x14/4] = error;

	param.size = sizeof(param);
	param.args = 0x0400;
	param.argp = vshmain_args;
	param.vshmain_args_size = 0x0400;
	param.vshmain_args = vshmain_args;
	param.configfile = "/kd/pspbtcnf.txt";

	sceKernelExitVSHKernel(&param);
}

void PatchLoadExec(u32 text_addr)
{
	if ((sceKernelInitApitype() & 0x200) == 0x200)
	{
		sceKernelExitVSHKernel = (void *)(text_addr+0x19C4);		
	}	
}

void PatchInitLoadExecAndMediaSync(char *buf)
{
	int intr = sceKernelCpuSuspendIntr();	
	
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceLoadExec");
	if (mod)
	{
		PatchLoadExec(*(mod+27));
	}	

	sceKernelCpuResumeIntr(intr);
	
	ClearCaches();			
}

//////////////////////////////////////////////////////////////

int (* sceRegSetKeyValueReal)(REGHANDLE hd, const char *name, void *buf, SceSize size);

int sceRegSetKeyValuePatched(REGHANDLE hd, const char *name, void *buf, SceSize size)
{
	int k1 = pspSdkSetK1(0);

	if (strcmp(name, "language") == 0)
	{
		int lang = *(int *)buf;

		if (lang == 0x09 /* korean */ ||
		    lang == 0x0A /* chinese */ ||
			lang == 0x0B) /* the other chinese */
		{
			RebootVSHWithError(0x98765432);
			// We shouldn't get here, but if that, then let's loop forever
			while (1);
		}
	}
	
	pspSdkSetK1(k1);

	return sceRegSetKeyValueReal(hd, name, buf, size);
}

void PatchVshMain(char *buf)
{
	int intr = sceKernelCpuSuspendIntr();

	u32 text_addr = (u32)buf+0xA0;

	_sw(NOP, (u32)(text_addr+0xdd7c));
	_sw(NOP, (u32)(text_addr+0xdd84)); 

	sceRegSetKeyValueReal = (void *)FindProc("sceRegistry_Service", "sceReg", 0x17768E14);
					
	if (sceRegSetKeyValueReal)
	{
		PatchSyscall((u32)sceRegSetKeyValueReal, sceRegSetKeyValuePatched);
	}

	sceKernelCpuResumeIntr(intr);
	ClearCaches();	
}

////////////////////////////////////////////////////////////////

/* Note: the compiler has to be configured in makefile to make 
   sizeof(wchar_t) = 2. */
wchar_t verinfo[] = L"2.71 DH-SE";

void PatchSysconfPlugin(char *buf)
{
	u32 addrlow;
	int intr = sceKernelCpuSuspendIntr();
	
	u32 text_addr = (u32)buf+0xA0;

	memcpy((void *)(text_addr+0x18218), verinfo, sizeof(verinfo));
	addrlow = _lh(text_addr+0xBDA8) & 0xFFFF;
	addrlow -= 0x36F8; /* address for ver info */				
				
	// addiu a1, s4, addrlow
	_sw(0x26850000 | addrlow, text_addr+0xBF5C);

	sceKernelCpuResumeIntr(intr);
	ClearCaches();
}

//////////////////////////////////////////////////

void OnImposeLoad()
{
	
}




