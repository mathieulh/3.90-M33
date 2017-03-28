#include <pspsdk.h>
#include <pspkernel.h>
#include <psploadexec.h>
#include <psputils.h>
#include <pspctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>

#include "systemctrl150.h"
#include "recovery.h"
#include "systemctrl.h"
#include "vshctrl.h"
#include "popcorn.h"
#include "march33.h"
#include "conf.h"


PSP_MODULE_INFO("Galaxy_Update", 0x1000, 1, 0);

PSP_MAIN_THREAD_ATTR(0);

#define printf pspDebugScreenPrintf


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
		return -1;
	}

	int written = sceIoWrite(fd, buf, size);
	
	if (sceIoClose(fd) < 0)
		return -1;

	return written;
}

u8 buf[256*1024] __attribute__((aligned(64)));


int ReadFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0);
	
	if (fd < 0)
		return fd;

	int read = sceIoRead(fd, buf, size);
	sceIoClose(fd);

	return read;
}

int CheckFirmwareAndCopyReboot()
{
	if (sceKernelDevkitVersion() != 0x01050001)
		return -1;

	if (!sceKernelFindModuleByName("SystemControl150"))
		return -1;

	int size = ReadFile("flash0:/kd/rtc.prx", buf, sizeof(buf));
	
	if (size != 46870)
		return -1;

	memcpy(systemctrl150+0x1320, buf+0x11A0, 0x96AF);
	
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();

	return 0;
}

void WriteConfig()
{
	SEConfig config;

	memset(&config, 0, sizeof(config));
	SE_GetConfig(&config);
	
	config.startupprog = 0;	
	
	SE_SetConfig(&config);
}

int main(int argc, char *argv[])
{
	pspDebugScreenInit();

	if (CheckFirmwareAndCopyReboot() < 0)
	{
		ErrorExit(5000, "This update doesn't apply to this firmware or it has already been applied.\n");
	}	
	
	printf("3.52 M33-2 Update\n");
	printf("Press X to do the update.\n");

	while (1)
	{
		SceCtrlData pad;

		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_CROSS)
			break;
	}

	if (sceIoUnassign("flash0:") < 0)
	{
		ErrorExit(5000, "Can't unassign flash0.\n");		
	}

	if (sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDWR, NULL, 0) < 0)
	{
		ErrorExit(5000, "Can't assign flash0.\n");
	}


	if (WriteFile("flash0:/kd/rtc.prx", systemctrl150, sizeof(systemctrl150)) != sizeof(systemctrl150))
	{
		ErrorExit(5000, "Cannot write rtc.prx\n");
	}

	if (WriteFile("flash0:/kd/recovery.prx", recovery, sizeof(recovery)) != sizeof(recovery))
	{
		ErrorExit(5000, "Cannot write recovery.prx\n");
	}

	if (WriteFile("flash0:/kn/systemctrl.prx", systemctrl, sizeof(systemctrl)) != sizeof(systemctrl))
	{
		ErrorExit(5000, "Cannot write systemctrl.prx\n");
	}

	if (WriteFile("flash0:/kn/vshctrl.prx", vshctrl, sizeof(vshctrl)) != sizeof(vshctrl))
	{
		ErrorExit(5000, "Cannot write vshctrl.prx\n");
	}

	if (WriteFile("flash0:/kn/popcorn.prx", popcorn, sizeof(popcorn)) != sizeof(popcorn))
	{
		ErrorExit(5000, "Cannot write popcorn.prx\n");
	}

	if (WriteFile("flash0:/kn/march33.prx", march33, sizeof(march33)) != sizeof(march33))
	{
		ErrorExit(5000, "Cannot write march33.prx\n");
	}

	WriteConfig();

	ErrorExit(5000, "Update succesfull.\n");

	return 0;
}
