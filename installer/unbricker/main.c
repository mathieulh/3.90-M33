#include <pspsdk.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspusb.h>
#include <pspusbstor.h>
#include <pspnand_driver.h>
#include <psploadexec.h>
#include <pspsuspend.h>
#include <pspsyscon.h>
#include <psplflash_fatfmt.h>
#include <pspdisplay_kernel.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "pspdecrypt.h"
#include "libpsardumper.h"

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

#include "ipl_01g.h"
#include "ipl_02g.h"

PSP_MODULE_INFO("Despertar_del_Cementerio", 0x1000, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

#define printf pspDebugScreenPrintf

static u8 g_dataPSAR[20320000] __attribute__((aligned(64))); 
static u8 g_dataOut[2600000] __attribute__((aligned(64)));
static u8 *g_dataOut2;   

static char com_table[0x4000];
static int comtable_size;

static char _1g_table[0x4000];
static int _1gtable_size;

static char _2g_table[0x4000];
static int _2gtable_size;

static int current_led=0;


int psp_slim = 0;

typedef struct
{
	char *path;
	u8 *buf;
	int size;
} M33File;

#define N_FILES	12

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

static u8 config[88] = 
{
	0x53, 0x45, 0x43, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static u32 led_mode[] =
{
	0x00000014,
	0x00000003,
	0x00000002,
	0x0000000A,
	0x00000000
};

#define RESTORE_BUTTONS (PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER | PSP_CTRL_HOME | PSP_CTRL_START)

void ClearCaches()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}

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

int WriteFile(char *path, void *buf, int size)
{
	SceUID fd = sceIoOpen(path, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (fd < 0)
		return fd;

	int w = sceIoWrite(fd, buf, size);
	sceIoClose(fd);

	return w;
}

void Unassign()
{
	sceIoUnassign("flash0:");
	sceIoUnassign("flash1:");
}

void ErrorExit(u32 milisecs, char *fmt, ...)
{
	char str[256];

	va_list list;
		
	va_start(list, fmt);
	vsprintf(str, fmt, list);
	va_end(list);

	pspDebugScreenPuts(str);

	Unassign();
	sceIoDelDrv("flashfat");
	sceIoDelDrv("lflash");
	
	sceKernelDelayThread(milisecs*1000);

	sceSysconPowerStandby();

	while (1);
}

void FlashLed()
{
	sceLedSetMode(current_led, 2, led_mode);	
	current_led = !current_led;
}

void FlashFile(char *file, void *buf, int size)
{
	printf("Flashing file %s (%d)... ", file, size);

	int res = WriteFile(file, buf, size);
	if (res != size)
	{
		ErrorExit(5000, "Failed -> 0x%08X\n", res);
	}

	printf("OK\n");
}

void CreateDirs()
{
	printf("Creating directories... ");
	
	sceIoMkdir("flash0:/data", 0777);
	sceIoMkdir("flash0:/data/cert", 0777);
	sceIoMkdir("flash0:/dic", 0777);
	sceIoMkdir("flash0:/font", 0777);
	sceIoMkdir("flash0:/kd", 0777);
	sceIoMkdir("flash0:/vsh", 0777);
	sceIoMkdir("flash0:/kd/resource", 0777);
	sceIoMkdir("flash0:/vsh/etc", 0777);
	sceIoMkdir("flash0:/vsh/module", 0777);
	sceIoMkdir("flash0:/vsh/resource", 0777);

	printf("OK\n");
}

void LoadModules(int original)
{
	int res;
	
	printf("Loading updater modules... ");

	res = pspSdkLoadStartModule("ms0:/TM/DC5/kd/nand_updater.prx", PSP_MEMORY_PARTITION_KERNEL);

	if (res < 0)
	{
		ErrorExit(5000, "Failed -> Error %08X loading module nand_updater\n", res);
	}

	
	if (!original && !psp_slim)
	{
		u32 *mod = (u32 *)sceKernelFindModuleByName("sceNAND_Updater_Driver");
		u32 text_addr = *(mod+27);
		_sh(0xac60, text_addr+0x0E7E);
		ClearCaches();
	}

	res = pspSdkLoadStartModule("ms0:/TM/DC5/kd/lfatfs_updater.prx", PSP_MEMORY_PARTITION_KERNEL);

	if (res < 0)
	{
		ErrorExit(5000, "Failed ->  Error %08X loading module lfatfs_updater\n", res);
	}


	res = pspSdkLoadStartModule("ms0:/TM/DC5/kd/lflash_fatfmt_updater.prx", PSP_MEMORY_PARTITION_KERNEL);

	if (res < 0)
	{
		ErrorExit(5000, "Failed -> Error %08X loading module lflash_fatfmt_updater\n", res);
	}


	res = pspSdkLoadStartModule("ms0:/TM/DC5/kd/libpsardumper.prx", PSP_MEMORY_PARTITION_KERNEL);
	
	if (res < 0)
	{
		ErrorExit(5000, "Failed -> Error %08X loading module libpsardumper\n", res);
	}

	

	res = pspSdkLoadStartModule("ms0:/TM/DC5/kd/pspdecrypt.prx", PSP_MEMORY_PARTITION_KERNEL);

	if (res < 0)
	{
		ErrorExit(5000, "Failed -> Error %08X loading module libpsardumper\n", res);
	}

	printf("OK\n");
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

void FlashPSAR(int original)
{
	u8 pbp_header[0x28];
	int cbFile;

	printf("Loading PSAR to RAM... ");

	if (ReadFile("ms0:/390.PBP", 0, pbp_header, sizeof(pbp_header)) != sizeof(pbp_header))
	{
		ErrorExit(5000, "Cannot find 390.PBP at root.\n");
	}

	cbFile = ReadFile("ms0:/390.PBP", *(u32 *)&pbp_header[0x24], g_dataPSAR, sizeof(g_dataPSAR));
	if (cbFile <= 0)
	{
		ErrorExit(5000, "Error Reading 390.PBP.\n");
	}

	if (pspPSARInit(g_dataPSAR, g_dataOut, g_dataOut2) < 0)
	{
		ErrorExit(5000, "pspPSARInit failed!.\n");
	}

	printf("OK\n");

	CreateDirs();

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

		if (cbExpanded > 0)
		{		
			if (!strncmp(name, "com:", 4) && comtable_size > 0)
			{
				if (!FindTablePath(com_table, comtable_size, name+4, name))
				{
					ErrorExit(5000, "Error: cannot find path of %s.\n", name);
				}
			}

			else if (!psp_slim && !strncmp(name, "01g:", 4) && _1gtable_size > 0)
			{
				if (!FindTablePath(_1g_table, _1gtable_size, name+4, name))
				{
					ErrorExit(5000, "Error: cannot find path of %s.\n", name);
				}
			}

			else if (psp_slim && !strncmp(name, "02g:", 4) && _2gtable_size > 0)
			{
				if (!FindTablePath(_2g_table, _2gtable_size, name+4, name))
				{
					ErrorExit(5000, "Error: cannot find path of %s.\n", name);
				}
			}

			if (!strncmp(name, "flash0:/", 8))
			{
				if (signcheck)
				{
					pspSignCheck(g_dataOut2);
				}
				
				FlashFile(name, g_dataOut2, cbExpanded);
			}
			else if (!strncmp(name, "ipl:", 4))
			{
				u8 *ipl;
				int ipl_size;
				
				printf("Flashing IPL... ");
				
				if (psp_slim)
				{
					cbExpanded = pspDecryptPRX(g_dataOut2, g_dataOut+16384, cbExpanded);
					if (cbExpanded <= 0)
					{
						ErrorExit(5000, "Cannot decrypt PSP Slim IPL.\n");
					}
					
					memcpy(g_dataOut, ipl_02g, 16384);
				}
				else
				{
					memcpy(g_dataOut+16384, g_dataOut2, cbExpanded);
					memcpy(g_dataOut, ipl_01g, 16384);
				}

				ipl = g_dataOut;
				ipl_size = cbExpanded+16384;

				if (original)
				{
					ipl += 16384;
					ipl_size -= 16384;
				}

				ClearCaches();

				sceNandLock(1);

				pspIplClearIpl();
				if (pspIplSetIpl(ipl, ipl_size) < 0)
				{
					ErrorExit(5000, "Error\n");
				}

				sceNandUnlock();

				printf("OK\n");
			}

			else if (!strcmp(name, "com:00000"))
			{
				comtable_size = pspDecryptTable(g_dataOut2, g_dataOut, cbExpanded);
							
				if (comtable_size <= 0)
				{
					ErrorExit(5000, "Cannot decrypt common table.\n");
				}

				memcpy(com_table, g_dataOut2, comtable_size);						
			}
					
			else if (!psp_slim && !strcmp(name, "01g:00000"))
			{
				_1gtable_size = pspDecryptTable(g_dataOut2, g_dataOut, cbExpanded);
							
				if (_1gtable_size <= 0)
				{
					ErrorExit(5000, "Cannot decrypt 1g table.\n");
				}

				memcpy(_1g_table, g_dataOut2, _1gtable_size);						
			}
					
			else if (psp_slim && !strcmp(name, "02g:00000"))
			{
				_2gtable_size = pspDecryptTable(g_dataOut2, g_dataOut, cbExpanded);
							
				if (_2gtable_size <= 0)
				{
					ErrorExit(5000, "Cannot decrypt 2g table %08X.\n", _2gtable_size);
				}

				memcpy(_2g_table, g_dataOut2, _2gtable_size);						
			}

			FlashLed();
		}
	}
}

void CreateFlash1Dirs()
{
	printf("Creating flash1 directories... ");
	
	sceIoMkdir("flash1:/dic", 0777);
	sceIoMkdir("flash1:/gps", 0777);
	sceIoMkdir("flash1:/net", 0777);
	sceIoMkdir("flash1:/net/http", 0777);
	sceIoMkdir("flash1:/registry", 0777);
	sceIoMkdir("flash1:/vsh", 0777);
	sceIoMkdir("flash1:/vsh/theme", 0777);

	printf("OK\n");
}

void RestoreRegistry()
{
	int size;

	printf("Copying registry... ");
	
	size = ReadFile("ms0:/TM/DC5/registry/system.dreg", 0, g_dataOut, sizeof(g_dataOut));
	if (size <= 0)
	{
		ErrorExit(5000, "Cannot read system.dreg\n");
	}

	if (WriteFile("flash1:/registry/system.dreg", g_dataOut, size) < size)
	{
		ErrorExit(5000, "Cannot write system.dreg\n");
	}

	size = ReadFile("ms0:/TM/DC5/registry/system.ireg", 0, g_dataOut, sizeof(g_dataOut));
	if (size <= 0)
	{
		ErrorExit(5000, "Cannot read system.ireg\n");
	}

	if (WriteFile("flash1:/registry/system.ireg", g_dataOut, size) < size)
	{
		ErrorExit(5000, "Cannot write system.ireg\n");		
	}

	printf("OK\n");
}

void Install(int original)
{
	int i, res;
	char *argv[2];
	
	sceKernelVolatileMemLock(0, (void *)&g_dataOut2, &res);	

	Unassign();
	LoadModules(original);

	sceKernelDelayThread(800000);

	printf("Formatting flash0... ");

	argv[0] = "fatfmt";
	argv[1] = "lflash0:0,0";

	if (sceLflashFatfmtStartFatfmt(2, argv) < 0)
	{
		ErrorExit(5000, "Error.\n");
	}

	printf("OK\n");

	printf("Formatting flash1... ");

	argv[1] = "lflash0:0,1";
	
	if (sceLflashFatfmtStartFatfmt(2, argv) < 0)
	{
		ErrorExit(5000, "Error.\n");
	}
	
	printf("OK\n");

	printf("Formatting flash2... ");

	argv[1] = "lflash0:0,2";
	
	if (sceLflashFatfmtStartFatfmt(2, argv) < 0)
	{
		printf("Error (not critical).\n");
	}
	else
	{	
		printf("OK\n");
	}

	printf("Reassigning flashes... ");

	res = sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDWR, NULL, 0);
	if (res < 0)
	{
		ErrorExit(5000, "Failed -> Flash0 assign failed 0x%08X\n", res);  
	}

	res = sceIoAssign("flash1:", "lflash0:0,1", "flashfat1:", IOASSIGN_RDWR, NULL, 0);
	if (res < 0)
	{
		ErrorExit(5000, "Failed -> Flash1 assign failed 0x%08X\n", res);  
	}

	res = sceIoAssign("flash2:", "lflash0:0,2", "flashfat2:", IOASSIGN_RDWR, NULL, 0);
	if (res < 0)
	{
		printf("Failed -> Flash2 assign failed 0x%08X (not critical)\n", res);  
	}
	else
	{
		printf("OK\n");
	}

	FlashPSAR(original);
	CreateFlash1Dirs();
	RestoreRegistry();

	if (!original)
	{
		M33File *files;
		
		printf("\n------------------------------------\n\n");
		printf("M33 files\n\n");

		if (psp_slim)
			files = m33_02g;
		else
			files = m33_01g;

		for (i = 0; i < N_FILES; i++)
		{
			FlashFile(files[i].path, files[i].buf, files[i].size);
			FlashLed();
		}		
		
		printf("Copying SE configuration... ");

		if (WriteFile("flash1:/config.se", config, sizeof(config)) != sizeof(config))
		{
			ErrorExit(5000, "Error\n");			
		}

		FlashLed();

		printf("OK\n");
	}

	res = ReadFile("ms0:/TM/DC5/registry/act.dat", 0, g_dataOut, sizeof(g_dataOut));
	if (res > 0)
	{
		printf("Restoring PSN DRM act.dat... ");
		res = WriteFile("flash2:/act.dat", g_dataOut, res);
		if (res < 0)
		{
			printf(" error 0x%08X\n", res);
		}
		else
		{
			printf("OK\n");
		}
	}

	sceIoDevctl("flash0:", 0x5802, NULL, 0, NULL, 0);
	sceIoDevctl("flash1:", 0x5802, NULL, 0, NULL, 0);
	sceIoDevctl("flash2:", 0x5802, NULL, 0, NULL, 0);

	printf("\nInstall success.\n");	
}

u8 user[32*512];
u8 spare[16*32];

int WriteBlock(u32 ppn, u8 *u, u8 *s)
{
	int i;

	for (i = 0; i < 32; i++)
	{
		sceNandWritePagesRawAll(ppn, u, s, 1);
		ppn++;
		u += 512;
		s += 16;
	}

	return 0;
}

void RestoreNand()
{
	u8 *p, *q, *r;
	u32 ppn = 0;
	int i, j, k;

	printf("Restoring nand... ");

	SceUID fd = sceIoOpen("ms0:/nand-dump.bin", PSP_O_RDONLY, 0);

	if (fd < 0)
	{
		ErrorExit(5000, "Cannot open nand-dump.bin.\n");		
	}

	sceNandLock(1);

	for (i = 0; i < (sceNandGetTotalBlocks() / 1024); i++)
	{
		if (sceIoRead(fd, g_dataPSAR, 16896*1024) != (16896*1024))
		{
			ErrorExit(5000, "Error in read.\n");			
		}

		p = g_dataPSAR;

		for (j = 0; j < 1024; j++)
		{
			q = user;
			r = spare;
			
			for (k = 0; k < 32; k++)
			{
				memcpy(q, p, 512);
				memcpy(r, p+512, 16);

				p += 528;
				q += 512;
				r += 16;
			}
			
			if (1)
			{
				if (sceNandEraseBlock(ppn) >= 0)
				{
					//Printf("Writing block 0x%08X\n", ppn);
					
					if (WriteBlock(ppn, user, spare) < 0)
						printf("Error writing block 0x%08X\n", ppn);
				}
				else
				{
					printf("Error erasing block ppn=0x%08X\n", ppn);
				}

				FlashLed();
			}

			ppn += 32;
		}
	}

	sceNandUnlock();

	printf("Nand write complete.\n");	
}

int main()
{
	SceCtrlData pad;
	u32 tachyon, baryon, pommel;
	u32 digitalkey;
	int sysconver;
	char mb[3];
	//u8 mac[6];

	tachyon = 0;
	baryon = 0;
	pommel = 0;
	digitalkey = 0;
	
	tachyon = sceSysregGetTachyonVersion();
	
	if (sceSysconGetBaryonVersion(&baryon) < 0)
		baryon = 0xDADADADA;

	if (sceSysconGetPommelVersion(&pommel) < 0)
		pommel = 0xDADADADA;

	if (sceSysconGetDigitalKey(&digitalkey) < 0)
		digitalkey = 0xDADA;

	psp_slim = (tachyon >= 0x00500000);	

	if (!psp_slim) sceDisplaySetBrightness(99, 0);
	
	strcpy(mb, "??");
	
	sysconver = (baryon>>16)&0xFF;
	if ((sysconver&0xF0) == 0x00)
	{
		switch(sysconver&0xF)
		{
			case 0 :
			case 1 :
			case 2 :
			case 3 : strcpy(mb, "79");
            break;
			default: strcpy(mb, "81");
			break;
		}
	}

	if ((sysconver&0xF0) == 0x10)
	{
		switch(sysconver&0xF)
		{
			case 0 :
			case 1 : strcpy(mb, "82");
			break;
			default: strcpy(mb, "86");
			break;
	  }
	}
	
	if ((sysconver&0xF0) == 0x20)
	{
		switch(sysconver&0xF)
		{
			case 0 :
			case 1 :
			case 2 : strcpy(mb, "85");
            break;
			default: strcpy(mb, "??"); // ?? the new shit

            break;
		}
	}

	pspDebugScreenInit();

	/*if (sceIdStorageLookup(0x44, 0, mac, 6) >= 0)
	{
		printf("MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	}
	else
	{
		printf("Error in idstorage.\n");
	}*/

	printf("Tachyon = 0x%08X, Baryon=0x%08X\n", tachyon, baryon);
	printf("Pommel = 0x%08X, Syscon DK =0x%04X\n", pommel, digitalkey);
	printf("Nand size = %d\n", sceNandGetTotalBlocks() * sceNandGetPagesPerBlock() * sceNandGetPageSize());
	printf("Model: %s (TA-%s)\n\n", (psp_slim) ? "PSP Slim" : " PSP Standard", mb);

	pspSdkInstallNoDeviceCheckPatch();	

	printf("Press X to install 3.90 M33\n");
	printf("Press O to install original 3.90\n");
	printf("Press [] to dump the nand\n");
	printf("Press L+R+start+home to restore nand dump phisically (dangerous!)\n\n");

	sceLedSetMode(0, 2, led_mode);

	sceKernelDelayThread(1*1000*1000);

	sceLedSetMode(1, 2, led_mode);
	sceKernelDelayThread(1*1000*1000);

	while (1)
	{
		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_CROSS)
		{
			Install(0);
			break;
		}

		else if (pad.Buttons & PSP_CTRL_CIRCLE)
		{
			Install(1);
			break;
		}

		else if (pad.Buttons & PSP_CTRL_SQUARE)
		{
			struct SceKernelLoadExecParam param;

			memset(&param, 0, sizeof(param));
			param.size = sizeof(param);
			param.key = "game";
			
			sceKernelLoadExec("ms0:/TM/DC5/kd/backup.elf", &param);
		}

		else if ((pad.Buttons & RESTORE_BUTTONS) == RESTORE_BUTTONS)
		{
			RestoreNand();
			break;
		}

		sceKernelDelayThread(10000);
	}


	pspDebugScreenPuts("Press X to shutdown the psp.\n");

	while (1)
	{
		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_CROSS)
		{
			break;
		}

		sceKernelDelayThread(10000);
	}
	
	ErrorExit(300, "");	

	return 0;

}

