#ifndef __CONF_H__

#define __CONF_H__

#define CONFIG_MAGIC	0x47434553

typedef struct
{
	int magic;
	int hidecorrupt;
	int	skiplogo;
	int umdactivatedplaincheck;
	int gamekernel150;
	int executebootbin;
	int startupprog;
} SEConfig;

int SE_GetConfig(SEConfig *config);
int SE_SetConfig(SEConfig *config);


#endif

