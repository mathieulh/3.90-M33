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
#include <pspreg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include <libpsardumper.h>
#include <pspdecrypt.h>
#include <kubridge.h>

#include "ipl.h"
#include "pspdecrypt_module.h"
#include "pspbtcnf.h"
#include "libpsardumper.h"
#include "resurrection.h"
#include "flashemu.h"
#include "tmctrl150_340.h"
#include "backup.h"
#include "hibari.h"
#include "iplloader.h"

PSP_MODULE_INFO("Resurrection_Manager", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

#define printf    pspDebugScreenPrintf
#define OK() printf("OK\n");				

#define PRX_SIZE_390				5326560
#define LFLASH_FATFMT_UPDATER_SIZE	0x13B0	
#define NAND_UPDATER_SIZE			0x2190
#define LFATFS_UPDATER_SIZE			0x7860

#define PRX_SIZE_150	3737904
#define MSIPL_SIZE		241664
#define IPL150_SIZE		225280
#define IPL150_LIN		213488

#define PSAR_SIZE_150	10149440
#define PSAR_SIZE_340	16820272

#define N_FILES	10

typedef struct
{
	char *path;
	u8 *buf;
	int size;
} M33File;

M33File m33files[N_FILES] =
{
	{ "ms0:/TM/DC5/kd/resurrection.elf", resurrection, sizeof(resurrection) },
	{ "ms0:/TM/DC5/kd/hibari.prx", hibari, sizeof(hibari) },
	{ "ms0:/TM/DC5/kd/pspdecrypt.prx", pspdecrypt, sizeof(pspdecrypt) },
	{ "ms0:/TM/DC5/kd/libpsardumper.prx", libpsardumper, sizeof(libpsardumper) },
	{ "ms0:/TM/DC5/kd/pspbtcnf.txt", pspbtcnf, sizeof(pspbtcnf) },
	{ "ms0:/TM/DC5/kd/pspbtcnf_game.txt", pspbtcnf_game, sizeof(pspbtcnf_game) },
	{ "ms0:/TM/DC5/kd/pspbtcnf_updater.txt", pspbtcnf_updater, sizeof(pspbtcnf_updater) },
	{ "ms0:/TM/DC5/flashemu150_340.prx", flashemu150_340, sizeof(flashemu150_340) },
	{ "ms0:/TM/DC5/tmctrl150_340.prx", tmctrl150_340, sizeof(tmctrl150_340) },
	{ "ms0:/TM/DC5/kd/backup.elf", backup, sizeof(backup) }
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

u8 updater_prx_390_md5[16] = 
{
	0x0D, 0x78, 0x75, 0x6A, 0x3E, 0x44, 0x2E, 0x0E, 
	0x9E, 0xF8, 0xD4, 0x1F, 0x41, 0xF6, 0xFC, 0x09
};

u8 updater_prx_150_md5[16] = 
{
	0x9F, 0xB7, 0xC7, 0xE9, 0x96, 0x79, 0x7F, 0xF2,
	0xD7, 0xFF, 0x4D, 0x14, 0xDD, 0xF5, 0xD3, 0xF2
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

#define NOWRITE_150	72

char *nowrite_150[NOWRITE_150] =
{
	"ata.prx",
	"idstorage.prx",
	"lfatfs.prx",
	"audio.prx",
	"audiocodec.prx",
	"blkdev.prx",
	"chkreg.prx",
	"codec.prx",
	"ifhandle.prx",
	"impose.prx",
	"isofs.prx",
	"lflash_fatfmt.prx",
	"libatrac3plus.prx",
	"libhttp.prx",
	"libparse_http.prx",
	"libparse_uri.prx",
	"libupdown.prx",
	"me_for_vsh.prx",
	"me_wrapper.prx",
	"mebooter.prx",
	"mebooter_umdvideo.prx",
	"mediaman.prx",
	"mediasync.prx",
	"mgr.prx",
	"mpeg_vsh.prx",
	"mpegbase.prx",
	"msaudio.prx",
	"openpsid.prx",
	"peq.prx",
	"pspnet.prx",
	"pspnet_adhoc.prx",
	"pspnet_adhoc_auth.prx",
	"pspnet_adhoc_download.prx",
	"pspnet_adhoc_matching.prx",
	"pspnet_adhocctl.prx",
	"pspnet_ap_dialog_dummy.prx",
	"pspnet_apctl.prx",
	"pspnet_inet.prx",
	"pspnet_inet.prx",
	"reboot.prx",
	"semawm.prx",
	"sircs.prx",
	"uart4.prx",
	"umdman.prx",
	"umd9660.prx",
	"usb.prx",
	"usbstor.prx",
	"usbstorboot.prx",
	"usbstormgr.prx",
	"usbstorms.prx",
	"utility.prx",
	"vaudio.prx",
	"vaudio_game.prx",
	"videocodec.prx",
	"vshbridge.prx",
	"wlan.prx",
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

#define WRITE_340	17

char *write_340[WRITE_340] =
{
	"syscon.prx",
	"idstorage.prx",
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

// display, clockgen, ctrl, lcdc, gpio, i2c, sysreg, dmacman, dmacplus, pwm, emc_ddr, power, systimer

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

int get_registry_value(const char *dir, const char *name, unsigned int *val)
{
        int ret = 0;
        struct RegParam reg;
        REGHANDLE h;

        memset(&reg, 0, sizeof(reg));
        reg.regtype = 1;
        reg.namelen = strlen("/system");
        reg.unk2 = 1;
        reg.unk3 = 1;
        strcpy(reg.name, "/system");
        if(sceRegOpenRegistry(&reg, 2, &h) == 0)
        {
                REGHANDLE hd;
                if(!sceRegOpenCategory(h, dir, 2, &hd))
                {
                        REGHANDLE hk;
                        unsigned int type, size;

                        if(!sceRegGetKeyInfo(hd, name, &hk, &type, &size))
                        {
                                if(!sceRegGetKeyValue(hd, hk, val, 4))
                                {
                                        ret = 1;
                                        sceRegFlushCategory(hd);
                                }
                        }
                        sceRegCloseCategory(hd);
                }
                sceRegFlushRegistry(h);
                sceRegCloseRegistry(h);
        }

        return ret;
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
			
			if (strncmp(name, "flash0:/kd/", 11) == 0 &&
				   strncmp(name, "flash0:/kd/resource", 19) != 0)
			{
				if (WritePrx(szFileBase, is150))
				{				
					sprintf(szDataPath, "ms0:/TM/DC5/kd/%s", szFileBase);

					if (is150)
					{
					   if (WriteFile(szDataPath, g_dataOut2, cbExpanded) != cbExpanded)
					   {
							ErrorExit(5000, "Error writing %s.\n", szDataPath);
					   }					   
					}					

					if (!is150)
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

static void Extract390Modules()
{
	int size, i;
	u8 md5[16];
	u8 pbp_header[0x28];

	printf("Extracting 390 updater modules... ");
	
	if (ReadFile("ms0:/390.PBP", 0, pbp_header, sizeof(pbp_header)) != sizeof(pbp_header))
	{
		ErrorExit(5000, "Error reading 390.PBP at root.\n");
	}

	if (ReadFile("ms0:/390.PBP", *(u32 *)&pbp_header[0x20], g_dataPSAR, PRX_SIZE_390) != PRX_SIZE_390)
	{
		ErrorExit(5000, "Invalid 390.PBP.\n");
	}

	sceKernelUtilsMd5Digest(g_dataPSAR, PRX_SIZE_390, md5);

	if (memcmp(md5, updater_prx_390_md5, 16) != 0)
	{
		ErrorExit(5000, "Invalid 390.PBP (2).\n");
	}

	size = pspDecryptPRX(g_dataPSAR, g_dataPSAR, PRX_SIZE_390);
	if (size <= 0)
	{
		ErrorExit(5000, "Error decrypting 390 updater.\n");
	}

	if (WriteFile("ms0:/TM/DC5/kd/lflash_fatfmt_updater.prx", g_dataPSAR+0x4A7700, LFLASH_FATFMT_UPDATER_SIZE) != LFLASH_FATFMT_UPDATER_SIZE)
	{
		ErrorExit(5000, "Error writing lflash_fatfmt_updater.prx.\n");
	}

	for (i = 0; i < LFATFS_UPDATER_SIZE; i++)
	{
		g_dataPSAR[i+0xD180] ^= 0xF1;
	}

	if (WriteFile("ms0:/TM/DC5/kd/lfatfs_updater.prx", g_dataPSAR+0xD340, LFATFS_UPDATER_SIZE) != LFATFS_UPDATER_SIZE)
	{
		ErrorExit(5000, "Error writing lfatfs_updater.prx.\n");
	}

	for (i = 0; i < NAND_UPDATER_SIZE; i++)
	{
		g_dataPSAR[i+0x14A00] ^= 0xF1;
	}

	if (WriteFile("ms0:/TM/DC5/kd/nand_updater.prx", g_dataPSAR+0x14BC0, NAND_UPDATER_SIZE) != NAND_UPDATER_SIZE)
	{
		ErrorExit(5000, "Error writing nand_updater.prx.\n");
	}
	
	printf("OK\n");
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

	if (pspLinearizeIPL2(g_dataOut, g_dataOut2, IPL150_SIZE) != IPL150_LIN)
	{
		ErrorExit(5000, "Error in linearize.\n");		
	}

	u32 *code = (u32 *)(g_dataOut2);
	code[0xE0/4] = 0x09038000; // j 0x040e0000
	code[0xE4/4] = 0; // nop

	sceKernelDcacheWritebackAll();

	memcpy(msipl, custom_block, sizeof(custom_block));
	memcpy(msipl+sizeof(custom_block), g_dataOut2, IPL150_LIN);

	if (WriteFile("ms0:/TM/DC5/ipl.bin", msipl, IPL150_LIN+sizeof(custom_block)) != IPL150_LIN+sizeof(custom_block))
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

int ReadSector(int sector, void *buf, int count)
{
	SceUID fd = sceIoOpen("msstor0:", PSP_O_RDONLY, 0);
	if (fd < 0)
		return fd;

	sceIoLseek(fd, sector*512, PSP_SEEK_SET);
	int read = sceIoRead(fd, buf, count*512);

	if (read > 0)
		read /= 512;

	sceIoClose(fd);
	return read;
}

int WriteSector(int sector, void *buf, int count)
{
	SceUID fd = sceIoOpen("msstor0:", PSP_O_WRONLY, 0);
	if (fd < 0)
		return fd;

	sceIoLseek(fd, sector*512, PSP_SEEK_SET);
	int written = sceIoWrite(fd, buf, count*512);

	if (written > 0)
		written /= 512;

	sceIoClose(fd);
	return written;
}

char *GetKeyName(u32 key)
{
	if (key == PSP_CTRL_SELECT)
		return "SELECT";

	if (key == PSP_CTRL_START)
		return "START";

	if (key == PSP_CTRL_UP)
		return "UP";

	if (key == PSP_CTRL_DOWN)
		return "DOWN";

	if (key == PSP_CTRL_LEFT)
		return "LEFT";

	if (key == PSP_CTRL_RIGHT)
		return "RIGHT";

	if (key == PSP_CTRL_SQUARE)
		return "SQUARE";

	if (key == PSP_CTRL_CIRCLE)
		return "CIRCLE";

	if (key == PSP_CTRL_CROSS)
		return "CROSS";

	if (key == PSP_CTRL_LTRIGGER)
		return "L";

	if (key == PSP_CTRL_TRIANGLE)
		return "TRIANGLE";

	if (key == PSP_CTRL_RTRIGGER)
		return "R";

	return NULL;
}

int install_iplloader()
{
	int res;
	int abs_sec,ttl_sec;
	int signature;
	int type;
	int free_sectors;

	printf("Installing iplloader... ");

	res = ReadSector(0, g_dataOut, 1);
	if (res != 1)
	{
		printf("Error 0x%08X in ReadSector.\n", res);
		return -1;
	}

	abs_sec   = g_dataOut[0x1c6]|(g_dataOut[0x1c7]<<8)|(g_dataOut[0x1c8]<<16)|(g_dataOut[0x1c9]<<24);
	ttl_sec   = g_dataOut[0x1ca]|(g_dataOut[0x1cb]<<8)|(g_dataOut[0x1cc]<<16)|(g_dataOut[0x1cd]<<24);
	signature = g_dataOut[0x1fe]|(g_dataOut[0x1ff]<<8);
	type      = g_dataOut[0x1c2];

	if (signature != 0xAA55)
	{
		printf("Invalid signature 0x%04X\n", signature);
		return -2;
	}

	free_sectors = (abs_sec-0x10);
	if (free_sectors < 32)
	{
		printf("Too few free sectors (%d). 32 at least required.\n", free_sectors);
		return -3;
	}

	res = WriteSector(0x10, iplloader, 32);
	if (res != 32)
	{
		printf("Error 0x%08X in WriteSector.\n", res);
	    return -4;
	}

	char *default_config = "NOTHING = \"/TM/DC5/ipl.bin\";";

	SceIoStat stat;

	if (sceIoGetstat("ms0:/TM/config.txt", &stat) < 0)
	{
		WriteFile("ms0:/TM/config.txt", default_config, strlen(default_config));
	}
	else
	{
		SceCtrlData pad;
		char buf[256];

		sceKernelDelayThread(700000);

		OK();

		printf("Please keep pressed the key/keys which you want to use to boot DC   for some seconds... ");

		while (1)
		{		
			sceKernelDelayThread(3000000);
			
			sceCtrlReadBufferPositive(&pad, 1);
			
			if (pad.Buttons != 0)
				break;
		}

		strcpy(buf, "");

		int i;
		int first = 1;

		for (i = 0; i < 32; i++)
		{
			if ((pad.Buttons & (1 << i)))
			{
				if (GetKeyName(1 << i))
				{
					if (!first)
						strcat(buf, "+");
					else
						first = 0;
					
					strcat(buf, GetKeyName(1 << i));
				}
			}
		}

		printf("%s\n", buf);
		strcat(buf, " = \"/TM/DC5/ipl.bin\";\r\n");

		memcpy(g_dataOut, buf, strlen(buf));

		int size = ReadFile("ms0:/TM/config.txt", 0, g_dataOut+strlen(buf), sizeof(g_dataOut));

		if (size >= 0)
		{
			WriteFile("ms0:/TM/config.txt", g_dataOut, size+strlen(buf));
		}

		return 0;
	}
	
	OK();
	return 0;
}

int main(void)
{
    int i, theme = 0xDA;
	
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

	if (get_registry_value("/CONFIG/SYSTEM/XMB/THEME", "custom_theme_mode", &theme))
	{	
		if (theme != 0)
		{
			ErrorExit(7000, "Your psp has a custom theme set.\n"
							"Turn the theme off before running this program.\n");
		}
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

	if (install_iplloader() < 0)
	{
		ErrorExit(7000, "");
	}

	printf("Creating directories... ");

	sceIoMkdir("ms0:/TM", 0777);
	sceIoMkdir("ms0:/TM/DC5", 0777);
	sceIoMkdir("ms0:/TM/DC5/kd", 0777);
	sceIoMkdir("ms0:/TM/DC5/registry", 0777);

	printf ("OK\n");
	
	Extract390Modules();
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

	if (WriteFile("ms0:/TM/DC5/registry/system.dreg", g_dataOut, i) != i)
	{
		ErrorExit(5000, "Error writing system.dreg.\n");
	}

	i = ReadFile("flash1:/registry/system.ireg", 0, g_dataOut,sizeof(g_dataOut));
	if (i <= 0)
	{
		ErrorExit(5000, "Error reading system.ireg.\n");
	}
	
	if (WriteFile("ms0:/TM/DC5/registry/system.ireg", g_dataOut, i) != i)
	{
		ErrorExit(5000, "Error writing system.ireg.\n");
	}

	i = ReadFile("flash2:/act.dat", 0, g_dataOut,sizeof(g_dataOut));
	if (i > 0)
	{
	
		if (WriteFile("ms0:/TM/DC5/registry/act.dat", g_dataOut, i) != i)
		{
			ErrorExit(5000, "Error writing act.dat.\n");
		}
	}

	printf("OK\n");

	ErrorExit(7000, "Done.\nAuto-exiting in 7 seconds.\n");

    return 0;
}

