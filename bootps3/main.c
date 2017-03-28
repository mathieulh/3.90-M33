#include <pspsdk.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <psploadexec_kernel.h>
#include <pspctrl.h>
#include <stdio.h>
#include <string.h>
#include "recovery.h"
#include "conf.h"

SEConfig config;


PSP_MODULE_INFO("SEboot", 0x1000, 1, 0);

PSP_MAIN_THREAD_ATTR(0);

#define STARTUP_PROG "ms0:/PSP/GAME/BOOT/EBOOT.PBP"

int recovery_thread(SceSize args, void *argp)
{
	SceUID mod;	

	int size = sceKernelGzipDecompress((void *)0x09F00000, 0x100000, recovery, 0);
	if (size > 0)
	{
		mod = sceKernelLoadModuleBuffer((void *)0x09F00000, size, 0, NULL);
		if (mod >= 0)
		{
			sceKernelStartModule(mod, 0, NULL, NULL, NULL);
		}
	}

	sceKernelExitDeleteThread(0);
	
	return 0;
}

int module_start(SceSize args, void *argp)
{
	SceUID th;

	th = sceKernelCreateThread("recovery_thread", recovery_thread, 0x20, 0x10000, 0, NULL);

	if (th >= 0)
	{
		sceKernelStartThread(th, args, argp);
	}

	return 0;
}
