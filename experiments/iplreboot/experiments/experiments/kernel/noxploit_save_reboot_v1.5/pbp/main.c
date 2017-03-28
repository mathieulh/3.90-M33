#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>


PSP_MODULE_INFO("NOKXPLOIT", 0x1000, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

#define printf pspDebugScreenPrintf

int installPatch()
{
	SceUID fd;

	fd = sceIoOpen("patch.bin", PSP_O_RDONLY, 0777);

	if (fd < 0)
		return 0;

	sceIoRead(fd, (void *)0x883e0000, 10*1024);
	sceIoClose(fd);

	// Redirect system bootstrap (sceLoadExec module, "game" mode)
	// c0 88 01 3c lui $at, 0x88c0 -> 3e 88 01 3c lui $at, 0x883e
	// 09 f8 20 00 jalr $ra, $at
	_sh(0x883e, 0x88069684);
	
	// The same here, for "vsh" and "updater" modes (sceLoadExec
	// module loads to a different location in those modes)
	_sh(0x883e, 0x880bce84);

	//sceKernelDcacheWritebackAll();

	return 1;
}

int main()
{
	pspDebugScreenInit();
	pspDebugScreenClear();

	if (installPatch())
		printf("Patch done.\n");
	else
		printf("Patch failed.\n");

	printf("\nReturning to VSH...");
	sceKernelDelayThread(700000);
	sceKernelExitGame();

	return 0;
}

