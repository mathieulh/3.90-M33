#include <pspsdk.h>
#include <pspkernel.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

PSP_MODULE_INFO("MyTest", 0x1000, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

#define printf	pspDebugScreenPrintf

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
		return fd;

	int written = sceIoWrite(fd, buf, size);
	sceIoClose(fd);

	return written;
}


int main()
{
	pspDebugScreenInit();
	pspDebugScreenClear();
	
	pspSdkInstallNoDeviceCheckPatch();
	pspSdkInstallNoPlainModuleCheckPatch();

	SceUID mod = pspSdkLoadStartModule("iplreboot.prx", PSP_MEMORY_PARTITION_KERNEL);

	if (mod < 0)
		ErrorExit(6000, "Error %08X loading module.\n", mod);

	ErrorExit(2000, "Done. Rebooting with ipl...\n");
	
	return 0;
}

