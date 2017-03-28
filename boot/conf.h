#ifndef __CONF_H__

#define __CONF_H__

#define CONFIG_MAGIC	0x47434553

enum 
{
	FAKE_REGION_DISABLED = 0,
	FAKE_REGION_JAPAN = 1,
	FAKE_REGION_AMERICA = 2,
	FAKE_REGION_EUROPE = 3,
};

typedef struct
{
	int magic;
	int hidecorrupt;
	int	skiplogo;
	int umdactivatedplaincheck;
	int gamekernel150;
	int executebootbin;
	int startupprog;
	int usenoumd;
	int useisofsonumdinserted;
	int	vshcpuspeed;
	int	vshbusspeed;
	int	umdisocpuspeed;
	int	umdisobusspeed;
	int	fakeregion;
} SEConfig;

int SE_GetConfig(SEConfig *config);
int SE_SetConfig(SEConfig *config);


#endif

