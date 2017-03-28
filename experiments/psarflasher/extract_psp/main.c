#include <pspsdk.h>
#include <pspkernel.h>
#include <psploadexec.h>
#include <pspvshbridge.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "decryptprx.h"

/* 0x0800 -> vsh ? */
PSP_MODULE_INFO("ExtractPrx", 0x0800, 1, 0);

PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VSH);

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
	vshKernelExitVSHVSH(NULL);
}

int LoadStartModule(char *module)
{
	SceUID mod = vshKernelLoadModuleVSH(module, 0, NULL);

	if (mod < 0)
		return mod;

	return sceKernelStartModule(mod, strlen(module)+1, module, NULL, NULL);
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

u8 dataIn[6000000] __attribute__((aligned(64)));
u8 dataOut[6000000] __attribute__((aligned(64)));

#define PROGRAM "ms0:/PSP/GAME/UPDATE/EBOOT.PBP"

int main()
{
	SceUID fd, mod;
	int i, size;
	u32 header[10];
	char args[256];
	struct SceKernelLoadExecVSHParam param;
		
	pspDebugScreenInit();

	mod = LoadStartModule("decryptprx.prx");
	if (mod < 0)
	{
		ErrorExit(6000, "Error 0x%08X loading/starting decryptprx.prx.\n");
	}

	printf("Extracting special prx's from pbp...\n");

	fd = sceIoOpen("UPDATE.PBP", PSP_O_RDONLY, 0777);
	if (fd < 0)
	{
		ErrorExit(6000, "Cannot find UPDATE.PBP.\n");
	}
	
	sceIoRead(fd, &header, sizeof(header));

	if (header[0] != 0x50425000)
	{
		ErrorExit(6000, "Invalid PBP file.\n");
	}

	size = header[9]-header[8];

	sceIoLseek(fd, header[8], PSP_SEEK_SET);
	
	if (sceIoRead(fd, dataIn, size) < size)
	{
		ErrorExit(6000, "Corrupted PBP file.\n");
	}

	sceIoClose(fd);

	size = decrypt_prx(dataIn, size, dataOut);

	if (size < 0)
	{
		ErrorExit(6000, "Error decrypting DATA.PSP\n");
	}

	for (i = 0; i < size-20; i++)
	{
		if (memcmp(dataOut+i, "~PSP", 4) == 0)
		{
			if (strcmp((char *)dataOut+i+0x0A, "sceLflashFatfmt") == 0)
			{
				int size = *(int *)&dataOut[i+0x2C];
				WriteFile("lflash_fatfmt.prx", dataOut+i, size);
			}
			else if (strcmp((char *)dataOut+i+0x0A, "IplUpdater") == 0
			|| strcmp((char *)dataOut+i+0x0A, "TexSeqCounter") == 0) // lame $ce name... pathetic
			{
				int size = *(int *)&dataOut[i+0x2C];
				WriteFile("ipl_update.prx", dataOut+i, size);
			}
		}
	}

	memset(args, 0, sizeof(args));
	memset(&param, 0, sizeof(param));

	param.size = sizeof(param);
	param.args = 256;
	strcpy(args, PROGRAM);
	strcpy(args+strlen(args), "extracted");
	param.argp = args;
	param.key = "updater";

	vshKernelLoadExecVSHMs1(PROGRAM, &param);	

	return 0;
}
