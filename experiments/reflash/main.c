#include <pspsdk.h>
#include <pspkernel.h>
#include <stdio.h>
#include <string.h>


PSP_MODULE_INFO("NandDumperMain", 0x0800, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VSH);


#define printf pspDebugScreenPrintf

int WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (fd < 0)
		return -1;

	int written = sceIoWrite(fd, buf, size);

	sceIoClose(fd);
	return written;
}

u8 buf[500*1024];

int ReadFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0777);

	if (fd < 0)
		return -1;

	int read = sceIoRead(fd, buf, size);

	sceIoClose(fd);
	return read;
}

int main() 
{
	/*SceUID mod = pspSdkLoadStartModule("reflasher.prx", PSP_MEMORY_PARTITION_KERNEL);

	if (mod < 0)
	{
		pspDebugScreenInit();
		printf("Error %08X\n", mod);
	}*/

	pspDebugScreenInit();

	printf("Hola.\n");

	int res = sceIoUnassign("flash0:");
	printf("res = 0x%08X\n", res);
	sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDWR, NULL, 0);

	printf("res = 0x%08X\n", res);

	int size = ReadFile("gpio.prx", buf, 500*1024);

	printf("read = %08X\n", size);

	printf("remove = 0x%08X\n", sceIoRemove("flash0:/kD/gpio.prx"));

	if (size > 0)
	{
		size = WriteFile("flash0:/kD/gpio.prx", buf, size);

		printf("written = %08X\n", size);
	}

	/*size = ReadFile("pspbtcnf_pops.txt", buf, 500*1024);

	printf("read = %08X\n", size);

	printf("remove = 0x%08X\n", sceIoRemove("flash0:/kn/pspbtcnf_pops.txt"));

	if (size > 0)
	{
		size = WriteFile("flash0:/kn/pspbtcnf_pops.txt", buf, size);

		printf("written = %08X\n", size);
	}*/


	

	sceKernelDelayThread(5*1000*1000);
	sceKernelExitGame();

	return 0;
}
