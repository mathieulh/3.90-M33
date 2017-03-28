#include <pspsdk.h>
#include <pspkernel.h>
#include <pspthreadman_kernel.h>
#include "main.h"
#include "systemctrl.h"
#include "systemctrl_se.h"
#include "sysmodpatches.h"
#include "umd9660_driver.h"
#include "isofs_driver.h"

int sctrlKernelSetInitApitype(int apitype)
{
	int k1 = pspSdkSetK1(0);
	int prev = sceKernelInitApitype();

	u32 *mod = (u32 *)sceKernelFindModuleByName("sceInit");
	u32 text_addr;

	if (!mod)
	{
		pspSdkSetK1(k1);
		return -1;
	}

	text_addr = *(mod+27);
	_sw(apitype, text_addr+0x1E00);

	pspSdkSetK1(k1);
	return prev;
}

int sctrlKernelSetInitFileName(char *filename)
{
	int k1 = pspSdkSetK1(0);

	u32 *mod = (u32 *)sceKernelFindModuleByName("sceInit");
	u32 text_addr;

	if (!mod)
	{
		pspSdkSetK1(k1);
		return -1;
	}

	text_addr = *(mod+27);
	_sw((u32)filename, text_addr+0x1E24);

	pspSdkSetK1(k1);
	return 0;
}

int sctrlKernelSetInitKeyConfig(int key)
{
	int k1 = pspSdkSetK1(0);
	int prev = sceKernelInitKeyConfig();

	u32 *mod = (u32 *)sceKernelFindModuleByName("sceInit");
	u32 text_addr;

	if (!mod)
	{
		pspSdkSetK1(k1);
		return -1;
	}

	text_addr = *(mod+27);
	_sw(key, text_addr+0x1F70);

	pspSdkSetK1(k1);
	return prev;
}

int sctrlKernelSetUserLevel(int level)
{
	int k1 = pspSdkSetK1(0);
	int res = sceKernelGetUserLevel();
	
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceThreadManager");
	u32 text_addr;
	u32 *thstruct;

	if (!mod)
	{
		pspSdkSetK1(k1);
		return -1;
	}

	text_addr = *(mod+27);
	thstruct = (u32 *)_lw(text_addr+0x173C4);

	thstruct[0x14/4] = (level ^ 8) << 28;

	pspSdkSetK1(k1);
	return res;
}

int sctrlKernelSetDevkitVersion(int version)
{
	int k1 = pspSdkSetK1(0);
	int prev = sceKernelDevkitVersion();

	int high = version >> 16;
	int low = version & 0xFFFF;

	_sh(high, 0x8800E960);
	_sh(low, 0x8800E968);

	ClearCaches();

	pspSdkSetK1(k1);
	return prev;
}

int	sctrlHENIsSE()
{
	return 1;
}

int	sctrlHENIsDevhook()
{
	return 0;
}

int sctrlHENGetVersion()
{
	return 0x00000500;
}

int sctrlSEGetVersion()
{
	return 0x00000600;
}

int sctrlKernelLoadExecVSHDisc(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	int k1;
	int res;

	k1 = pspSdkSetK1(0);
	res = sceKernelLoadExecVSHDisc(file, param);

	pspSdkSetK1(k1);
	return res;
}

int sctrlKernelLoadExecVSHDiscUpdater(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	int k1;
	int res;

	k1 = pspSdkSetK1(0);
	res = sceKernelLoadExecVSHDiscUpdater(file, param);

	pspSdkSetK1(k1);
	return res;
}

int sctrlKernelLoadExecVSHMs1(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	int k1;
	int res;

	k1 = pspSdkSetK1(00);
	res = sceKernelLoadExecVSHMs1(file, param);

	pspSdkSetK1(k1);
	return res;
}

int sctrlKernelLoadExecVSHMs2(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	int k1;
	int res;

	k1 = pspSdkSetK1(0);	
	res = sceKernelLoadExecVSHMs2(file, param);
	
	pspSdkSetK1(k1);
	return res;
}

int sctrlKernelLoadExecVSHMs3(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	int k1;
	int res;

	k1 = pspSdkSetK1(0);	
	res = sceKernelLoadExecVSHMs3(file, param);
	pspSdkSetK1(k1);

	return res;
}

int sctrlKernelLoadExecVSHMs4(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	int k1;
	int res;

	k1 = pspSdkSetK1(0);	
	res = sceKernelLoadExecVSHMs4(file, param);
	pspSdkSetK1(k1);

	return res;
}

int sctrlKernelLoadExecVSHWithApitype(int apitype, const char *file, struct SceKernelLoadExecVSHParam *param)
{
	int k1;
	int res;
	int (* LoadExecVSH)(int apitype, const char *file, struct SceKernelLoadExecVSHParam *param, int unk2);

	k1 = pspSdkSetK1(0);

	u32 *mod = (u32 *)sceKernelFindModuleByName("sceLoadExec");
	u32 text_addr;

	if (!mod)
		return -1;

	text_addr = *(mod+27);
	LoadExecVSH = (void *)(text_addr+0xEE8);

	res = LoadExecVSH(apitype, file, param, 0x10000);
	
	pspSdkSetK1(k1);

	return res;
}

int sctrlKernelExitVSH(struct SceKernelLoadExecVSHParam *param)
{
	int k1;
	int res;

	k1 = pspSdkSetK1(0);
	res = sceKernelExitVSHVSH(param);

	pspSdkSetK1(k1);
	return res;
}

static void PatchIsofsDriver2()
{
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceIsofs_driver");

	if (mod)
	{
		u32 text_addr = *(mod+27);

		_sw(0x03e00008, text_addr+0x4a2c);
		_sw(0x34020000, text_addr+0x4a30);
		ClearCaches();
	}
}

int sctrlSEMountUmdFromFile(char *file, int noumd, int isofs)
{
	int k1 = pspSdkSetK1(0);
	int res;

	SetUmdFile(file);

	if (!noumd && !isofs)
	{
		DoAnyUmd();
	}

	else
	{
		if ((res = sceIoDelDrv("umd")) < 0)
			return res;

		if ((res = sceIoAddDrv(getumd9660_driver())) < 0)
			return res;
	}

	if (noumd)
	{
		DoNoUmdPatches();
	}

	if (isofs)
	{
		sceIoDelDrv("isofs");
		sceIoAddDrv(getisofs_driver());
		PatchIsofsDriver2();

		sceIoAssign("disc0:", "umd0:", "isofs0:", IOASSIGN_RDONLY, NULL, 0);
	}

	pspSdkSetK1(k1);
	return 0;
}

PspIoDrv *sctrlHENFindDriver(char *drvname)
{
	int k1 = pspSdkSetK1(0);
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceIOFileManager");

	if (!mod)
	{
		pspSdkSetK1(k1);
		return NULL;
	}

	u32 text_addr = *(mod+27);

	u32 *(* GetDevice)(char *) = (void *)(text_addr+0x1920);
	u32 *u;

	u = GetDevice(drvname);

	if (!u)
	{
		pspSdkSetK1(k1);
		return NULL;
	}

	return (PspIoDrv *)u[1];
}



