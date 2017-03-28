#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <kubridge.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

PSP_MODULE_INFO("RamTest", 0, 1, 1);
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


int main(void)
{
    u8 *buf; 
	
	pspDebugScreenInit();

	if (sceKernelDevkitVersion() < 0x03070110)
	{
		ErrorExit(5000, "This program requires 3.71 M33 or higher.\n");
	}

	if (kuKernelGetModel() != PSP_MODEL_SLIM_AND_LITE)
	{
		ErrorExit(5000, "This program for psp slim only.\n");
	}

	
	buf = (u8 *)malloc(50*1024*1024);
	if (!buf)
		ErrorExit(5000, "Error in malloc.\n");

	printf("Filling big buffer with 0xDA... ");
	memset(buf, 0xDA, 50*1024*1024);
	printf(" done.\n");

	printf("Press X to write file.\n");

	while (1)
	{
		SceCtrlData pad;

		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_CROSS)
			break;

		sceKernelDelayThread(5000);
	}

	printf("Writing file... ");
	WriteFile("ms0:/memory.bin", buf, 50*1024*1024);
	printf(" done.\n");

	ErrorExit(5000, "Finished.\n");


    return 0;
}

