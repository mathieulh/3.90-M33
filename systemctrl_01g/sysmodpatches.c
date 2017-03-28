#include <pspsdk.h>
#include <pspkernel.h>
#include <pspinit.h>
#include <pspcrypt.h>
#include <psploadexec_kernel.h>
#include <psputilsforkernel.h>
#include <psppower.h>
#include <pspreg.h>
#include <pspmediaman.h>
#include <pspsysevent.h>
#include <psploadcore.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <oe_malloc.h>

#include "sysmodpatches.h"
#include "main.h"
#include "reboot.h"
#include <umd9660_driver.h>
#include <isofs_driver.h>
//#include "virtualpbpmgr.h"
#include "systemctrl.h"
#include "systemctrl_se.h"

SEConfig config;
int bootfile;
int dummy[0x1C/4];
int booted150;


void SetConfig(SEConfig *newconfig)
{
	memcpy(&config, newconfig, sizeof(SEConfig));
}

void sctrlSESetBootConfFileIndex(int n)
{
	bootfile = n;	
}

/*SEConfig *GetConfig()
{
	return &config;
}*/



u32 FindPowerFunction(u32 nid)
{
	return FindProc("scePower_Service", "scePower", nid);
}

static u32 FindPowerDriverFunction(u32 nid)
{
	return FindProc("scePower_Service", "scePower_driver", nid);
}

int (* scePowerSetCpuClockFrequency_k)(int cpufreq);
int (* scePowerSetBusClockFrequency_k)(int busfreq);
int (* scePowerSetClockFrequency_k)(int cpufreq, int ramfreq, int busfreq);

#ifndef PSP_SLIM

void PatchNandDriver(u32 text_addr)
{
	_sh(0xac60, text_addr+0x89B6);	
	ClearCaches();		
}

#endif

//////////////////////////////////////////////////////////////

void PatchUmdMan(u32 text_addr)
{
	if (sceKernelBootFrom() == PSP_BOOT_MS)
	{
		// Replace call to sceKernelBootFrom with return PSP_BOOT_DISC
		_sw(0x24020020, text_addr+0x13B4);
		ClearCaches();
	}
}


//////////////////////////////////////////////////////////////

int (* reboot_decompress)(u8 *dest, u32 destSize, const u8 *src, void *unk, void *unk2);

int LoadRebootex(u8 *dest, u32 destSize, const u8 *src, void *unk, void *unk2)
{
	u8 *output = (u8 *)0x88fb0000;
	char *theiso;
	int i;

	for (i = 0; i < 0x100; i++)
	{
		output[i] = 0;		
	}

	theiso = GetUmdFile();
	if (theiso)
	{	
		strcpy((void *)0x88fb0000, theiso);
	}

	memcpy((void *)0x88fb0050, &config, sizeof(SEConfig));
	_sw(bootfile, 0x88fb00c0);

	sceKernelGzipDecompress((void *)0x88fc0000, 0x4000, rebootex+0x10, 0);
	return reboot_decompress(dest, destSize, src, unk, unk2);	
}

void PatchLoadExec(u32 text_addr)
{
	MAKE_CALL(text_addr+0x1E3C, LoadRebootex);
	_sw(0x3C0188fc, text_addr+0x1E80);

	// Allow LoadExecVSH in whatever user level
	_sw(0x1000000b, text_addr+0x151C);
	_sw(0, text_addr+0x155C);

	// Allow ExitVSHVSH in whatever user level
	_sw(0x10000008, text_addr+0x0AE4);
	_sw(0, text_addr+0x0B18);
	
	reboot_decompress = (void *)(text_addr+0x2340);
}

void PatchInitLoadExecAndMediaSync(u32 text_addr)
{
	char *filename = sceKernelInitFileName();
	int clear = 0;
	
	if (filename)
	{
		if (strstr(filename, ".PBP"))
		{
			// Patch mediasync (avoid error 0x80120005 old sfo error) 
			// Make return 0, move s0, zero
			_sw(0x00008021, text_addr+0x628);

			// Avoid direct elf (no pbp) error (for eLoader plugin)
			// mov s0, zero
			_sw(0x00008021, text_addr+0x528);		

			ClearCaches();
			
			clear = 1;			
		}

		else if (strstr(filename, "disc") == filename)
		{
			if (!config.umdactivatedplaincheck)
				UndoSuperNoPlainModuleCheckPatch();

			char *theiso = GetUmdFile();

			if (theiso)
			{
				if (strncmp(theiso, "ms0:/", 5) == 0)
				{
					if (config.umdmode == MODE_UMD)
					{
						DoAnyUmd();
					}
					else if (config.umdmode == MODE_OE_LEGACY)
					{
						sceIoDelDrv("umd");
						sceIoAddDrv(getumd9660_driver());
						// Patch the error of no disc
						_sw(0x34020000, text_addr+0x8C);
					}
					if (config.umdmode != MODE_OE_LEGACY)
					{
						// Patch the error of diferent sfo?
						_sw(0x00008021, text_addr+0x94);
					}
				}
				else
				{
					clear = 1;
				}
			}
		}
		else
		{
			clear = 1;
		}
	}
	else
	{
		clear = 1;
	}

	if (clear)
	{
		SetUmdFile("");
		bootfile = 0;		
	}

	SceModule2 *mod = sceKernelFindModuleByName("sceLoadExec");
	PatchLoadExec(mod->text_addr);
	
	PatchMesgLed();
	ClearCaches();	

#ifndef PSP_SLIM

	if (booted150)
	{
		// Note: using new nid
		
		int (* sceClockgenAudioClkEnable) (void) = (void *)FindProc("sceClockgen_Driver", "sceClockgen_driver", 0x53D8F603);
	
		sceClockgenAudioClkEnable();
	}

#endif

}

//////////////////////////////////////////////////

void *block;
int drivestat = SCE_UMD_READY | SCE_UMD_MEDIA_IN;
SceUID umdcallback;

void UmdCallback(int stat)
{
	if (umdcallback >= 0)
	{
		sceKernelNotifyCallback(umdcallback, stat);
	}
}

int sceUmdActivatePatched(const int mode, const char *aliasname)
{
	int k1 = pspSdkSetK1(0);
	sceIoAssign(aliasname, "umd0:", "isofs0:", IOASSIGN_RDONLY, NULL, 0);
	
	int report_callback = !(drivestat & SCE_UMD_READABLE);
	
	drivestat = SCE_UMD_READABLE | SCE_UMD_READY | SCE_UMD_MEDIA_IN;	

	if (report_callback)
		UmdCallback(SCE_UMD_READABLE);

	pspSdkSetK1(k1);	
	return 0;
}

int sceUmdDeactivatePatched(const int mode, const char *aliasname)
{
	int k1 = pspSdkSetK1(0);
	
	sceIoUnassign(aliasname);
	drivestat = SCE_UMD_MEDIA_IN | SCE_UMD_READY;
	
	UmdCallback(drivestat);
	pspSdkSetK1(k1);
	return 0;
}

int sceUmdGetDiscInfoPatched(SceUmdDiscInfo *disc_info)
{
	int k1 = pspSdkSetK1(0);
	
	disc_info->uiSize = 8; 
	disc_info->uiMediaType = SCE_UMD_FMT_GAME; 

	pspSdkSetK1(k1);
	return 0;
}

int sceUmdRegisterUMDCallBackPatched(SceUID cbid)
{
	int k1 = pspSdkSetK1(0);
	
	umdcallback = cbid;
	UmdCallback(drivestat);

	pspSdkSetK1(k1);
	return 0;
}

int sceUmdUnRegisterUMDCallBackPatched(SceUID cbid)
{
	int k1 = pspSdkSetK1(0);
	
	umdcallback = -1;
	
	pspSdkSetK1(k1);
	return 0;
}

int sceUmdGetDriveStatPatched()
{
	int k1 = pspSdkSetK1(0);
	int res = drivestat;
	
	pspSdkSetK1(k1);
	return res;
}

u32 FindUmdUserFunction(u32 nid)
{
	return FindProc("sceUmd_driver", "sceUmdUser", nid);
}

void PatchIsofsDriver(u32 text_addr)
{
	char *iso = GetUmdFile();

	if (iso)
	{
		if (strstr(iso, "ms0:/") == iso)
		{
			if (config.umdmode == MODE_OE_LEGACY)
			{
				// make module exit inmediately 
				_sw(0x03E00008, text_addr+0x43B8);
				_sw(0x24020001, text_addr+0x43BC);				

				ClearCaches();
			}
		}
	}

	/*_sw(0x03e00008, text_addr+0x4a2c);
	_sw(0x34020000, text_addr+0x4a30);*/
}

#ifndef USE_STATIC_PATCHES

void PatchPower(u32 text_addr)
{
	_sw(0, text_addr+0x0C30);

	ClearCaches();
}

void PatchWlan(u32 text_addr)
{
	_sw(0, text_addr+0x25A8);
	ClearCaches();
}

#endif

void DoNoUmdPatches()
{
	REDIRECT_FUNCTION(FindUmdUserFunction(0xC6183D47), sceUmdActivatePatched);
	REDIRECT_FUNCTION(FindUmdUserFunction(0xE83742BA), sceUmdDeactivatePatched);
	REDIRECT_FUNCTION(FindUmdUserFunction(0x340B7686), sceUmdGetDiscInfoPatched);
	REDIRECT_FUNCTION(FindUmdUserFunction(0xAEE7404D), sceUmdRegisterUMDCallBackPatched);
	REDIRECT_FUNCTION(FindUmdUserFunction(0xBD2BDE07), sceUmdUnRegisterUMDCallBackPatched);
	MAKE_DUMMY_FUNCTION1(FindUmdUserFunction(0x46EBB729)); // sceUmdCheckMedium
	MAKE_DUMMY_FUNCTION0(FindUmdUserFunction(0x8EF08FCE)); // sceUmdWaitDriveStat
	MAKE_DUMMY_FUNCTION0(FindUmdUserFunction(0x56202973)); // sceUmdWaitDriveStatWithTimer
	MAKE_DUMMY_FUNCTION0(FindUmdUserFunction(0x4A9E5E29)); // sceUmdWaitDriveStatCB
	REDIRECT_FUNCTION(FindUmdUserFunction(0x6B4A146C), sceUmdGetDriveStatPatched);
	MAKE_DUMMY_FUNCTION0(FindUmdUserFunction(0x20628E6F)); // sceUmdGetErrorStat

	ClearCaches();
}

int sceChkregGetPsCodePatched(u8 *pscode)
{
	pscode[0] = 0x01;
	pscode[1] = 0x00;
	pscode[2] = config.fakeregion + 2;

	if (pscode[2] == 2)
		pscode[2] = 3;

	pscode[3] = 0x00;
	pscode[4] = 0x01;
	pscode[5] = 0x00;
	pscode[6] = 0x01;
	pscode[7] = 0x00;

	return 0;
}

void PatchChkreg()
{
	u32 pscode = FindProc("sceChkreg", "sceChkreg_driver", 0x59F8491D);

	if (pscode)
	{
		if (config.fakeregion)
		{
			REDIRECT_FUNCTION(pscode, sceChkregGetPsCodePatched);			
		}		
	}

	/*if (config.freeumdregion)
	{	
		u32 checkregion = FindProc("sceChkreg", "sceChkreg_driver", 0x54495B19);

		if (checkregion)
		{
			MAKE_DUMMY_FUNCTION1(checkregion);
		}
	}*/

	ClearCaches();
}

void SetSpeed(int cpu, int bus)
{
	if (cpu == 20  || cpu == 75 || cpu == 100 || cpu == 133 || cpu == 333 || cpu == 300 ||
		cpu == 266 || cpu == 222)
	{	
		scePowerSetClockFrequency_k = (void *)FindPowerFunction(0x737486F2);
		scePowerSetClockFrequency_k(cpu, cpu, bus);

		if (sceKernelInitMode() != PSP_INIT_MODE_VSH)
		{	
			MAKE_DUMMY_FUNCTION0((u32)scePowerSetClockFrequency_k);
			MAKE_DUMMY_FUNCTION0((u32)FindPowerFunction(0x545A7F3C));
			MAKE_DUMMY_FUNCTION0((u32)FindPowerFunction(0xB8D7B3FB));
			MAKE_DUMMY_FUNCTION0((u32)FindPowerFunction(0x843FBF43));

			ClearCaches();
		}		
	}	
}

void sctrlHENSetSpeed(int cpu, int bus)
{
	scePowerSetClockFrequency_k = (void *)FindPowerFunction(0x545A7F3C);
	scePowerSetClockFrequency_k(cpu, cpu, bus);
}

void OnImposeLoad()
{
	SceModule2 *mod = sceKernelFindModuleByName("sceChkreg");

	if (mod)
	{
		sctrlSEGetConfig(&config);
		PatchChkreg();
	}
	
	if (sceKernelInitMode() == PSP_INIT_MODE_GAME)
	{
		char *theiso = GetUmdFile();

		if (theiso)
		{
			if (strncmp(theiso, "ms0:/", 5) == 0)
			{
				if (config.umdmode == MODE_OE_LEGACY)
				{
					sceIoDelDrv("isofs");
					sceIoAddDrv(getisofs_driver());
					
					if (config.umdmode == MODE_OE_LEGACY)
					{
						DoNoUmdPatches();
					}

					sceIoAssign("disc0:", "umd0:", "isofs0:", IOASSIGN_RDONLY, NULL, 0);
				}
			}			
		}		
	}
}

void OnSystemStatusIdle()
{
	if (sceKernelBootFrom() == PSP_BOOT_DISC)
	{
		SetSpeed(config.umdisocpuspeed, config.umdisobusspeed);
	}
}





