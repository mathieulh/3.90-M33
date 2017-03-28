#ifndef __CONF_H__

#define __CONF_H__

#define CONFIG_MAGIC	0x47434553

enum 
{
	FAKE_REGION_DISABLED = 0,
	FAKE_REGION_JAPAN = 1,
	FAKE_REGION_AMERICA = 2,
	FAKE_REGION_EUROPE = 3,
	FAKE_REGION_KOREA = 4, /* do not use, may cause brick on restore default settings */
	FAKE_REGION_UNK = 5, 
	FAKE_REGION_UNK2 = 6,
	FAKE_REGION_AUSTRALIA = 7,
	FAKE_REGION_HONGKONG = 8, /* do not use, may cause brick on restore default settings */
	FAKE_REGION_TAIWAN = 9, /* do not use, may cause brick on restore default settings */
	FAKE_REGION_RUSSIA = 10,
	FAKE_REGION_CHINA = 11, /* do not use, may cause brick on restore default settings */
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

