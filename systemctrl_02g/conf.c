#include <pspsdk.h>
#include <pspkernel.h>

#include <string.h>
#include "main.h"
#include "systemctrl_se.h"

#define CONFIG_MAGIC	0x47434553

int sctrlSEGetConfigEx(SEConfig *config, int size)
{
	int k1 = pspSdkSetK1(0);
	int read;
	
	SceUID fd;
	
	memset(config, 0, size);
	fd = sceIoOpen("flash1:/config.se", PSP_O_RDONLY, 0);

	if (fd < 0)
	{
		pspSdkSetK1(k1);
		return -1;
	}

	read = sceIoRead(fd, config, size);

	sceIoClose(fd);
	pspSdkSetK1(k1);
	return read;
}

int sctrlSESetConfigEx(SEConfig *config, int size)
{
	int k1 = pspSdkSetK1(0);
		
	SceUID fd = sceIoOpen("flash1:/config.se", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (fd < 0)
	{
		pspSdkSetK1(k1);
		return -1;
	}

	config->magic = CONFIG_MAGIC;

	if (sceIoWrite(fd, config, size) < size)
	{
		sceIoClose(fd);
		pspSdkSetK1(k1);
		return -1;
	}

	sceIoClose(fd);
	pspSdkSetK1(k1);
	return 0;
}

int sctrlSEGetConfig(SEConfig *config)
{
	return sctrlSEGetConfigEx(config, sizeof(SEConfig));
}

int sctrlSESetConfig(SEConfig *config)
{
	return sctrlSESetConfigEx(config, sizeof(SEConfig));
}




