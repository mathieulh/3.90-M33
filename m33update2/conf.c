#include <pspsdk.h>
#include <pspkernel.h>

#include <string.h>
#include "conf.h"

int SE_GetConfig(SEConfig *config)
{
	SceUID fd;
	
	memset(config, 0, sizeof(SEConfig));
	fd = sceIoOpen("flash1:/config.se", PSP_O_RDONLY, 0644);

	if (fd < 0)
	{
		return -1;
	}

	if (sceIoRead(fd, config, sizeof(SEConfig)) < sizeof(SEConfig))
	{
		sceIoClose(fd);
		return -1;
	}

	sceIoClose(fd);

	return 0;
}

int SE_SetConfig(SEConfig *config)
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

	sceIoClose(fd);
	return 0;
}


