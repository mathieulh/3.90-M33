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
#include <pspsysmem_kernel.h>
#include <pspusb.h>


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
int bootfile = 0;
int dummy[0x1C/4];
int ram2, ram8;
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

#else

typedef struct PartitionData
{
	u32 unk[5];
	u32 size;
} PartitionData;

typedef struct SysMemPartition
{
	struct SysMemPartition *next;
	u32	address;
	u32 size;
	u32 attributes;
	PartitionData *data;
} SysMemPartition;

void ApplyMemory()
{
	if (ram2 != 0 && (ram2+ram8) <= 52)
	{
		SysMemPartition *(* GetPartition)(int partition) = (void *)0x88000e74;
		SysMemPartition *partition;
		u32 user_size;

		user_size = (ram2*1024*1024);
		partition = GetPartition(PSP_MEMORY_PARTITION_USER);
		partition->size = user_size;
		partition->data->size = (((user_size >> 8) << 9) | 0xFC);

		partition = GetPartition(8);
		partition->size = (ram8*1024*1024);
		partition->address = 0x88800000+user_size;
		partition->data->size = (((partition->size >> 8) << 9) | 0xFC);
	}		
}

void PatchUmdCache(u32 text_addr)
{
	if (sceKernelInitMode() == PSP_INIT_MODE_GAME)
	{
		if (sceKernelBootFrom() == PSP_BOOT_MS)
		{
			// Make umdcache exit inmediately so it doesn't alloc memory
			_sw(0x03E00008, text_addr+0x810);
			_sw(0x24020001, text_addr+0x814);

			ClearCaches();

			// Unprotect memory
			u32 *prot = (u32 *)0xbc000040;
			int i;

			for (i = 0; i < 0x10; i++)
				prot[i] = 0xFFFFFFFF;						
		}
	}	
}

#endif

int sctrlHENSetMemory(u32 p2, u32 p8)
{
	if (p2 == 0)
		return 0x80000107;

	if ((p2+p8) > 52)
	{
		return 0x80000107;
	}

	int k1 = pspSdkSetK1(0);

	ram2 = p2;
	ram8 = p8;
	
	pspSdkSetK1(k1);
	return 0;

}

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

	_sw(ram2, 0x88fb00c4);
	_sw(ram8, 0x88fb00c8);
	_sw(booted150, 0x88fb00cc);
	
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

typedef struct  __attribute__((packed))
{
	u32 signature;
	u32 version;
	u32 fields_table_offs;
	u32 values_table_offs;
	int nitems;
} SFOHeader;

typedef struct __attribute__((packed))
{
	u16 field_offs;
	u8  unk;
	u8  type; // 0x2 -> string, 0x4 -> number
	u32 unk2;
	u32 unk3;
	u16 val_offs;
	u16 unk4;
} SFODir;

int (* CheckSfo)(char *sfo, SceSize size, void *gameinfo);

int CheckSfoPatched(char *sfo, SceSize size, void *gameinfo)
{
	int largememory = 0;
	
	if (ram2 == 0)
	{	
		SFOHeader *header = (SFOHeader *)sfo;
		SFODir *entries = (SFODir *)(sfo+0x14);
		int i;
		
		for (i = 0; i < header->nitems; i++)
		{
			if (strcmp(sfo+header->fields_table_offs+entries[i].field_offs, "MEMSIZE") == 0)
			{
				memcpy(&largememory, sfo+header->values_table_offs+entries[i].val_offs, 4);
			}			
		}

		if (largememory)
		{
			sctrlHENSetMemory(52, 0);
			ApplyMemory();
		}
	}
	else
	{
		ApplyMemory();
		ram2 = 0;	
	}

	return CheckSfo(sfo, size, gameinfo);
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

			// Patch for slim large memory check
			MAKE_CALL(text_addr+0x61C, CheckSfoPatched);
			CheckSfo = (void *)(text_addr+0x860);

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

static int connected = 0;
static int x = 0;

static SceUInt vtimer_handler(SceUID uid, SceInt64 a, SceInt64 b, void *param)
{
	// Note: using new nid for power_driver, old nid for power
	
	int (* batcharging)(void) = (void *)FindPowerFunction(0x1E490401);

	if (!batcharging())
	{
		if (connected == 1)
		{
			if (x)
			{			
				int (* stop_charge)(int) = (void *)FindPowerDriverFunction(0x3642B63C);
				stop_charge(0);				
			}

			x = !x;
			connected = 0;
			return 5000000;
		}
		
		else
		{
			int (* start_charge)(int) = (void *)FindPowerDriverFunction(0x854D34FB);
			start_charge(1);
			connected = 1;			

			return 15000000;
		}
	}	

	return 2000000;
}

void OnImposeLoad(u32 text_addr)
{
	sctrlSEGetConfig(&config);
	
	PatchChkreg();	
	
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

	if (config.usbcharge)
	{
		SceUID vtimer = sceKernelCreateVTimer("", NULL);

		if (vtimer >= 0)
		{	
			sceKernelStartVTimer(vtimer);
			sceKernelSetVTimerHandlerWide(vtimer, 5000000, vtimer_handler, NULL);

			// Destroy calls to charge functions from usb.prx
			SceModule2 *mod = sceKernelFindModuleByName("sceUSB_Driver");
			if (mod)
			{
				MAKE_DUMMY_FUNCTION0(mod->text_addr+0x8874);
				MAKE_DUMMY_FUNCTION0(mod->text_addr+0x887C);
				ClearCaches();
			}
		}
	}

	ClearCaches();
}

void OnSystemStatusIdle()
{
	if (sceKernelBootFrom() == PSP_BOOT_DISC)
	{
		SetSpeed(config.umdisocpuspeed, config.umdisobusspeed);
	}
}





