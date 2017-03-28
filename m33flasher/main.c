#include <pspsdk.h>
#include <pspkernel.h>
#include <psploadexec.h>
#include <psputils.h>
#include <pspctrl.h>
#include <psppower.h>
#include <psplflash_fatfmt.h>
#include <pspsuspend.h>
#include <psprtc.h>
#include <psputility_sysparam.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>

#include "psar.h"
#include "conf.h"

PSP_MODULE_INFO("P-psarUpdate", 0x1000, 1, 0);

PSP_MAIN_THREAD_ATTR(0);

#define printf pspDebugScreenPrintf

u8 *dataPSAR;
u8 *dataOut, *dataOut2;

int psarSize;

#define EXTRACT_PRXS	"ms0:/PSP/GAME/UPDATE/extprxs.pbp"

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

int LoadStartModule(char *module)
{
	SceUID mod = sceKernelLoadModule(module, 0, NULL);

	if (mod < 0)
		return mod;

	return sceKernelStartModule(mod, strlen(module)+1, module, NULL, NULL);
}

int WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (fd < 0)
	{
		return -1;
	}

	int written = sceIoWrite(fd, buf, size);
	
	if (sceIoClose(fd) < 0)
		return -1;

	return written;
}

int ByPass()
{
	SceCtrlData pad;
	
	sceCtrlReadBufferPositive(&pad, 1);

	if (pad.Buttons & PSP_CTRL_LTRIGGER)
	{
		if (pad.Buttons & PSP_CTRL_TRIANGLE)
			return 1;
	}

	return 0;
}

void LoadModules()
{
	SceUID mod;

	mod = LoadStartModule("flash0:/kd/lflash_fatfmt.prx");
	if (mod < 0)
	{
		ErrorExit(6000, "Error %08X loading/starting lflash_fatfmt.prx\n", mod);
	}	
}

u8 md5_psar[16] = 
{
	0xC4, 0x5E, 0xBF, 0x84, 0x65, 0x2E, 0x9E, 0xBC, 
	0x4E, 0xD6, 0x25, 0x1B, 0x65, 0xAC, 0x46, 0xCC
};

void ReadPSAR(char *file)
{
	u8 md5[16];
	
	printf("Loading PSAR to RAM... ");
	
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0777);
	u32 header[10];	
		
	if (fd < 0)
	{
		ErrorExit(6000, "Error opening EBOOT.PBP.\n");
	}

	sceIoRead(fd, header, sizeof(header));

	if (header[0] != 0x50425000)
	{
		ErrorExit(6000, "No a valid PBP file.\n");
	}

	sceIoLseek(fd, header[9], PSP_SEEK_SET);
	psarSize = sceIoRead(fd, dataPSAR, 19000000);

	if (psarSize <= 0)
	{
		ErrorExit(6000, "Read error or corrupted file.\n");
	}

	printf ("OK.\n");	

	printf("Checking psar integrity... ");

	sceKernelUtilsMd5Digest(dataPSAR, psarSize, md5);

	if (memcmp(md5, md5_psar, 16) != 0)
	{
		ErrorExit(6000, "Incorrect psar.\n");
	}

	printf("OK.\n");

	if (pspPSARInit(dataPSAR, dataOut, dataOut2) < 0)
	{
		ErrorExit(6000, "Corrupted or unsupported PSAR data\n");
	}

	sceIoClose(fd);	
}

int CreateDirs()
{
	if (sceIoMkdir("flash0:/data", 0777) < 0)
		return -1;

	if (sceIoMkdir("flash0:/dic", 0777) < 0)
		return -1;

	if (sceIoMkdir("flash0:/font", 0777) < 0)
		return -1;

	if (sceIoMkdir("flash0:/kd", 0777) < 0)
		return -1;

	if (sceIoMkdir("flash0:/kn", 0777) < 0)
		return -1;

	if (sceIoMkdir("flash0:/vsh", 0777) < 0)
		return -1;

	if (sceIoMkdir("flash0:/data/cert", 0777) < 0)
		return -1;

	if (sceIoMkdir("flash0:/kd/resource", 0777) < 0)
		return -1;

	if (sceIoMkdir("flash0:/kn/resource", 0777) < 0)
		return -1;

	if (sceIoMkdir("flash0:/vsh/etc", 0777) < 0)
		return -1;

	if (sceIoMkdir("flash0:/vsh/module", 0777) < 0)
		return  -1;

	if (sceIoMkdir("flash0:/vsh/nodule", 0777) < 0)
		return -1;

	if (sceIoMkdir("flash0:/vsh/resource", 0777) < 0)
		return -1;

	return 0;
}

void CreateFlash1Dirs()
{
	sceIoMkdir("flash1:/dic", 0777);
	sceIoMkdir("flash1:/gps", 0777);
	sceIoMkdir("flash1:/net", 0777);
	sceIoMkdir("flash1:/net/http", 0777);
	sceIoMkdir("flash1:/registry", 0777);
	sceIoMkdir("flash1:/vsh", 0777);
	sceIoMkdir("flash1:/vsh/theme", 0777);	
}

void SetPercentage(int x, int y, int value, int max, int dv)
{
	if (dv)
	{
		value /= dv;
		max /= dv;
	}

	pspDebugScreenSetXY(x, y);
	printf("%3d%%", ((100 * value) / max));
}

void WriteConfig()
{
	SEConfig config;

	memset(&config, 0, sizeof(config));
	SE_GetConfig(&config);
	
	config.startupprog = 0;	
	SE_SetConfig(&config);
}

void DisablePlugins()
{
	u8 conf[15];

	memset(conf, 0, 15);
	
	WriteFile("ms0:/seplugins/conf.bin", conf, 15);
}

char name[128];

int formatflash2=0;

void Update()
{
	char *argv[2];
	int x, y;

	if (sceIoUnassign("flash0:") < 0)
	{
		ErrorExit(6000, "ERROR: unassigning flash0.\n");		
	}

	if (sceIoUnassign("flash1:") < 0)
	{
		ErrorExit(6000, "ERROR: unassigning flash1.\n");		
	}

	printf("Formating flash0...");
	
	argv[0] = "fatfmt";
	argv[1] = "lflash0:0,0";

	if (sceLflashFatfmtStartFatfmt(2, argv) < 0)
	{
		ErrorExit(6000, "PANIC: Error formating flash0.\n");
	}

	printf(" OK.\n");

	if (formatflash2)
	{
		printf("Formating flash2...");
		
		argv[0] = "fatfmt";
		argv[1] = "lflash0:0,2";

		if (sceLflashFatfmtStartFatfmt(2, argv) < 0)
		{
			printf(" Warning: Error formatting  flash2.\n");
		}
		else
		{
			printf(" OK.\n");
		}
	}	

	if (sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDWR, NULL, 0) < 0)
	{
		ErrorExit(6000, "PANIC: error re-assigning flash0.\n");
	}

	if (sceIoAssign("flash1:", "lflash0:0,1", "flashfat1:", IOASSIGN_RDWR, NULL, 0) < 0)
	{
		printf("Error re-assigning flash1.\n");
	}

	printf("Creating directories...");

	if (CreateDirs() < 0)
	{
		printf("PANIC: error creating directories.\n");
	}

	printf(" OK.\n");

	printf("Writing files...");

	x = pspDebugScreenGetX();
	y = pspDebugScreenGetY();
	printf("\n");

	while (1)
	{
		int filesize;
		int pos;

		int res = pspPSARGetNextFile(dataPSAR, psarSize, dataOut, dataOut2, name, &filesize, &pos);
		
		if (res < 0)
		{
			ErrorExit(6000, "PANIC: PSAR error.\n");
		}
		else if (res == 0) /* no more files */
		{
			SetPercentage(x, y, 1, 1, 0);
			break;
		}

		if (filesize > 0)
		{
			if (filesize >= (2*1024*1024))
				printf("fs.\n");
			
			if (strncmp(name, "flash0:", 7) == 0)
			{
				if (WriteFile(name, dataOut2, filesize) < 0)
					printf("PANIC: error writing file %s.\n", name);
				
				sceIoSync("flash0:", 0x10);				
			}
		}
	
		SetPercentage(x, y, pos, psarSize, 100);	
		scePowerTick(0);
	}

	printf("\n");
}

void Agreement()
{
	SceCtrlData pad;
	SceCtrlData oldpad;

	printf("You are about to update your PSP to 3.52-M33.\n");
	printf("Press X to start the flashing process, accepting that "
	       "authors aren'tresponsable of the consequences this process may cause on your unit.\n");
	
	printf("If you don't agree, press R button to exit.\n");

	oldpad.Buttons = 0;
	
	while (1)
	{
		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons != oldpad.Buttons)
		{
			if (pad.Buttons & PSP_CTRL_CROSS)
			{
				return;			
			}

			else if (pad.Buttons & PSP_CTRL_RTRIGGER)
			{
				sceKernelExitGame();
			}
		}

		oldpad.Buttons = pad.Buttons;
		sceKernelDelayThread(10000);
	}
}

void Reboot()
{
	SceCtrlData pad;
	SceCtrlData oldpad;

	oldpad.Buttons = 0;
	printf("Press X to shut down the psp.\n");

	while (1)
	{
		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons != oldpad.Buttons)
		{
			if (pad.Buttons & PSP_CTRL_CROSS)
				break;
		}

		oldpad.Buttons = pad.Buttons;
		sceKernelDelayThread(10000);
	}

	sceSysconPowerStandby();
}

void SetDate()
{
	sceKernelDelayThread(4000000);
}

void CheckFlash2()
{
	if (sceIoAssign("flash2:", "lflash0:0,2", "flashfat2:", IOASSIGN_RDWR, NULL, 0) < 0)
	{
		formatflash2 = 1;
		return;
	}

	sceIoUnassign("flash2:");
}

void CopyFlashAct()
{
	SceUID fd = sceIoOpen("flash1:/act.dat", PSP_O_RDONLY, 0);

	if (fd >= 0)
	{
		int read = sceIoRead(fd, dataOut, 4152);
		sceIoClose(fd);

		if (read == 4152)
		{
			if (sceIoAssign("flash2:", "lflash0:0,2", "flashfat2:", IOASSIGN_RDWR, NULL, 0) >= 0)
			{
				if (WriteFile("flash2:/act.dat", dataOut, 4152) == 4152)
				{
					sceIoRemove("flash1:/act.dat");
				}

				sceIoUnassign("flash2:");
			}
		}
	}
}

int main(int argc, char *argv[])
{
	int x;
	
	pspDebugScreenInit();
	
	if (scePowerGetBatteryLifePercent() < 75)
	{
		if (!ByPass())
		{
			ErrorExit(6000, "Battery has to be at least at 75%%.\n");
		}
	}

	dataPSAR = (u8 *)memalign(0x40, 19000000);
	if (!dataPSAR)
	{
		ErrorExit(6000, "Error alocating psar buffer.\n");
	}

	if (sceKernelVolatileMemLock(0, (void *)&dataOut, &x) != 0)
	{
		ErrorExit(6000, "Error allocating 2 buffer.\n");
	}	

	dataOut2 = dataOut+(2*1024*1024);

	//formatflash2 = 1;

	Agreement();
	printf("\n");
	LoadModules();
	ReadPSAR(argv[0]);
	CheckFlash2();
	Update();
	CopyFlashAct();

	printf("Creating flash1 directories... ");
	CreateFlash1Dirs();
	printf("OK.\n");

	printf("Writing config... ");
	WriteConfig();
	printf("OK.\n");

	printf("Disabling plugins... ");
	DisablePlugins();
	printf("OK.\n");

	printf("\nSetting the date when the daylight shined in the sky of the night.\n"
		   "March 33, 2007; 3:33:33 am... ");
	SetDate();
	printf("nah, changing back the date later is abit annoying.\n");

	printf("\nProcess finished.\n");
	Reboot();

	return 0;
}
