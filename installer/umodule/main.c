#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspsuspend.h>
#include <psppower.h>
#include <pspsysmem_kernel.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <systemctrl.h>
#include <systemctrl_se.h>
#include <kubridge.h>

#include "systemctrl_01g.h"
#include "systemctrl_02g.h"
#include "vshctrl_01g.h"
#include "vshctrl_02g.h"
#include "usbdevice.h"
#include "satelite.h"
#include "recovery.h"
#include "popcorn.h"
#include "march33.h"
#include "idcanager.h"
#include "galaxy.h"
#include "pspbtcnf_01g.h"
#include "pspbtcnf_02g.h"

PSP_MODULE_INFO("uranium235_module", 0x0800, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VSH);

typedef struct
{
	char *path;
	u8 *buf;
	int size;
} M33File;

#define N_FILES		12
#define NEW_FILES	4

M33File m33_01g[N_FILES] =
{
	{ "flash0:/kd/pspbtjnf.bin", pspbtjnf_01g, sizeof(pspbtjnf_01g) },
	{ "flash0:/kd/pspbtknf.bin", pspbtknf_01g, sizeof(pspbtknf_01g) },
	{ "flash0:/kd/pspbtlnf.bin", pspbtlnf_01g, sizeof(pspbtlnf_01g) },
	{ "flash0:/kd/systemctrl.prx", systemctrl_01g, sizeof(systemctrl_01g) },
	{ "flash0:/kd/vshctrl.prx", vshctrl_01g, sizeof(vshctrl_01g) },
	{ "flash0:/kd/usbdevice.prx", usbdevice, sizeof(usbdevice) },
	{ "flash0:/vsh/module/satelite.prx", satelite, sizeof(satelite) },
	{ "flash0:/vsh/module/recovery.prx", recovery, sizeof(recovery) },
	{ "flash0:/kd/popcorn.prx", popcorn, sizeof(popcorn) },
	{ "flash0:/kd/march33.prx", march33, sizeof(march33) },
	{ "flash0:/kd/idcanager.prx", idcanager, sizeof(idcanager) },
	{ "flash0:/kd/galaxy.prx", galaxy, sizeof(galaxy) }
};

M33File m33_02g[N_FILES] =
{
	{ "flash0:/kd/pspbtjnf_02g.bin", pspbtjnf_02g, sizeof(pspbtjnf_02g) },
	{ "flash0:/kd/pspbtknf_02g.bin", pspbtknf_02g, sizeof(pspbtknf_02g) },
	{ "flash0:/kd/pspbtlnf_02g.bin", pspbtlnf_02g, sizeof(pspbtlnf_02g) },
	{ "flash0:/kd/systemctrl_02g.prx", systemctrl_02g, sizeof(systemctrl_02g) },
	{ "flash0:/kd/vshctrl_02g.prx", vshctrl_02g, sizeof(vshctrl_02g) },
	{ "flash0:/kd/usbdevice.prx", usbdevice, sizeof(usbdevice) },
	{ "flash0:/vsh/module/satelite.prx", satelite, sizeof(satelite) },
	{ "flash0:/vsh/module/recovery.prx", recovery, sizeof(recovery) },
	{ "flash0:/kd/popcorn.prx", popcorn, sizeof(popcorn) },
	{ "flash0:/kd/march33.prx", march33, sizeof(march33) },
	{ "flash0:/kd/idcanager.prx", idcanager, sizeof(idcanager) },
	{ "flash0:/kd/galaxy.prx", galaxy, sizeof(galaxy) }
};

M33File new_01g[NEW_FILES] =
{
	{ "flash0:/kd/systemctrl.prx", systemctrl_01g, sizeof(systemctrl_01g) },
	{ "flash0:/kd/vshctrl.prx", vshctrl_01g, sizeof(vshctrl_01g) },
	{ "flash0:/kd/march33.prx", march33, sizeof(march33) },
	{ "flash0:/vsh/module/recovery.prx", recovery, sizeof(recovery) },
};

M33File new_02g[NEW_FILES] =
{
	{ "flash0:/kd/systemctrl_02g.prx", systemctrl_02g, sizeof(systemctrl_02g) },
	{ "flash0:/kd/vshctrl_02g.prx", vshctrl_02g, sizeof(vshctrl_02g) },
	{ "flash0:/kd/march33.prx", march33, sizeof(march33) },
	{ "flash0:/vsh/module/recovery.prx", recovery, sizeof(recovery) },
};


u8 ipl_buf[300*1024] __attribute__((aligned(64)));
u8 buf1[100*1024] __attribute__((aligned(64)));
u8 buf2[100*1024] __attribute__((aligned(64)));

void exit_module()
{
	sceKernelDelayThread(800000);
	PlutoniumRebootPsp();
	sceKernelSleepThread();
	while (1);
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

void DisablePlugins()
{
	u8 conf[15];

	memset(conf, 0, 15);
	
	WriteFile("ms0:/seplugins/conf.bin", conf, 15);
}

void WriteConfig()
{
	SEConfig config;

	memset(&config, 0, sizeof(config));
	sctrlSEGetConfig(&config);

	WriteFile("flash1:/config.se", &config, sizeof(config));
}

int main(int argc, char *argv[])
{	
	M33File *files;
	int i, res;

	if (argc > 0)
	{
		if (strcmp(argv[0], "PARTIAL") == 0)
		{
			pspDebugScreenInit();
			pspDebugScreenClear();
			
			if (kuKernelGetModel() == PSP_MODEL_SLIM_AND_LITE)
			{
				files = new_02g;
			}
			else
			{
				files = new_01g;
			}

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

			pspDebugScreenPrintf("\nUpdate complete. Restarting in 6 seconds...\n");
			sceKernelDelayThread(6*1000*1000);
			sceKernelExitGame();
		}
	}

	if (PlutoniumGetModel() == PSP_MODEL_SLIM_AND_LITE)
	{
		files = m33_02g;
	}
	else
	{
		files = m33_01g;
	}

	for (i = 0; i < N_FILES; i++)
	{
		res = WriteFile(files[i].path, files[i].buf, files[i].size);

		if (res < 0)
		{
			exit_module();
		}		
	}

	DisablePlugins();
	exit_module();	
	
	return 0;
}


