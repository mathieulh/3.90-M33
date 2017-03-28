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
#include <systemctrl.h>
#include <systemctrl_se.h>
#include <kubridge.h>
#include <pspipl_update.h>

#include "u235.h"
#include "pupd.h"
#include "downloader.h"
#include "ipl_update.h"
#include "ipl_02g.h"

PSP_MODULE_INFO("plutonium_updater", 0x0800, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VSH);

#define printf    pspDebugScreenPrintf

u8 buffer[1*1024*1024] __attribute__((aligned(64)));

typedef struct
{
	char *path;
	u8 *buf;
	int size;
} M33File;

#define NEW_FILES	4

M33File new_01g[NEW_FILES] =
{
	{ "flash0:/kd/systemctrl.prx", u235+0x5260, 28458 },
	{ "flash0:/kd/vshctrl.prx", u235+0x13420, 12335 },
	{ "flash0:/kd/march33.prx", u235+0x25EA0, 6801 },
	{ "flash0:/vsh/module/recovery.prx", u235+0x1C320, 22643 },
};

M33File new_02g[NEW_FILES] =
{
	{ "flash0:/kd/systemctrl_02g.prx", u235+0xC1A0, 29285 },
	{ "flash0:/kd/vshctrl_02g.prx", u235+0x16460, 12317 },
	{ "flash0:/kd/march33.prx", u235+0x25EA0, 6801 },
	{ "flash0:/vsh/module/recovery.prx", u235+0x1C320, 22643 },
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

SceUID LoadStartModule(void *buf, int size, char *param)
{
	SceUID mod = vshKernelLoadModuleBufferVSH(size, buf, 0, NULL);

	if (mod < 0)
		return mod;

	return sceKernelStartModule(mod, (param) ? strlen(param)+1 : 0, param, NULL, NULL);

}

void UpdateIpl()
{
	printf("Writing ipl... ");
	
	int	size = pspIplUpdateGetIpl(buffer);
	if (size <= 0)
	{
		ErrorExit(5000, "Cannot read nand ipl (0x%08X).\n", size);
	}

	memcpy(buffer, ipl_02g, 16384);
	sceKernelDcacheWritebackAll();

	if (pspIplUpdateClearIpl() < 0)
	{
		ErrorExit(5000, "Error in clear IPL.\n");
	}

	if (pspIplUpdateSetIpl(buffer, 167936) < 0)
	{
		ErrorExit(5000, "Error writing double IPL.\n");
	}

	printf("OK\n");
}

int main()
{
	int dl = 0;
	SceIoStat stat;
	int res;
	
	pspDebugScreenInit();
	pspDebugScreenClear();

	if (sctrlSEGetVersion() > 0x1031)
	{
		ErrorExit(5000, "This update or a higher one was already applied.\n");
	}

	sceIoChdir("ms0:/PSP/GAME/UPDATE");

	if (sceKernelDevkitVersion() == 0x03090010)
	{
		M33File *files;
		
		printf("3.90 M33-3 installer.\n\n");

		

		printf("Changes:\n\n");

		if (sctrlSEGetVersion() == 0x1030)
		{
		   printf("- Improved plugins loading code to fix problems with problematic\n"
		   "  cards.\n"
		   "- Added the rest of regions to the recovery fake region option.\n");
		
			if (kuKernelGetModel() == PSP_MODEL_SLIM_AND_LITE)
			{
				printf("- (PSP SLIM): Do some patches on NAND IPL so Booster multiiplloader"
				   "  and TM iplloader can boot from nand on 3.90 M33-2 and higher.\n");
				files = new_02g;
			}
		}

		printf(" - Improved the compatibility of March33 NO-UMD driver.\n");

		if (kuKernelGetModel() == PSP_MODEL_SLIM_AND_LITE)
		{
			files = new_02g;
		}
		else
		{
			files = new_01g;
		}

		printf("\nPress X to install, R to exit.\n\n");	

		while (1)
		{
			SceCtrlData pad;
		
			sceCtrlReadBufferPositive(&pad, 1);

			if (pad.Buttons & PSP_CTRL_CROSS)
				break;

			else if (pad.Buttons & PSP_CTRL_RTRIGGER)
				ErrorExit(5000, "Cancelled by user.\n");

			sceKernelDelayThread(5000);
		}

		if (kuKernelGetModel() == PSP_MODEL_SLIM_AND_LITE && sctrlSEGetVersion() == 0x1030)
		{
			res = LoadStartModule(ipl_update, sizeof(ipl_update), NULL);
			if (res < 0)
			{
				ErrorExit(5000, "Error 0x%08X loading ipl_update.\n", res);
			}
			
			UpdateIpl();
		}

		if (sceIoUnassign("flash0:") < 0)
		{
			ErrorExit(5000, "Error in unassign.\n");
		}

		if (sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDWR, NULL, 0) < 0)
		{
			ErrorExit(5000, "Error in assign.\n");
		}

		int i;

		for (i = 0; i < NEW_FILES; i++)
		{
			pspDebugScreenPrintf("Flashing %s (%d)... ", files[i].path, files[i].size);

			if (WriteFile(files[i].path, files[i].buf, files[i].size) != files[i].size)
			{
				pspDebugScreenPrintf("Error!\n");
				sceKernelSleepThread();
			}

			pspDebugScreenPrintf("OK\n");
		}

		ErrorExit(6000, "Update complete. Restarting in 6 seconds...\n");
	}

	printf("Unpacking... ");
	WriteFile("u235.prx", u235, sizeof(u235));
	WriteFile("PUPD.PBP", pupd, sizeof(pupd));
	WriteFile("DL.PBP", downloader, sizeof(downloader));
	printf(" done.\n");

	memset(&stat, 0, sizeof(stat));

	if (sceIoGetstat("390.PBP", &stat) < 0)
	{
		printf("390.PBP doesn't exist.\n"
			   "Do you want to download it from internet? (x=yes, R=no).\n");

		while (1)
		{
			SceCtrlData pad;

			sceCtrlReadBufferPositive(&pad, 1);

			if (pad.Buttons & PSP_CTRL_RTRIGGER)
				ErrorExit(5000, "Cancelled by user.\n");

			else if (pad.Buttons & PSP_CTRL_CROSS)
				break;

			sceKernelDelayThread(7000);

		}

		dl = 1;
	}	
	
	struct SceKernelLoadExecVSHParam param;
	char *program;

	if (dl)
	{
		program = "ms0:/PSP/GAME/UPDATE/DL.PBP";
	}
	else
	{
		program = "ms0:/PSP/GAME/UPDATE/PUPD.PBP";
	}

	memset(&param, 0, sizeof(param));			
	param.size = sizeof(param);
	param.args = strlen(program)+1;
	param.argp = program;
	param.key = (dl) ? "game" : "updater";
	
	printf("\nPlease wait...\n");
	sceDisplaySetHoldMode(1);
	
	if (!dl)
		sctrlKernelLoadExecVSHMs1(program, &param);

	else
		sctrlKernelLoadExecVSHMs2(program, &param);
	
	sceKernelSleepThread();
	
	return 0;
}

