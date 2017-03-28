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

	if (kuKernelGetModel() != PSP_MODEL_SLIM_AND_LITE)
	{
		ErrorExit(5000, "This program for psp slim only.\n");
	}

	if (pspSdkLoadStartModule("partition8mgr.prx", PSP_MEMORY_PARTITION_KERNEL) < 0)
	{
		ErrorExit(5000, "Cannot load partition8mgr.prx.\n");
	}

	printf("Free memory: %d (%d KB)\n", sceKernelTotalFreeMemSize(), sceKernelTotalFreeMemSize() / 1024);
	printf("Max free block: %d (%d KB)\n\n", sceKernelMaxFreeMemSize(), sceKernelMaxFreeMemSize() / 1024);

	sceKernelDelayThread(3*1000*1000);

	for (i = 52; i > 0; i--)
	{
		printf("Attempting to allocate %d MB... ", i);
		buf = (u8 *)malloc(i*1024*1024);

		if (buf)
			break;

		printf("failed\n");
		sceKernelDelayThread(300000);
	}
	
	
	if (!buf)
	{
		printf("All tries failed.\n");
	}
	else
	{
		printf("OK\nFilling big buffer with 0xDA... ");
		memset(buf, 0xDA, i*1024*1024);
		printf(" done.\n");
	}

	printf("Press X to write file, R to skip.\n");

	while (1)
	{
		SceCtrlData pad;

		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_CROSS)
		{
			WriteFile("ms0:/memory.bin", buf, i*1024*1024);
			break;
		}

		else if (pad.Buttons & PSP_CTRL_RTRIGGER)
		{
			sceKernelDelayThread(300000);
			break;
		}

		sceKernelDelayThread(5000);
	}

	printf("Free memory: %d (%d KB)\n", Partition8TotalFreeMemsize(), Partition8TotalFreeMemsize() / 1024);
	printf("Max free block: %d (%d KB)\n\n", Partition8MaxFreeMemsize(), Partition8MaxFreeMemsize() / 1024);

	sceKernelDelayThread(3*1000*1000);

	for (i = 52; i > 0; i--)
	{
		printf("Attempting to allocate %d MB... ", i);
		buf = (u8 *)Partition8Malloc(i*1024*1024);

		if (buf)
			break;

		printf("failed\n");
		sceKernelDelayThread(300000);
	}
	
	
	if (!buf)
	{
		printf("All tries failed.\n");
	}
	else
	{
		buf = (u8 *)((u32)buf & 0x7FFFFFFF);
		printf("OK\nFilling big buffer with 0xDA... ");
		memset(buf, 0xDA, i*1024*1024);
		printf(" done.\n");
	}

	printf("Press X to write file, R to skip.\n");

	while (1)
	{
		SceCtrlData pad;

		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_CROSS)
		{
			WriteFile("ms0:/memory2.bin", buf, i*1024*1024);
			break;
		}

		else if (pad.Buttons & PSP_CTRL_RTRIGGER)
		{
			sceKernelDelayThread(300000);
			break;
		}

		sceKernelDelayThread(5000);
	}

	printf("Press X to restart in 32+20 mode, R to exit.\n");

	while (1)
	{
		SceCtrlData pad;

		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_CROSS)
			break;

		else if (pad.Buttons & PSP_CTRL_RTRIGGER)
			ErrorExit(2000, "Exiting...");

		sceKernelDelayThread(5000);
	}

	memset(&param, 0, sizeof(param));
	param.size = sizeof(param);
	param.args = strlen(argv[0])+1;
	param.argp = argv[0];
	param.key = "game";

	if (sctrlHENSetMemory(32, 20) < 0)
	{
		ErrorExit(5000, "Set Memory failed.\n");
	}

	sctrlKernelLoadExecVSHMs2(argv[0], &param);

    return 0;
}

