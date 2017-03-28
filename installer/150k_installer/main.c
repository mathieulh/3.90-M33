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
#include <malloc.h>

#include <libpsardumper.h>
#include <pspdecrypt.h>
#include <kubridge.h>
#include <zlib.h>

#include <pspipl_update.h>

#include "pspbtcnf.h"
#include "systemctrl150.h"
#include "reboot150.h"
#include "ipl.h"

PSP_MODULE_INFO("legacy150_installer", 0x0800, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VSH);

#define printf    pspDebugScreenPrintf

#define PSAR_SIZE_150		10149440
#define UPDATER_PRX_SIZE	3737904
#define IPL150_SIZE			225280
#define IPL150_LIN			213488
#define NEWIPL_SIZE			245760


#define N_FILES	4

typedef struct
{
	char *path;
	u8 *buf;
	int size;
} M33File;

M33File m33files[N_FILES] =
{
	{ "flash0:/kd/reboot150.prx", reboot150, sizeof(reboot150) },
	{ "flash0:/km/systemctrl150.prx", systemctrl150, sizeof(systemctrl150) },
	{ "flash0:/km/pspbtcnf_game.txt", pspbtcnf_game, sizeof(pspbtcnf_game) },
	{ "flash0:/km/pspbtcnf_updater.txt", pspbtcnf_updater, sizeof(pspbtcnf_updater) },
};


////////////////////////////////////////////////////////////////////
// big buffers for data. Some system calls require 64 byte alignment

// big enough for the full PSAR file
static u8 g_dataPSAR[13000000] __attribute__((aligned(0x40)));; 

// big enough for the largest (multiple uses)
static u8 g_dataOut[2000000] __attribute__((aligned(0x40)));
   
// for deflate output
//u8 g_dataOut2[3000000] __attribute__((aligned(0x40)));
static u8 *g_dataOut2;   

static u8 ipl_buf[300*1024] __attribute__((aligned(0x40)));;

static char com_table[0x4000];
static int comtable_size;

static char _1g_table[0x4000];
static int _1gtable_size;

static char _2g_table[0x4000];
static int _2gtable_size;

u8 psar_150_md5[16] = 
{
	0xF5, 0x65, 0x54, 0xE2, 0xBE, 0x35, 0x64, 0x5A, 
	0x76, 0x0B, 0x18, 0xED, 0x65, 0x05, 0xC4, 0xCC
};

#define N_DELETE 13
#define N_150 109

char *todelete[N_DELETE] =
{
	"flash0:/vsh/module/lftv_main_plugin.prx",
	"flash0:/vsh/module/lftv_middleware.prx",
	"flash0:/vsh/module/lftv_plugin.prx",
	"flash0:/vsh/resource/lftv_main_plugin.rco",
	"flash0:/vsh/resource/lftv_rmc_univer3in1.rco",
	"flash0:/vsh/resource/lftv_rmc_univer3in1_jp.rco",
	"flash0:/vsh/resource/lftv_rmc_univerpanel.rco",
	"flash0:/vsh/resource/lftv_rmc_univerpanel_jp.rco",
	"flash0:/vsh/resource/lftv_rmc_univertuner.rco",
	"flash0:/vsh/resource/lftv_rmc_univertuner_jp.rco",
	"flash0:/vsh/resource/lftv_tuner_jp_jp.rco",
	"flash0:/vsh/resource/lftv_tuner_us_en.rco",
	"flash0:/font/kr0.pgf"
};

char *subset150[N_150] =
{	
	"flash0:/kd/ata.prx",
	"flash0:/kd/audio.prx",
	"flash0:/kd/audiocodec.prx",
	"flash0:/kd/blkdev.prx",
	"flash0:/kd/chkreg.prx",
	"flash0:/kd/clockgen.prx",
	"flash0:/kd/codec.prx",
	"flash0:/kd/ctrl.prx",
	"flash0:/kd/display.prx",
	"flash0:/kd/dmacman.prx",
	"flash0:/kd/dmacplus.prx",
	"flash0:/kd/emc_ddr.prx",
	"flash0:/kd/emc_sm.prx",
	"flash0:/kd/exceptionman.prx",
	"flash0:/kd/fatmsmod.prx",
	"flash0:/kd/ge.prx",
	"flash0:/kd/gpio.prx",
	"flash0:/kd/hpremote.prx",
	"flash0:/kd/i2c.prx",
	"flash0:/kd/idstorage.prx",
	"flash0:/kd/ifhandle.prx",
	"flash0:/kd/impose.prx",
	"flash0:/kd/init.prx",
	"flash0:/kd/interruptman.prx",
	"flash0:/kd/iofilemgr.prx",
	"flash0:/kd/isofs.prx",
	"flash0:/kd/lcdc.prx",
	"flash0:/kd/led.prx",
	"flash0:/kd/lfatfs.prx",
	"flash0:/kd/lflash_fatfmt.prx",
	"flash0:/kd/libatrac3plus.prx",
	"flash0:/kd/libhttp.prx",
	"flash0:/kd/libparse_http.prx",
	"flash0:/kd/libparse_uri.prx",
	"flash0:/kd/libupdown.prx",
	"flash0:/kd/loadcore.prx",
	"flash0:/kd/loadexec.prx",
	"flash0:/kd/me_for_vsh.prx",
	"flash0:/kd/me_wrapper.prx",
	"flash0:/kd/mebooter.prx",
	"flash0:/kd/mediaman.prx",
	"flash0:/kd/mediasync.prx",
	"flash0:/kd/memab.prx",
	"flash0:/kd/memlmd.prx",
	"flash0:/kd/mesg_led.prx",
	"flash0:/kd/mgr.prx",
	"flash0:/kd/modulemgr.prx",
	"flash0:/kd/mpeg_vsh.prx",
	"flash0:/kd/mpegbase.prx",
	"flash0:/kd/msaudio.prx",
	"flash0:/kd/mscm.prx",
	"flash0:/kd/msstor.prx",
	"flash0:/kd/openpsid.prx",
	"flash0:/kd/peq.prx",
	"flash0:/kd/power.prx",
	//"flash0:/kd/pspbtcnf.txt",
	//"flash0:/kd/pspbtcnf_game.txt",
	//"flash0:/kd/pspbtcnf_updater.txt",
	"flash0:/kd/pspcnf_tbl.txt",
	"flash0:/kd/pspnet.prx",
	"flash0:/kd/pspnet_adhoc.prx",
	"flash0:/kd/pspnet_adhoc_auth.prx",
	"flash0:/kd/pspnet_adhoc_download.prx",
	"flash0:/kd/pspnet_adhoc_matching.prx",
	"flash0:/kd/pspnet_adhocctl.prx",
	"flash0:/kd/pspnet_ap_dialog_dummy.prx",
	"flash0:/kd/pspnet_apctl.prx",
	"flash0:/kd/pspnet_inet.prx",
	"flash0:/kd/pspnet_resolver.prx",
	"flash0:/kd/pwm.prx",
	"flash0:/kd/registry.prx",
	"flash0:/kd/resource/impose.rsc",
	"flash0:/kd/rtc.prx",
	"flash0:/kd/semawm.prx",
	"flash0:/kd/sircs.prx",
	"flash0:/kd/stdio.prx",
	"flash0:/kd/sysclib.prx",
	"flash0:/kd/syscon.prx",
	"flash0:/kd/sysmem.prx",
	"flash0:/kd/sysreg.prx",
	"flash0:/kd/systimer.prx",
	"flash0:/kd/threadman.prx",
	"flash0:/kd/uart4.prx",
	"flash0:/kd/umd9660.prx",
	"flash0:/kd/umdman.prx",
	"flash0:/kd/usb.prx",
	"flash0:/kd/usbstor.prx",
	"flash0:/kd/usbstorboot.prx",
	"flash0:/kd/usbstormgr.prx",
	"flash0:/kd/usbstorms.prx",
	"flash0:/kd/usersystemlib.prx",
	"flash0:/kd/utility.prx",
	"flash0:/kd/utils.prx",
	"flash0:/kd/vaudio.prx",
	"flash0:/kd/vaudio_game.prx",
	"flash0:/kd/videocodec.prx",
	"flash0:/kd/vshbridge.prx",
	"flash0:/kd/wlan.prx",
	"flash0:/vsh/module/chnnlsv.prx",
	"flash0:/vsh/module/common_gui.prx",
	"flash0:/vsh/module/common_util.prx",
	"flash0:/vsh/module/dialogmain.prx",
	"flash0:/vsh/module/heaparea1.prx",
	"flash0:/vsh/module/heaparea2.prx",
	"flash0:/vsh/module/netconf_plugin.prx",
	"flash0:/vsh/module/netplay_client_plugin.prx",
	"flash0:/vsh/module/netplay_server_utility.prx",
	"flash0:/vsh/module/osk_plugin.prx",
	//"flash0:/vsh/module/paf.prx",
	"flash0:/vsh/module/pafmini.prx",
	"flash0:/vsh/module/savedata_auto_dialog.prx",
	"flash0:/vsh/module/savedata_plugin.prx",
	"flash0:/vsh/module/savedata_utility.prx",
	//"flash0:/vsh/module/vshmain.prx"	
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

z_stream z;

static void *t_malloc(void *u, unsigned int items, unsigned int size)
{
	SceUID uid = sceKernelAllocPartitionMemory(PSP_MEMORY_PARTITION_USER, "", PSP_SMEM_Low, items*size, NULL);

	if (uid < 0)
	{
		printf("Error in partition memory.\n");
		return NULL;
	}

	return sceKernelGetBlockHeadAddr(uid);
}

static void t_free()
{
}

int deflateCompress(void *inbuf, int insize, void *outbuf, int outsize)
{
	int res;
	int size;
	
	z.zalloc = t_malloc;
	z.zfree  = t_free;
	z.opaque = Z_NULL;

	res = deflateInit2(&z, Z_DEFAULT_COMPRESSION , Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY);
	if (res != Z_OK)
		return res;

	z.next_out  = outbuf;
	z.avail_out = outsize;
	z.next_in   = inbuf;
	z.avail_in  = insize;

	res = deflate(&z, Z_FINISH);
	if (res != Z_STREAM_END)
	{
		return res;
	}

	size = outsize - z.avail_out;

	res = deflateEnd(&z);
	if (res != Z_OK)
		return res;

	return size;
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

static int WritePrx(char *name)
{
	int i;

	for (i = 0; i < N_150; i++)
	{
		if (strcmp(name, subset150[i]) == 0)
		{
			return 1;
		}
	}
	
	return 0;
}

static void CorrectPath(char *path)
{
	if (strncmp(path, "flash0:/kd", 10) == 0)
	{
		path[9] = 'm';
	}
	else if (strncmp(path, "flash0:/vsh/module", 18) == 0)
	{
		path[12] = 'p';
	}
}

int LoadStartModule(char *module, int partition)
{
	SceUID mod = sceKernelLoadModule(module, 0, NULL);

	if (mod < 0)
		return mod;

	return sceKernelStartModule(mod, 0, NULL, NULL, NULL);
}

int GetReboot(u8 *dataOut, u8 *dataOut2, int cbExpanded, int decompress)
{
	cbExpanded = pspDecryptPRX(dataOut, dataOut2, cbExpanded);
	if (cbExpanded <= 0)
	{
		ErrorExit(5000, "Cannot decrypt loadexec.\n");
	}

	cbExpanded = pspDecompress(dataOut2, dataOut, sizeof(g_dataOut));
	if (cbExpanded <= 0)
	{
		ErrorExit(5000, "Cannot decompress loadexec.\n");
	}

	int i;

	for (i = 0; i < cbExpanded-20; i++)
	{
		if (memcmp(dataOut+i, "~PSP", 4) == 0)
		{
			break;
		}
	}

	if (i == (cbExpanded-20))
	{
		ErrorExit(5000, "Cannot find reboot.bin inside loadexec.\n");
	}

	cbExpanded = pspDecryptPRX(dataOut+i, dataOut2, *(u32 *)&dataOut[i+0x2C]);
	if (cbExpanded <= 0)
	{
		ErrorExit(5000, "Cannot decrypt reboot.bin.\n");
	}

	if (decompress)
	{
		cbExpanded = pspDecompress(dataOut2, dataOut, sizeof(g_dataOut));
		if (cbExpanded <= 0)
		{
			ErrorExit(5000, "Cannot decompress reboot.bin (0x%08X)\n", cbExpanded);
		}	
	}
	
	return cbExpanded;
}

void ExtractPrxs(int cbFile)
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
			if (WritePrx(name))
			{				
				CorrectPath(name);

				printf("Flashing %s (%d)... ", name, cbExpanded);
				if (WriteFile(name, g_dataOut2, cbExpanded) != cbExpanded)
				{
					ErrorExit(5000, "Error\n");
				}

				printf("OK\n");

				if (strcmp(name, "flash0:/km/loadexec.prx") == 0)
				{
					cbExpanded = GetReboot(g_dataOut2, g_dataOut, cbExpanded, 0);
					memcpy(reboot150+0x71C, g_dataOut, cbExpanded);
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



static void DeleteLFTV()
{
	int res;
	int i;

	for (i = 0; i < N_DELETE; i++)
	{
		printf("Deleting %s... ", todelete[i]);
		
		res = sceIoRemove(todelete[i]);
		if (res < 0)
		{
			if (res == 0x80010002)
				printf("doesn't exist\n");
			else
				ErrorExit(5000, "Error 0x%08X in remove.\n", res);
		}
		else
			printf("OK\n");
	}

	res = ReadFile("flash0:/font/ltn9.pgf", 0, g_dataOut, sizeof(g_dataOut));
	if (res <= 0)
	{
		ErrorExit(5000, "Error reading lt9.pgf\n");
	}

	if (WriteFile("flash0:/font/kr0.pgf", g_dataOut, res) != res)
	{
		ErrorExit(5000, "Error writing kr0.pgf\n");
	}
}

static void CreateDirs()
{
	printf("Creating directories... ");
	sceIoMkdir("flash0:/km", 0777);
	sceIoMkdir("flash0:/km/resource", 0777);
	sceIoMkdir("flash0:/vsh/podule", 0777);
	printf("OK\n");
}

static void Update()
{
	u8 md5[16];
	int size;

	printf("Reading 1.50 psar... ");
	
	if (ReadFile("ms0:/150.PBP", 0x393B81, g_dataPSAR, PSAR_SIZE_150) != PSAR_SIZE_150)
    {
        ErrorExit(5000, "Incorrect or inexistant 150.PBP at root.\n");
	}   
    
	sceKernelUtilsMd5Digest(g_dataPSAR, PSAR_SIZE_150, md5);

	if (memcmp(md5, psar_150_md5, 16) != 0)
	{
		ErrorExit(5000, "Incorrect or corrupted 150.PBP.\n");
	}

	printf("OK\n");

	printf("Reassigning flash0... ");

	if (sceIoUnassign("flash0:") < 0)
	{
		ErrorExit(5000, "Error in unassign.\n");
	}

	if (sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDWR, NULL, 0) < 0)
	{
		ErrorExit(5000, "Error in assign.\n");
	}

	printf("OK\n");

	CreateDirs();

	printf("\nDeleting Location Free Player and korean font...\n");
	DeleteLFTV();

	printf("\nExtracting and flashing 150 prx's...\n");
	ExtractPrxs(PSAR_SIZE_150);

	size = ReadFile("flash0:/kd/loadexec.prx", 0, g_dataOut, sizeof(g_dataOut));
	if (size <= 0)
	{
		ErrorExit(5000, "Cannot read loadexec from flash.\n");
	}

	pspUnsignCheck(g_dataOut);
	size = GetReboot(g_dataOut, g_dataOut2, size, 1);

	//printf("size = %d.\n", size);

	size = deflateCompress(g_dataOut, size, g_dataOut2, sizeof(g_dataOut));
	if (size <= 0)
	{
		ErrorExit(5000, "Error %d in zlib.\n", size);
	}

	//WriteFile("ms0:/reboot380.def", g_dataOut2, size);
	memcpy(systemctrl150+0x278C, g_dataOut2, size);
}

void BuildFlashIpl()
{
	int i, size;
	
	size = pspIplUpdateGetIpl(ipl_buf);
	if (size < 0)
	{
		ErrorExit(5000, "Cannot read nand ipl.\n");
	}

	memcpy(ipl_buf, ipl, 16384);

	if (ReadFile("ms0:/150.PBP", 0x3251, g_dataPSAR, UPDATER_PRX_SIZE) != UPDATER_PRX_SIZE)
    {
        ErrorExit(5000, "Incorrect or inexistant 150.PBP at root.\n");
	}  

	size = pspDecryptPRX(g_dataPSAR, g_dataPSAR, UPDATER_PRX_SIZE);
	if (size <= 0)
	{
		ErrorExit(5000, "Cannot decrypt updater prx.\n");
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
		ErrorExit(5000, "Error decrypting ipl.\n");
	}

	if (pspLinearizeIPL2(g_dataOut, g_dataOut2, IPL150_SIZE) != IPL150_LIN)
	{
		ErrorExit(5000, "Error in linearize.\n");
	}

	u32 *code = (u32 *)(g_dataOut2);
	code[0xE0/4] = 0x09038000; // j 0x040e0000
	code[0xE4/4] = 0; // nop

	memcpy(ipl_buf+(176*1024), g_dataOut2, 0x10000);

	if (pspIplUpdateClearIpl() < 0)
	{
		ErrorExit(5000, "Error in clear IPL.\n");
	}

	if (pspIplUpdateSetIpl(ipl_buf, NEWIPL_SIZE) < 0)
	{
		ErrorExit(5000, "Error writing double IPL.\n");
	}
}

int main(void)
{
    int i;
	
	pspDebugScreenInit();

	if (sceKernelDevkitVersion() != 0x03090010)
	{
		ErrorExit(5000, "This program only for 3.90 M33\n");
	}

	if (kuKernelGetModel() != PSP_MODEL_STANDARD)
	{
		ErrorExit(5000, "This program only for the standard psp.\n");
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

	mod = LoadStartModule("ipl_update.prx", PSP_MEMORY_PARTITION_KERNEL);
	if (mod < 0)
	{
		ErrorExit(5000, "Error 0x%08X loading/starting ipl_update.prx.\n", mod);
	}

    printf("Press cross to begin the installation of 1.50 subset, or R to exit.\n"
		   "Warning: this will delete Location Free Player and korean font.\n\n");

	while (1)
	{
		SceCtrlData pad;

		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_CROSS)
		{
			break;
		}
		else if (pad.Buttons & PSP_CTRL_RTRIGGER)
		{
			ErrorExit(5000, "Cancelled by user.\n");
		}
		sceKernelDelayThread(10000);
	}

	sceKernelVolatileMemLock(0, (void *)&g_dataOut2, &i);

	BuildFlashIpl();
	Update();	

	printf("Writing custom modules...\n");

	for (i = 0; i < N_FILES; i++)
	{
		printf("Flashing %s... ", m33files[i].path);
		
		if (WriteFile(m33files[i].path, m33files[i].buf, m33files[i].size) != m33files[i].size)
		{
			ErrorExit(5000, "Error.\n");
		}

		printf("OK\n");
	}	

	ErrorExit(7000, "\n\nDone.\nAuto-exiting in 7 seconds.\n");

    return 0;
}

