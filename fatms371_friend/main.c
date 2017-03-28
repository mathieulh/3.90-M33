#include <pspsdk.h>
#include <pspkernel.h>
#include <pspiofilemgr_kernel.h>
#include <systemctrl.h>

#include <stdio.h>
#include <string.h>

#include "patch.h"


PSP_MODULE_INFO("pspFatMs371_Friend", 0x1007, 1, 0);

int (*IoDread)(PspIoDrvFileArg *arg, SceIoDirent *dir);
STMOD_HANDLER previous;

typedef struct
{
	char shortname[13];
	u8   pad[3]; // Just in case compiler doesn't do it automatically
	char longname[256];
} PspMsFatDirentPrivate;

typedef struct
{
	SceSize size;
	char shortname[13];
	u8	 pad[3]; // Just in case compiler doesn't do it automatically
	char longname[1024];
} PspMsFatDirentPrivate380;

void ClearCaches()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}

int MyDread(PspIoDrvFileArg *arg, SceIoDirent *dir)
{	
	PspMsFatDirentPrivate380 *private = NULL;

	if (sceKernelGetCompiledSdkVersion() >= 0x03080000)
	{
		private = dir->d_private;

		if (private)
		{
			if (private->size != sizeof(PspMsFatDirentPrivate380))
			{
				Kprintf("Error: d_private invalid size (0x%08X) for sdk_version >= 3.80\n", private->size);
				return 0x80010016;	
			}
		}
	}

	int res = IoDread(arg, dir);

	if (res > 0)
	{
		if (private)
		{
			//PspMsFatDirentPrivate *private_old = dir->d_private;
			u8 *p = (dir->d_private);

			memmove(p+0x10, p+0x0D, 256);
			memmove(p+4, p, sizeof(PspMsFatDirentPrivate));

			if (private->longname[0] == 0)
			{
				strncpy(private->longname, private->shortname, 13);
			}
			
			private->size = sizeof(PspMsFatDirentPrivate380);
		}		
	}

	return res;
}

int Fatms_sceIoAddDrv(PspIoDrv *driver)
{
	if (driver)
	{
		IoDread = driver->funcs->IoDread;
		driver->funcs->IoDread = MyDread;
		ClearCaches();
	}

	return sceIoAddDrv(driver);

}

int sceRtcGetCurrentClockLocalTimeP(u32 a0, u32 a1, u32 a2, u32 a3)
{
	return sceRtcGetCurrentClockLocalTime(a0, a1, a2, a3);
}

int sceRtcSetDosTimeP(u32 a0, u32 a1, u32 a2, u32 a3)
{
	return sceRtcSetDosTime(a0, a1, a2, a3);
}

int OnModuleStart(SceModule2 *mod)
{
	if (strcmp(mod->modname, "sceMSFAT_Driver") == 0)
	{
		u32 version = _lw(mod->segmentaddr[1]);

		if ((version >> 16) == 0x0307)
		{
			_sw(0, mod->text_addr+0x22D8);
			MAKE_CALL(mod->text_addr+0x34, Fatms_sceIoAddDrv);
			REDIRECT_FUNCTION(mod->text_addr+0x23EDC, sceRtcGetCurrentClockLocalTimeP);
			REDIRECT_FUNCTION(mod->text_addr+0x23EE4, sceRtcSetDosTimeP);

			ClearCaches();
		}
	}

	if (previous)
		return previous(mod);

	return 0;
}

int module_start(SceSize args, void *argp)
{
	previous = sctrlHENSetStartModuleHandler(OnModuleStart);

	return 0;
}

