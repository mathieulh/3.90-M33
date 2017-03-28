#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <psputilsforkernel.h>

#include <stdio.h>
#include <string.h>

PSP_MODULE_INFO("simpleplugin", 0x1007, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

int module_start(SceSize args, void *argp)
{
	SceUID fd = sceIoOpen("ms0:/soyunplugin.txt", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	if (fd >= 0)
	{
		sceIoWrite(fd, (void *)"hola", 4);
		sceIoClose(fd);
	}	

	return 0;
}

int module_stop(void)
{
	return 0;
}

