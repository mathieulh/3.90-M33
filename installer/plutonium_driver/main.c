#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <psputilsforkernel.h>
#include <pspsysmem_kernel.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <psploadcore.h>
#include <pspmodulemgr_kernel.h>
#include <pspiofilemgr_kernel.h>

#include <systemctrl.h>

#include "main.h"

#include "ipl_01g.h"
#include "ipl_02g.h"

#define M33_UPDATER	"ms0:/PSP/GAME/UPDATE/EBOOT.PBP"
#define SCE_UPDATER "ms0:/PSP/GAME/UPDATE/390.PBP"

PSP_MODULE_INFO("Plutonium_Driver", 0x1000, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

struct PspModuleImport
{
	const char *name;
	unsigned short version;
	unsigned short attribute;
	unsigned char entLen;
	unsigned char varCount;
	unsigned short funcCount;
	u32 *fnids;
	u32 *funcs;
	u32 *vnids;
	u32 *vars;
} __attribute__((packed));

u32 FindImport(char *prxname, char *importlib, u32 nid)
{
	SceModule2 *pMod;
	void *stubTab;
	int stubLen;

	pMod = sceKernelFindModuleByName(prxname);
	if (!pMod)
		return 0;

	pMod = sceKernelFindModuleByUID(pMod->modid);
	if(pMod != NULL)
	{
		int i = 0;

		stubTab = pMod->stub_top;
		stubLen = pMod->stub_size;
		//printf("stubTab %p - stubLen %d\n", stubTab, stubLen);
		while(i < stubLen)
		{
			int count;
			struct PspModuleImport *pImp = (struct PspModuleImport *) (stubTab + i);

			if(pImp->name)
			{
				//printf("Import Library %s, attr 0x%04X\n", pImp->name, pImp->attribute);
			}
			else
			{
				//printf("Import Library %s, attr 0x%04X\n", "Unknown", pImp->attribute);
			}

			if(pImp->funcCount > 0)
			{
				//printf("Function Imports:\n");
				for(count = 0; count < pImp->funcCount; count++)
				{
					//printf("Entry %-3d: UID 0x%08X, Function 0x%08X\n", count+1, pImp->fnids[count], (u32) &pImp->funcs[count*2]);

					if (pImp->name)
					{
						if (strcmp(pImp->name, importlib) == 0)
						{
							if (pImp->fnids[count] == nid)
							{
								return (u32)&pImp->funcs[count*2];
							}
						}						
					}

				}
			}

			i += (pImp->entLen * 4);
		}
	}
	
	return 0;
}

int (* sceIoAddDrv_Real)(PspIoDrv *drv);

void WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	sceIoWrite(fd, buf, size);
	sceIoClose(fd);
}

void ClearCaches()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}

int sceIoAddDrvPatched(PspIoDrv *drv)
{
	// patch nand updater

	return sceIoAddDrv_Real(drv);
}

int ipl_patched;

int sceIplUpdateSetIplPatched(u8 *buf, int size)
{
	int k1 = pspSdkSetK1(0);

	memcpy(ipl+16384, buf, sizeof(ipl)-16384);

	if (sceKernelGetModel() == PSP_MODEL_SLIM_AND_LITE)
	{
		memcpy(ipl, ipl_02g, 16384);
	}

	ClearCaches();

	int (* sceIplUpdateSetIpl)(u8 *, int) = (void *)sctrlHENFindFunction("TexSeqCounter", "sceIplUpdate", 0xEE7EB563);

	int res = sceIplUpdateSetIpl(ipl, sizeof(ipl));

	pspSdkSetK1(k1);
	return res;
}

int sceIplUpdateVerifyIplPatched(u8 *buf, int size)
{
	int k1 = pspSdkSetK1(0);

	int (* sceIplUpdateVerifyIpl)(u8 *, int) = (void *)sctrlHENFindFunction("TexSeqCounter", "sceIplUpdate", 0x0565E6DD);

	int res = sceIplUpdateVerifyIpl(ipl, sizeof(ipl));

	if (res < 0)
		res = 0;

	pspSdkSetK1(k1);
	return res;
}

SceUID sceIoOpenPatched(char *path, int flags, int mode)
{
	int k1 = pspSdkSetK1(0);

	if (!ipl_patched)
	{
		u32 addr = sctrlHENFindFunction("TexSeqCounter", "sceIplUpdate", 0xEE7EB563);

		if (addr != 0)
		{
			PatchSyscall(addr, sceIplUpdateSetIplPatched);
			PatchSyscall(sctrlHENFindFunction("TexSeqCounter", "sceIplUpdate", 0x0565E6DD), sceIplUpdateVerifyIplPatched);

			ipl_patched = 1;
		}
	}

	if (strcmp(path, M33_UPDATER) == 0)
	{
		strcpy(path, SCE_UPDATER);
	}

	pspSdkSetK1(k1);
	return sceIoOpen(path, flags, mode);
}

SceUID sceIoGetstatPatched(char *path, SceIoStat *stat)
{
	int k1 = pspSdkSetK1(0);

	if (strcmp(path, M33_UPDATER) == 0)
	{
		strcpy(path, SCE_UPDATER);
	}

	pspSdkSetK1(k1);
	return sceIoGetstat(path, stat);
}

int sceKernelStartModulePatched(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option)
{
	int k1 = pspSdkSetK1(0);

	SceModule2* mod = sceKernelFindModuleByName("sceNAND_Updater_Driver");
		
	if (mod)
	{	
		if (strcmp(mod->modname, "sceNAND_Updater_Driver") == 0)
		{
			_sh(0xac60, mod->text_addr+0x0E7E);
			//WriteFile("ms0:/nand_updater_loaded.bin", "hhhh", 4);
			ClearCaches();		
		} 
	}

	pspSdkSetK1(k1);
	return sceKernelStartModule(modid, argsize, argp, status, option);
}

int uranium_thread(SceSize args, void *argp)
{
	SceUID uid;
	static SceKernelLMOption lmoption;
	
	sceKernelSetDdrMemoryProtection((void *)0x08400000, 4*1024*1024, 0xF); 

	lmoption.size = sizeof(lmoption);
	lmoption.mpidtext = 5;
	lmoption.mpiddata = 5;
	lmoption.access = 1;
	uid = sceKernelLoadModule("ms0:/PSP/GAME/UPDATE/u235.prx", 0, &lmoption);

	if (uid > 0)
	{
		uid = sceKernelStartModule(uid, 0, NULL, NULL, NULL);
	}

	return sceKernelExitDeleteThread(0);
}

int UpdaterReturn()
{
	int k1 = pspSdkSetK1(0);

	SceUID thid = sceKernelCreateThread("uranium235", uranium_thread, 0xF, 0x1000, 0, NULL);
	
	if (thid < 0)
	{
		pspSdkSetK1(k1);
		return -1;
	}
	
	sceKernelStartThread(thid, 0, NULL);

	pspSdkSetK1(k1);
	return 0;	
}

int plutonium_thread(SceSize args, void *argp)
{
	SceModule2 *mod;
		
	PatchSyscall(sctrlHENFindFunction("scePower_Service", "scePower", 0x0442D852), UpdaterReturn);

	if (sceKernelDevkitVersion() < 0x03060010)
	{	
		u32 func = FindImport("SystemControl", "IoFileMgrForKernel", 0x8E982A74);
		u32 addr = sctrlHENFindFunction("sceIOFileManager", "IoFileMgrForKernel", 0x8E982A74);

		REDIRECT_FUNCTION(addr, sceIoAddDrvPatched);

		sceIoAddDrv_Real = (void *)func;
	}

	if (sceKernelGetModel() == PSP_MODEL_STANDARD)
	{
		PatchSyscall(sctrlHENFindFunction("sceModuleManager", "ModuleMgrForUser", 0x50F0C1EC), sceKernelStartModulePatched);
	}

	PatchSyscall(sctrlHENFindFunction("sceIOFileManager", "IoFileMgrForUser", 0x109F50BC), sceIoOpenPatched);
	PatchSyscall(sctrlHENFindFunction("sceIOFileManager", "IoFileMgrForUser", 0xACE946E8), sceIoGetstatPatched);

	ClearCaches();
	sceKernelDelayThread(700000);
	
	while ((mod = sceKernelFindModuleByName("plutonium_updater")))
	{
		sceKernelStopModule(mod->modid, 0, NULL, NULL, NULL);
		sceKernelUnloadModule(mod->modid);
		sceKernelDelayThread(50000);
	}

	SceUID uid = sceKernelLoadModuleMs1(SCE_UPDATER, 0, NULL);	

	if (uid >= 0)
	{
		sceKernelStartModule(uid, strlen(M33_UPDATER)+1, M33_UPDATER, NULL, NULL);
	}

	return sceKernelExitDeleteThread(0);
}

int PlutoniumStartUpdate(int argc, char *argv[])
{
	int k1 = pspSdkSetK1(0);
	
	SceUID thid = sceKernelCreateThread("plutonium", plutonium_thread, 0xF, 0x1000, 0, NULL);
	
	if (thid < 0)
	{
		pspSdkSetK1(k1);
		return -1;
	}

	sceKernelStartThread(thid, 0, NULL);

	pspSdkSetK1(k1);
	return 0;
}

int PlutoniumGetModel()
{
	int k1 = pspSdkSetK1(0);

	int res = sceKernelGetModel();

	pspSdkSetK1(k1);
	return res;
}

int PlutoniumRebootPsp()
{
	int k1 = pspSdkSetK1(0);

	int (* reboot)(int) = (void *)sctrlHENFindFunction("scePower_Service", "scePower", 0x0442D852);
	int res = reboot(0);

	pspSdkSetK1(k1);
	return res;
}

int module_start(SceSize args, void *argp)
{
	if (argp && args)
	{
		u32 addr = sctrlHENFindFunction("scePower_Service", "scePower", 0x2085D15D);
		if (addr)
		{
			_sw(0x03e00008, addr);
			_sw(0x24020062, addr+4);

			ClearCaches();
		}
	}
	
	return 0;
}

int module_stop(void)
{
	return 0;
}
