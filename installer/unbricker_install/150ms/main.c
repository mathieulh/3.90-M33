// PSAR dumper for Updater data 
// Original author: PspPet
//
// Contributions:
//
// Vampire (bugfixes)
// Nem (ipl decryption)
// Dark_AleX (2.60-2.80 decryption)
// Noobz (3.00-3.02 decryption)
// Team C+D (3.03-3.52 decryption)
// M33 Team (3.60-3.71 decryption) + recode for 2.XX+ kernels 

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

#include <libpsardumper.h>
#include <pspdecrypt.h>
#include <kubridge.h>

#include "ipl.h"
#include "pspbtcnf.h"
#include "flashemu.h"
#include "hibari.h"

PSP_MODULE_INFO("Resurrection_Manager", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

#define printf    pspDebugScreenPrintf

#define PRX_SIZE_371				5245648
#define LFLASH_FATFMT_UPDATER_SIZE	0x13C0	
#define NAND_UPDATER_SIZE			0x2150
#define LFATFS_UPDATER_SIZE			0x7880

#define PRX_SIZE_150	3737904
#define MSIPL_SIZE		241664
#define IPL150_SIZE		225280

#define PSAR_SIZE_150	10149440
#define PSAR_SIZE_340	16820272

#define N_FILES	5

typedef struct
{
	char *path;
	u8 *buf;
	int size;
} M33File;

M33File m33files[N_FILES] =
{
	{ "ms0:/kd/hibari.prx", hibari, sizeof(hibari) },
	{ "ms0:/kd/pspbtcnf.txt", pspbtcnf, sizeof(pspbtcnf) },
	{ "ms0:/kd/pspbtcnf_game.txt", pspbtcnf_game, sizeof(pspbtcnf_game) },
	{ "ms0:/kd/pspbtcnf_updater.txt", pspbtcnf_updater, sizeof(pspbtcnf_updater) },
	{ "ms0:/kd/flashemu.prx", flashemu, sizeof(flashemu) },
};


////////////////////////////////////////////////////////////////////
// big buffers for data. Some system calls require 64 byte alignment

// big enough for the full PSAR file
static u8 g_dataPSAR[19000000] __attribute__((aligned(64))); 

// big enough for the largest (multiple uses)
static u8 g_dataOut[3000000] __attribute__((aligned(0x40)));
   
// for deflate output
//u8 g_dataOut2[3000000] __attribute__((aligned(0x40)));
static u8 *g_dataOut2;   

static u8 msipl[MSIPL_SIZE]; 

static char com_table[0x4000];
static int comtable_size;

static char _1g_table[0x4000];
static int _1gtable_size;

static char _2g_table[0x4000];
static int _2gtable_size;

u8 updater_prx_371_md5[16] = 
{
	0x55, 0x6E, 0x30, 0xC9, 0x0C, 0xA5, 0xD7, 0x06, 
	0xF1, 0x36, 0x51, 0x18, 0x1E, 0x73, 0xBE, 0x2E
};

u8 updater_prx_150_md5[16] = 
{
	0x9F, 0xB7, 0xC7, 0xE9, 0x96, 0x79, 0x7F, 0xF2,
	0xD7, 0xFF, 0x4D, 0x14, 0xDD, 0xF5, 0xD3, 0xF2
};

u8 msipl_md5[16] = 
{
	0x14, 0x2C, 0xC4, 0xE1, 0x8E, 0xDD, 0x07, 0xF2, 
	0x54, 0x47, 0x93, 0xA1, 0xCB, 0xB4, 0x39, 0x20
};

u8 psar_150_md5[16] = 
{
	0xF5, 0x65, 0x54, 0xE2, 0xBE, 0x35, 0x64, 0x5A, 
	0x76, 0x0B, 0x18, 0xED, 0x65, 0x05, 0xC4, 0xCC
};

u8 psar_340_md5[16] = 
{
	0xC5, 0xDE, 0xF2, 0x9B, 0xD8, 0xC2, 0xAC, 0x62, 
	0x75, 0x60, 0x46, 0xE8, 0xFE, 0xDD, 0xAC, 0x7C
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

static int FindTablePath(char *table, int table_size, char *number, char *szOut)
{
	int i, j, k;

	for (i = 0; i < table_size-5; i++)
	{
		if (strncmp(number, table+i, 5) == 0)
		{
			for (j = 0, k = 0; ; j++, k++)
			{
				if (table[i+j+6] < 0x20)
				{
					szOut[k] = 0;
					break;
				}

				if (!strncmp(table+i+6, "flash", 5) &&
					j == 6)
				{
					szOut[6] = ':';
					szOut[7] = '/';
					k++;
				}
				else if (!strncmp(table+i+6, "ipl", 3) &&
					j == 3)
				{
					szOut[3] = ':';
					szOut[4] = '/';
					k++;
				}
				else
				{				
					szOut[k] = table[i+j+6];
				}
			}

			return 1;
		}
	}

	return 0;
}

#define NOWRITE_150	16

char *nowrite_150[NOWRITE_150] =
{
	"syscon.prx",
	"emc_sm.prx",
	"led.prx",
	"display.prx",
	"clockgen.prx",
	"ctrl.prx",
	"lcdc.prx",
	"gpio.prx",
	"i2c.prx",
	"sysreg.prx",
	"dmacman.prx",
	"dmacplus.prx",
	"pwm.prx",
	"emc_ddr.prx",
	"power.prx",
	"systimer.prx"
};

#define WRITE_340	16

// clockgen -> buggy
// i2c -> crash

char *write_340[WRITE_340] =
{
	"syscon.prx",
	"emc_sm.prx",
	"led.prx",
	"display.prx",
	"clockgen.prx",
	"ctrl.prx",
	"lcdc.prx",
	"gpio.prx",
	"i2c.prx",
	"sysreg.prx",
	"dmacman.prx",
	"dmacplus.prx",
	"pwm.prx",
	"emc_ddr.prx",
	"power.prx",
	"systimer.prx"
};

// audio, i2c (Z), impose, vaudio, msaudio, codec (Z), idstorage(Z)

// display, clockgen, ctrl, lcdc, gpio, i2c, syscon, sysreg, dmacman, dmacplus, pwm, emc_ddr, emc_sm, power, systimer


// Lista: audio, i2c(Z), impose, vaudio, msaudio, codec(Z), 
// idstorage(Z), display, clockgen, ctrl, lcdc, gpio, syscon, 
// sysreg, dmacman, dmacplus, pwm, emc_ddr, emc_sm, power, systimer
// led

static int WritePrx(char *name, int is150)
{
	int i;

	if (is150)
	{
		for (i = 0; i < NOWRITE_150; i++)
		{
			if (strcmp(name, nowrite_150[i]) == 0)
				return 0;
		}

		return 1;
	}

	for (i = 0; i < WRITE_340; i++)
	{
		if (strcmp(name, write_340[i]) == 0)
			return 1;
	}
	
	return 0;
}

int LoadStartModule(char *module, int partition)
{
	SceUID mod = kuKernelLoadModule(module, 0, NULL);

	if (mod < 0)
		return mod;

	return sceKernelStartModule(mod, 0, NULL, NULL, NULL);
}

void ExtractPrxs(int cbFile, int is150)
{
	if (pspPSARInit(g_dataPSAR, g_dataOut, g_dataOut2) < 0)
	{
		ErrorExit(5000, "pspPSARInit failed!.\n");
	}   

    while (1)
	{
		char name[128];
		int cbExpanded;
		int pos;
		int signcheck;

		int res = pspPSARGetNextFile(g_dataPSAR, cbFile, g_dataOut, g_dataOut2, name, &cbExpanded, &pos, &signcheck);

		if (res < 0)
		{
			ErrorExit(5000, "PSAR decode error, pos=0x%08X.\n", pos);
		}
		else if (res == 0) /* no more files */
		{
			break;
		}
		
		if (!strncmp(name, "com:", 4) && comtable_size > 0)
		{
			if (!FindTablePath(com_table, comtable_size, name+4, name))
			{
				ErrorExit(5000, "Error: cannot find path of %s.\n", name);
			}
		}

		else if (!strncmp(name, "01g:", 4) && _1gtable_size > 0)
		{
			if (!FindTablePath(_1g_table, _1gtable_size, name+4, name))
			{
				ErrorExit(5000, "Error: cannot find path of %s.\n", name);
			}
		}

		else if (!strncmp(name, "02g:", 4) && _2gtable_size > 0)
		{
			if (!FindTablePath(_2g_table, _2gtable_size, name+4, name))
			{
				ErrorExit(5000, "Error: cannot find path of %s.\n", name);
			}
		}

       	char* szFileBase = strrchr(name, '/');
		
		if (szFileBase != NULL)
			szFileBase++;  // after slash
		else
			szFileBase = "err.err";

		if (cbExpanded > 0)
		{
			char szDataPath[128];
			
			if (1)
			{
				if (WritePrx(szFileBase, is150))
				{				
					int lfatfs = 0;
					
					sprintf(szDataPath, "ms0:/%s", name+8);

					if (is150)
					{
					   if (WriteFile(szDataPath, g_dataOut2, cbExpanded) != cbExpanded)
					   {
							ErrorExit(5000, "Error writing %s.\n", szDataPath);
					   }

					   if (strcmp(name, "flash0:/kd/lfatfs.prx") == 0)
					   {
						   lfatfs = 1;
						   strcpy(szDataPath, "ms0:/kd/lfatfs_patched.prx");
						}
					}					

					if (lfatfs || !is150)
					{
						cbExpanded = pspDecryptPRX(g_dataOut2, g_dataOut, cbExpanded);
						if (cbExpanded <= 0)
						{
							ErrorExit(5000, "Error decrypting %s.\n", szFileBase);
						}

						cbExpanded = pspDecompress(g_dataOut, g_dataOut2, sizeof(g_dataOut));

						if (cbExpanded <= 0)
						{
							ErrorExit(5000, "Error decompressing %s.\n", szFileBase);
						}

						if (lfatfs)
						{
							_sw(0x3E00008, (u32)g_dataOut2+0x7AC); // jr ra
							_sw(0x1021, (u32)g_dataOut2+0x7B0); // mov v0, zero
						}

						if (WriteFile(szDataPath, g_dataOut2, cbExpanded) != cbExpanded)
						{
							ErrorExit(5000, "Error writing %s.\n", szDataPath);
						}
					}
				}
			}

			else if (!strcmp(name, "com:00000"))
			{
				comtable_size = pspDecryptTable(g_dataOut2, g_dataOut, cbExpanded);
							
				if (comtable_size <= 0)
				{
					ErrorExit(5000, "Cannot decrypt common table.\n");
				}

				if (comtable_size > sizeof(com_table))
				{
					ErrorExit(5000, "Com table buffer too small. Recompile with bigger buffer.\n");
				}

				memcpy(com_table, g_dataOut2, comtable_size);						
			}
					
			else if (!strcmp(name, "01g:00000"))
			{
				_1gtable_size = pspDecryptTable(g_dataOut2, g_dataOut, cbExpanded);
							
				if (_1gtable_size <= 0)
				{
					ErrorExit(5000, "Cannot decrypt 1g table.\n");
				}

				if (_1gtable_size > sizeof(_1g_table))
				{
					ErrorExit(5000, "1g table buffer too small. Recompile with bigger buffer.\n");
				}

				memcpy(_1g_table, g_dataOut2, _1gtable_size);						
			}
					
			else if (!strcmp(name, "02g:00000"))
			{
				_2gtable_size = pspDecryptTable(g_dataOut2, g_dataOut, cbExpanded);
							
				if (_2gtable_size <= 0)
				{
					ErrorExit(5000, "Cannot decrypt 2g table %08X.\n", _2gtable_size);
				}

				if (_2gtable_size > sizeof(_2g_table))
				{
					ErrorExit(5000, "2g table buffer too small. Recompile with bigger buffer.\n");
				}

				memcpy(_2g_table, g_dataOut2, _2gtable_size);						
			}			
		}

		scePowerTick(0);
	}
}

static void BuildIpl()
{
	int size, i;
	u8  md5[16];

	printf("Building IPL... ");
	
	if (ReadFile("ms0:/150.PBP", 0x3251, g_dataPSAR, PRX_SIZE_150) != PRX_SIZE_150)
	{
		ErrorExit(5000, "Incorrect or inexistant 150.PBP at root.\n");
	}	

	sceKernelUtilsMd5Digest(g_dataPSAR, PRX_SIZE_150, md5);

	if (memcmp(md5, updater_prx_150_md5, 16) != 0)
	{
		ErrorExit(5000, "Incorrect 150.PBP.\n");
	}

	size = pspDecryptPRX(g_dataPSAR, g_dataPSAR, PRX_SIZE_150);
	if (size <= 0)
	{
		ErrorExit(5000, "Error decrypting 150 updater.\n");
	}

	for (i = 0; i < size-20; i++)
	{
		if (memcmp(g_dataPSAR+i, "~PSP", 4) == 0)
		{
			if (strcmp((char *)g_dataPSAR+i+0x0A, "IplUpdater") == 0)
			{
				size = *(int *)&g_dataPSAR[i+0x2C];
				memcpy(g_dataOut, g_dataPSAR+i, size);
				break;
			}
		}
	}

	if (i == (size-20))
	{
		ErrorExit(5000, "Curious error.\n");
	}

	if (pspDecryptPRX(g_dataOut, g_dataOut2, size) < 0)
	{
		ErrorExit(5000, "Error decrypting ipl_update.prx.\n");
	}

	if (pspDecryptIPL1(g_dataOut2+0x980, g_dataOut, IPL150_SIZE) != IPL150_SIZE)
	{
		ErrorExit(5000, "Error decrypting 150 ipl.\n");
	}

	u32 *code = (u32 *)(g_dataOut+0x10);
	code[0xE0/4] = 0x09038000; // j 0x040e0000
	code[0xE4/4] = 0; // nop

	memcpy(msipl, custom_block, sizeof(custom_block));
	memcpy(msipl+sizeof(custom_block), g_dataOut, IPL150_SIZE);

	sceKernelUtilsMd5Digest(msipl, MSIPL_SIZE, md5);

	if (memcmp(md5, msipl_md5, 16) != 0)
	{
		ErrorExit(5000, "Error creating ipl.\n");
	}

	if (WriteFile("ms0:/msipl.bin", msipl, MSIPL_SIZE) != MSIPL_SIZE)
	{
		ErrorExit(5000, "Error creating ipl.\n");
	}

	printf("OK\n");
}

static void Extract150PSAR()
{
	u8 md5[16];
	
	printf("Extracting 1.50 kernel subset... ");
	
	if (ReadFile("ms0:/150.PBP", 0x393B81, g_dataPSAR, PSAR_SIZE_150) != PSAR_SIZE_150)
    {
        ErrorExit(5000, "Incorrect or inexistant 150.PBP at root.\n");
	}   
    
	sceKernelUtilsMd5Digest(g_dataPSAR, PSAR_SIZE_150, md5);

	if (memcmp(md5, psar_150_md5, 16) != 0)
	{
		ErrorExit(5000, "Incorrect or corrupted 150.PBP.\n");
	}

	ExtractPrxs(PSAR_SIZE_150, 1);

	printf("OK\n");
}

static void Extract340PSAR()
{
	u8 md5[16];
	
	printf("Extracting 3.40 kernel subset... ");
	
	if (ReadFile("ms0:/340.PBP", 0x4FC6C5, g_dataPSAR, PSAR_SIZE_340) != PSAR_SIZE_340)
    {
        ErrorExit(5000, "Incorrect or inexistant 340.PBP at root.\n");
	}   
    
	sceKernelUtilsMd5Digest(g_dataPSAR, PSAR_SIZE_340, md5);

	if (memcmp(md5, psar_340_md5, 16) != 0)
	{
		ErrorExit(5000, "Incorrect or corrupted 340.PBP.\n");
	}

	ExtractPrxs(PSAR_SIZE_340, 0);

	printf("OK\n");
}

int main(void)
{
    int i;	
	
	pspDebugScreenInit();

	if (sceKernelDevkitVersion() < 0x02070110)
	{
		ErrorExit(10000, "This program requires 2.71 or higher.\n",
			             "If you are in a cfw, please reexecute psardumper on the higher kernel.\n");
	}

	SceUID mod = LoadStartModule("libpsardumper.prx", PSP_MEMORY_PARTITION_KERNEL);
	if (mod < 0)
	{
		ErrorExit(5000, "Error 0x%08X loading/starting libpsardumper.prx.\n", mod);
	}

	mod = LoadStartModule("pspdecrypt.prx", PSP_MEMORY_PARTITION_KERNEL);
	if (mod < 0)
	{
		ErrorExit(5000, "Error 0x%08X loading/starting pspdecrypt.prx.\n", mod);
	}

    printf("Press cross to begin the installation.\n\n");

	while (1)
	{
		SceCtrlData pad;

		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_CROSS)
		{
			break;
		}	

		sceKernelDelayThread(10000);
	}

	sceKernelVolatileMemLock(0, (void *)&g_dataOut2, &i);

	sceIoMkdir("ms0:/kd", 0777);
	sceIoMkdir("ms0:/kd/resource", 0777);
	sceIoMkdir("ms0:/data", 0777);
	sceIoMkdir("ms0:/data/cert", 0777);
	sceIoMkdir("ms0:/dic", 0777);
	sceIoMkdir("ms0:/font", 0777);
	sceIoMkdir("ms0:/vsh", 0777);
	sceIoMkdir("ms0:/vsh/etc", 0777);
	sceIoMkdir("ms0:/vsh/module", 0777);
	sceIoMkdir("ms0:/vsh/resource", 0777);
	sceIoMkdir("ms0:/registry", 0777);
	
	BuildIpl();
	Extract150PSAR();
	Extract340PSAR();

	printf("Writing custom modules\n");

	for (i = 0; i < N_FILES; i++)
	{
		printf("Writing %s... ", m33files[i].path);
		
		if (WriteFile(m33files[i].path, m33files[i].buf, m33files[i].size) != m33files[i].size)
		{
			ErrorExit(5000, "Error writing %s.\n", m33files[i].path);
		}

		printf("OK\n");
	}

	printf("OK\n");

	printf("Copying registry... ");

	i = ReadFile("flash1:/registry/system.dreg", 0, g_dataOut, sizeof(g_dataOut));
	if (i <= 0)
	{
		ErrorExit(5000, "Error reading system.dreg.\n");
	}

	if (WriteFile("ms0:/registry/system.dreg", g_dataOut, i) != i)
	{
		ErrorExit(5000, "Error writing system.dreg.\n");
	}

	i = ReadFile("flash1:/registry/system.ireg", 0, g_dataOut,sizeof(g_dataOut));
	if (i <= 0)
	{
		ErrorExit(5000, "Error reading system.ireg.\n");
	}
	
	if (WriteFile("ms0:/registry/system.ireg", g_dataOut, i) != i)
	{
		ErrorExit(5000, "Error writing system.ireg.\n");
	}

	printf("OK\n");

	ErrorExit(7000, "Done.\nAuto-exiting in 7 seconds.\n");

    return 0;
}

