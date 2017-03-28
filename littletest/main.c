#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <psputilsforkernel.h>
#include <pspctrl.h>
#include <pspmoduleexport.h>
#include <psprtc.h>


#include <stdio.h>
#include <string.h>


PSP_MODULE_INFO("pspPopsLoader", 0x1007, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

void WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	sceIoWrite(fd, buf, size);
	sceIoClose(fd);
}

int module_start(SceSize args, void *argp)
{
	if (sceKernelFindModuleByName("sceUmdCache_driver"))
		WriteFile("ms0:/umdcache.bin", "si", 2);
	else
		WriteFile("ms0:/umdcache.bin", "no", 2);

	WriteFile("ms0:/newmem.bin", 0x8A000000, 32*1024*1024);

	return 0;


}

int module_stop(void)
{
	return 0;
}

