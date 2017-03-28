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

	u32 *mod = (u32 *)sceKernelFindModuleByName("sceInit");
	u32 text_addr;

	if (!mod)
	{
		pspSdkSetK1(k1);
		return -1;
	}

	text_addr = *(mod+27);
	_sw(apitype, text_addr+0x22A0);

	pspSdkSetK1(k1);
	return 0;
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
	_sw((u32)filename, text_addr+0x22C4);

	pspSdkSetK1(k1);
	return 0;
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
	thstruct = (u32 *)_lw(text_addr+0x16CA4);

	thstruct[0xD8/4] = (level ^ 8) << 28;

	pspSdkSetK1(k1);
	return res;
}

int sctrlKernelSetDevkitVersion(int version)
{
	int k1 = pspSdkSetK1(0);

	int high = version >> 16;
	int low = version & 0xFFFF;

	_sh(high, 0x8800e60c);
	_sh(low, 0x8800e614);

	ClearCaches();

	pspSdkSetK1(k1);
	return 0;
}

int	sctrlHENIsSE()
{
	return 1;
}

int	sctrlHENIsDevhook()
{
	return 1;
}

int sctrlHENGetVersion()
{
	return 0x00000400;
}

int sctrlSEGetVersion()
{
	return 0x00000300;
}

int sctrlKernelLoadExecVSHDisc(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	int k1;
	int userlevel;
	int res;

	k1 = pspSdkSetK1(0x18 << 16);
	userlevel = sctrlKernelSetUserLevel(4);

	res = sceKernelLoadExecVSHDisc(file, param);

	sctrlKernelSetUserLevel(userlevel);
	pspSdkSetK1(k1);

	return res;
}

int sctrlKernelLoadExecVSHDiscUpdater(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	int k1;
	int userlevel;
	int res;

	k1 = pspSdkSetK1(0x18 << 16);
	userlevel = sctrlKernelSetUserLevel(4);

	res = sceKernelLoadExecVSHDiscUpdater(file, param);

	sctrlKernelSetUserLevel(userlevel);
	pspSdkSetK1(k1);

	return res;
}

int sctrlKernelLoadExecVSHMs1(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	int k1;
	int userlevel;
	int res;

	k1 = pspSdkSetK1(0x18 << 16);
	userlevel = sctrlKernelSetUserLevel(4);

	res = sceKernelLoadExecVSHMs1(file, param);

	sctrlKernelSetUserLevel(userlevel);
	pspSdkSetK1(k1);

	return res;
}

int sctrlKernelLoadExecVSHMs2(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	int k1;
	int userlevel;
	int res;

	k1 = pspSdkSetK1(0x18 << 16);
	userlevel = sctrlKernelSetUserLevel(4);

	res = sceKernelLoadExecVSHMs2(file, param);

	sctrlKernelSetUserLevel(userlevel);
	pspSdkSetK1(k1);

	return res;
}

int sctrlKernelLoadExecVSHMs3(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	int k1;
	int userlevel;
	int res;

	k1 = pspSdkSetK1(0x18 << 16);
	userlevel = sctrlKernelSetUserLevel(4);

	res = sceKernelLoadExecVSHMs3(file, param);

	sctrlKernelSetUserLevel(userlevel);
	pspSdkSetK1(k1);

	return res;
}

int sctrlKernelLoadExecVSHWithApitype(int apitype, const char *file, struct SceKernelLoadExecVSHParam *param)
{
	int k1;
	int userlevel;
	int res;
	int (* LoadExecVSH)(int apitype, const char *file, struct SceKernelLoadExecVSHParam *param, int unk2);

	k1 = pspSdkSetK1(0);

	u32 *mod = (u32 *)sceKernelFindModuleByName("sceLoadExec");
	u32 text_addr;

	if (!mod)
		return -1;

	text_addr = *(mod+27);
	LoadExecVSH = (void *)(text_addr+0x1D6C);

	pspSdkSetK1(k1);

	k1 = pspSdkSetK1(0x18 << 16);
	userlevel = sctrlKernelSetUserLevel(4);

	res = LoadExecVSH(apitype, file, param, 0x10000);

	sctrlKernelSetUserLevel(userlevel);
	pspSdkSetK1(k1);

	return res;
}

int sctrlKernelExitVSH(struct SceKernelLoadExecVSHParam *param)
{
	int k1;
	int userlevel;
	int res;

	k1 = pspSdkSetK1(0x18 << 16);
	userlevel = sctrlKernelSetUserLevel(4);

	res = sceKernelExitVSHVSH(param);

	sctrlKernelSetUserLevel(userlevel);
	pspSdkSetK1(k1);

	return res;
}

PspIoDrv *sctrlHENFindDriver(char *drvname)
{
	int k1 = pspSdkSetK1(0);
	u32 *(* GetDevice)(char *) = (void *)0x8804dd24;
	u32 *u;

	u = GetDevice(drvname);

	if (!u)
	{
		pspSdkSetK1(k1);
		return NULL;
	}

	return (PspIoDrv *)u[1];
}



