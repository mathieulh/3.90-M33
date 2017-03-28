#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspsuspend.h>
#include <psppower.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>

#include <kubridge.h>
#include <systemctrl.h>
#include <systemctrl_se.h>

#include "systemctrl_01g.h"
#include "systemctrl_02g.h"
#include "vshctrl_01g.h"
#include "vshctrl_02g.h"
#include "popcorn.h"
#include "galaxy.h"


PSP_MODULE_INFO("m33updater", 0x0800, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VSH);

#define printf    pspDebugScreenPrintf

#define PSAR_SIZE_150	10149440

#define N_FILES	4

typedef struct
{
	char *path;
	u8 *buf;
	int size;
} M33File;

M33File m33files_01g[N_FILES] =
{
	{ "flash0:/kd/systemctrl.prx", systemctrl_01g, sizeof(systemctrl_01g) },
	{ "flash0:/kd/vshctrl.prx", vshctrl_01g, sizeof(vshctrl_01g) },
	{ "flash0:/kd/popcorn.prx", popcorn, sizeof(popcorn) },
	{ "flash0:/kd/galaxy.prx", galaxy, sizeof(galaxy) },
};

M33File m33files_02g[N_FILES] =
{
	{ "flash0:/kd/systemctrl_02g.prx", systemctrl_02g, sizeof(systemctrl_02g) },
	{ "flash0:/kd/vshctrl_02g.prx", vshctrl_02g, sizeof(vshctrl_02g) },
	{ "flash0:/kd/popcorn.prx", popcorn, sizeof(popcorn) },
	{ "flash0:/kd/galaxy.prx", galaxy, sizeof(galaxy) },
};

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
	sceKernelSleepThread();
}

////////////////////////////////////////////////////////////////////
// File helpers

int ReadFile(char *file, int seek, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0);
	if (fd < 0)
		return fd;

	if (seek > 0)
	{
		if (sceIoLseek(fd, seek, PSP_SEEK_SET) != seek)
		{
			sceIoClose(fd);
			return -1;
		}
	}

	int read = sceIoRead(fd, buf, size);
	
	sceIoClose(fd);
	return read;
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

void WriteConfig()
{
	SEConfig config;

	memset(&config, 0, sizeof(config));
	sctrlSEGetConfig(&config);

	WriteFile("flash1:/config.se", &config, sizeof(config));
}

int main(void)
{
    int i;
	M33File *files;
	
	pspDebugScreenInit();

	if (sceKernelDevkitVersion() != 0x03080010)
	{
		ErrorExit(5000, "This program can only be executed on 3.80 M33.\n");
	}

	if (sctrlSEGetVersion() >= 0x1023)
	{
		ErrorExit(5000, "This update or a higher one was already applied.\n");
	}

	printf("3.80 M33-4 installer.\n");

	printf("Changes:\n\n");

	if (sctrlSEGetVersion() == 0x1020)
	{
		printf("- Fixed scePowerGetClockFrequency/scePowerGetClockFrequencyInt in\n  nids resolver.\n"
		       "- PSX eboot.pbp with 80x80 icons will not have Dracula icon anymore.\n");
	}
	
	if (sctrlSEGetVersion() == 0x1021)
	{
		printf("- Fixed issue that plugins check code caused in PSN NP9660 original\n"
			   "  games (fixes 0x80010013 error).\n"
		       "- Added a couple of libs to the nids resolver.\n"
		       "- Added some internal changes required by 3.80 popsloader plugin.\n");
	}

	printf("- Reverted back to previous galaxy.prx,as the one gave problems with slow memory sticks.\n\n");

	
	printf("Press X to install, R to exit.\n\n");

	

	while (1)
	{
		SceCtrlData pad;
		
		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_CROSS)
			break;

		else if (pad.Buttons & PSP_CTRL_RTRIGGER)
			ErrorExit(5000, "Cancelled by user.\n");
	}

	if (sceIoUnassign("flash0:") < 0)
	{
		ErrorExit(5000, "Error in unassign.\n");
	}

	if (sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDWR, NULL, 0) < 0)
	{
		ErrorExit(5000, "Error in assign.\n");
	}

	if (kuKernelGetModel() == PSP_MODEL_STANDARD)
		files = m33files_01g;
	else
		files = m33files_02g;

	for (i = 0; i < N_FILES; i++)
	{
		printf("Flashing %s (%d)... ", files[i].path, files[i].size);

		if (WriteFile(files[i].path, files[i].buf, files[i].size) != files[i].size)
		{
			ErrorExit(5000, "Error!\n");
		}

		printf("OK\n");
	}

	WriteConfig();
	ErrorExit(6000, "\nUpdate complete. Restarting in 6 seconds...\n");

    return 0;
}

