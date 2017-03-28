#ifndef __CONF_H__

#define __CONF_H__

#define CONFIG_MAGIC	0x47434553

enum 
{
	FAKE_REGION_DISABLED = 0,
	FAKE_REGION_JAPAN = 1,
	FAKE_REGION_AMERICA = 2,
	FAKE_REGION_EUROPE = 3,
	FAKE_REGION_KOREA = 4,
};

typedef struct
{
	int magic; /* 0x47434553 */
	int hidecorrupt;
	int	skiplogo;
	int umdactivatedplaincheck;
	int gamekernel150;
	int executebootbin;
	int startupprog;
	int usenoumd;
	int useisofsonumdinserted;
	int	vshcpuspeed; /* not available yet */
	int	vshbusspeed; /* not available yet */
	int	umdisocpuspeed; /* not available yet */
	int	umdisobusspeed; /* not available yet */
	int fakeregion;
	int freeumdregion;
} SEConfig;
int SE_GetConfig(SEConfig *config);
int SE_SetConfig(SEConfig *config);


#endif

