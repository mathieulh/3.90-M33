#include <pspsdk.h>
#include <pspkernel.h>

#include <string.h>
#include "systemctrl_se.h"

#define CONFIG_MAGIC	0x47434553

int sctrlSEGetConfig(SEConfig *config)
{
	int k1 = pspSdkSetK1(0);
	
	SceUID fd;
	
	memset(config, 0, sizeof(SEConfig));
	fd = sceIoOpen("flash1:/config.se", PSP_O_RDONLY, 0644);

	if (fd < 0)
	{
		pspSdkSetK1(k1);
		return -1;
	}

	if (sceIoRead(fd, config, sizeof(SEConfig)) < sizeof(SEConfig))
	{
		sceIoClose(fd);
		pspSdkSetK1(k1);
		return -1;
	}

	pspSdkSetK1(k1);
	return 0;
}

int sctrlSESetConfig(SEConfig *config)
{
	sceIoRemove("flash1:/config.se");

	SceUID fd = sceIoOpen("flash1:/config.se", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (fd < 0)
	{
		return -1;
	}

	config->magic = CONFIG_MAGIC;

	if (sceIoWrite(fd, config, sizeof(SEConfig)) < sizeof(SEConfig))
	{
		sceIoClose(fd);
		return -1;
	}

	return 0;
}


