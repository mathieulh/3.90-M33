#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <psploadexec_kernel.h>
#include <kubridge.h>
#include <systemctrl.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

PSP_MODULE_INFO("RamTest", 0x0200, 1, 1);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

#define printf    pspDebugScreenPrintf

void ErrorExit(int milisecs, char *fmt, ...)
{
	va_list list;
	char msg[256];	

	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);

	printf(msg);
	
	sceKernelDelayThread(milisecs*1000);
	sceKernelExitGame();
}

int WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	
	if (fd < 0)
	{
		return fd;
	}

	int written = sceIoWrite(fd, buf, size);

	sceIoClose(fd);
	return written;
}


int main(int argc, char *argv[])
{
    u8 *buf = NULL;
	int i;
	struct SceKernelLoadExecVSHParam param;
	
	pspDebugScreenInit();

	if (sceKernelDevkitVersion() < 0x03070110)
	{
		ErrorExit(5000, "This program requires 3.71 M33 or higher.\n");
	}

	printf("ModuleMgrForUser kernel module = 0x%08X\n", pspSdkLoadStartModule("flash0:/kd/usbstor.prx", PSP_MEMORY_PARTITION_KERNEL));
	printf("ModuleMgrForUser user module = 0x%08X\n", pspSdkLoadStartModule("flash0:/kd/libmp3.prx", PSP_MEMORY_PARTITION_USER));
	printf("ModuleMgrForKernel 2.71 sdk kernel module = 0x%08X\n", kuKernelLoadModule("kmodule271.prx", 0, NULL));
	printf("ModuleMgrForKernel 2.71 sdk user module = 0x%08X\n", kuKernelLoadModule("umodule271.prx", 0, NULL));
	printf("ModuleMgrForKernel 2.00 sdk kernel module = 0x%08X\n", kuKernelLoadModule("kmodule200.prx", 0, NULL));
	printf("ModuleMgrForKernel 2.00 sdk user module = 0x%08X\n", kuKernelLoadModule("umodule200.prx", 0, NULL));
	printf("ModuleMgrForKernel 1.50 sdk kernel module = 0x%08X\n", kuKernelLoadModule("kmodule150.prx", 0, NULL));
	printf("ModuleMgrForKernel 1.50 sdk user module = 0x%08X\n", kuKernelLoadModule("umodule150.prx", 0, NULL));
	printf("ModuleMgrForUser  1.50 sdk user module = 0x%08X\n", sceKernelLoadModule("umodule150.prx", 0, NULL));
	printf("ModuleMgrForKernel 3.50 sdk user module = 0x%08X\n", kuKernelLoadModule("umodule350.prx", 0, NULL));


	ErrorExit(29000, "Exiting...");

    return 0;
}

