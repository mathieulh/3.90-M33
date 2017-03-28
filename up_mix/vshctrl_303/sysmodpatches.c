#include <pspsdk.h>
#include <pspkernel.h>
#include <pspinit.h>
#include <psploadexec_kernel.h>
#include <psputilsforkernel.h>
#include <pspreg.h>
#include <pspmediaman.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "sysmodpatches.h"
#include "main.h"
#include "reboot.h"
#include "virtualpbpmgr.h"
#include "systemctrl_se.h"
#include "malloc.h"

SEConfig config;
u32 vshmain_args[0x0400/4];

void SetConfig(SEConfig *newconfig)
{
	memcpy(&config, newconfig, sizeof(SEConfig));
}

/*SEConfig *GetConfig()
{
	return &config;
}*/

int LoadStartModule(char *module)
{
	SceUID mod = sceKernelLoadModule(module, 0, NULL);

	if (mod < 0)
		return mod;

	return sceKernelStartModule(mod, strlen(module)+1, module, NULL, NULL);
}

int ReadLine(SceUID fd, char *str)
{
	char ch = 0;
	int n = 0;

	while (1)
	{	
		if (sceIoRead(fd, &ch, 1) != 1)
			return n;

		if (ch < 0x20)
		{
			if (n != 0)
				return n;
		}
		else
		{
			*str++ = ch;
			n++;
		}
	}
	
}

u32 FindPowerFunction(u32 nid)
{
	return FindProc("scePower_Service", "scePower", nid);
}

u32 FindPowerDriverFunction(u32 nid)
{
	return FindProc("scePower_Service", "scePower_driver", nid);
}

int (* scePowerSetCpuClockFrequency)(int cpufreq);
int (* scePowerSetBusClockFrequency)(int busfreq);
int (* scePowerSetClockFrequency)(int cpufreq, int ramfreq, int busfreq);

/*void DestroySpeedFuncs()
{
	u32 func;
	
	//func = FindPowerFunction(0x737486F2);
	//MAKE_DUMMY_FUNCTION0(func);
	
	func = FindPowerFunction(0x843FBF43);
	MAKE_DUMMY_FUNCTION0(func);

	func = FindPowerFunction(0xB8D7B3FB);
	MAKE_DUMMY_FUNCTION0(func);

	ClearCaches();
}*/

int sceKernelStartModulePatched(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option)
{
	SceModule *mod = sceKernelFindModuleByUID(modid);
	if (mod != NULL)
	{		
		if (strcmp(mod->modname, "vsh_module") == 0)
		{
			if (sceKernelInitKeyConfig() == PSP_INIT_KEYCONFIG_VSH)
			{
				if (argsize == 0)
				{
					if (config.skiplogo)
					{
						memset(vshmain_args, 0, 0x0400);
						vshmain_args[0/4] = 0x0400;
						vshmain_args[4/4] = 0x20;
						vshmain_args[0x40/4] = 1;
						vshmain_args[0x280/4] = 1;
						vshmain_args[0x284/4] = 0x40003;

						argsize = 0x0400;
						argp = vshmain_args;				
					}
				}
			}			
		}

		else if (strcmp(mod->modname, "sceMediaSync") == 0)
		{
			u8 plugcon[15];
			char plugin[128];
			int	nvsh = 0, ngame = 0;
			int keyconfig, i;
			SceUID fd;

			keyconfig = sceKernelInitKeyConfig();

			memset(plugcon, 0, 10);

			for (i = 0; i < 0x10; i++)
			{
				fd = sceIoOpen("ms0:/seplugins/conf.bin", PSP_O_RDONLY, 0777);
				
				if (fd >= 0)
				{
					break;	
				}

				sceKernelDelayThread(20000);
			}
			
			if (fd >= 0)
			{
				sceIoRead(fd, plugcon, 15);
				sceIoClose(fd);
			}			

			fd = sceIoOpen("ms0:/seplugins/vsh.txt", PSP_O_RDONLY, 0777);
			if (fd >= 0)
			{
				for (i = 0; i < 5; i++)
				{
					memset(plugin, 0, sizeof(plugin));
					if (ReadLine(fd, plugin) > 0)
					{
						nvsh++;
						if (keyconfig == PSP_INIT_KEYCONFIG_VSH)
						{
							if (plugcon[i])
							{
								LoadStartModule(plugin);								
							}
						}
					}
					else
					{
						break;
					}
				}

				sceIoClose(fd);
			}

			fd= sceIoOpen("ms0:/seplugins/game.txt", PSP_O_RDONLY, 0777);
			if (fd >= 0)
			{
				for (i = 0; i < 5; i++)
				{
					memset(plugin, 0, sizeof(plugin));
					if (ReadLine(fd, plugin) > 0)
					{
						ngame++;
						if (keyconfig == PSP_INIT_KEYCONFIG_GAME)
						{
							if (plugcon[i+nvsh])
							{
								LoadStartModule(plugin);								
							}
						}
					}
					else
					{
						break;
					}
				}

				sceIoClose(fd);
			}

			if (keyconfig == PSP_INIT_KEYCONFIG_POPS)
			{
				fd = sceIoOpen("ms0:/seplugins/pops.txt", PSP_O_RDONLY, 0777);
				if (fd >= 0)
				{
					for (i = 0; i < 5; i++)
					{
						memset(plugin, 0, sizeof(plugin));

						if (ReadLine(fd, plugin) > 0)
						{							
							if (plugcon[i+nvsh+ngame])
							{
								LoadStartModule(plugin);								
							}
						}
						else
						{
							break;
						}
					}
					
					sceIoClose(fd);	
				}				
			}
		}
		
		/*else if (strcmp(mod->modname, "sceKernelLibrary") == 0)
		{
			//sctrlSEGetConfig(&config);
			
			return sceKernelStartModule(modid, argsize, argp, status, option);	

			scePowerSetCpuClockFrequency = (void *)FindPowerDriverFunction(0x843FBF43);
			scePowerSetBusClockFrequency = (void *)FindPowerDriverFunction(0xB8D7B3FB);
			scePowerSetClockFrequency = (void *)FindPowerDriverFunction(0x737486F2);

			if (sceKernelInitKeyConfig() == PSP_INIT_KEYCONFIG_GAME)
			{
				scePowerSetClockFrequency(333, 333, 166);
			}

			//return res;

			
			
			if ((sceKernelInitApitype() & 0x200) == 0x200)
			{
				if (config.vshcpuspeed > 0 && config.vshbusspeed > 0)
				{
					if (config.vshcpuspeed < 222)
					{
						scePowerSetCpuClockFrequency(config.vshcpuspeed);
						scePowerSetBusClockFrequency(config.vshbusspeed);
					}
					else
					{
						scePowerSetClockFrequency(config.vshcpuspeed, config.vshcpuspeed, config.vshbusspeed);
					}

					DestroySpeedFuncs();
				}
			}
			else if ((sceKernelInitApitype() & 0x120) == 0x120)
			{
				if (config.umdisocpuspeed > 0 && config.umdisobusspeed > 0)
				{
					if (config.umdisocpuspeed < 222)
					{					
						scePowerSetCpuClockFrequency(config.umdisocpuspeed);
						scePowerSetBusClockFrequency(config.umdisobusspeed);
					}
					else
					{
						scePowerSetClockFrequency(config.umdisocpuspeed, config.umdisocpuspeed, config.umdisobusspeed);
					}

					DestroySpeedFuncs();
				}
			}

			return res;
		}*/
	}

	return sceKernelStartModule(modid, argsize, argp, status, option);	
}

void PatchInit(u32 text_addr)
{
	// Patch StartModule (for vshmain sce logo patch stuff)

	MAKE_JUMP(text_addr+0x1664, sceKernelStartModulePatched);	
}

void PatchNandDriver(char *buf)
{
	u32 text_addr = (u32)buf+0xA0;

	_sh(0xac60, text_addr+0x0e4c);

	ClearCaches();

	if (mallocinit() < 0)
	{
		while (1) _sw(0, 0);
	}	
}

//////////////////////////////////////////////////////////////

void PatchUmdMan(char *buf)
{
	int intr = sceKernelCpuSuspendIntr();
	u32 text_addr = (u32)buf+0xA0;

	if (sceKernelBootFrom() == PSP_BOOT_MS)
	{
		// Replace call to sceKernelBootFrom with return PSP_BOOT_DISC
		_sw(0x24020020, text_addr+0x13F8);
	}

	sceKernelCpuResumeIntr(intr);
	ClearCaches();
}

//////////////////////////////////////////////////////////////

#define EBOOT_BIN "disc0:/PSP_GAME/SYSDIR/EBOOT.BIN"
#define BOOT_BIN  "disc0:/PSP_GAME/SYSDIR/BOOT.BIN"

int (* UtilsForKernel_7dd07271)(u8 *dest, u32 destSize, const u8 *src, void *unk, void *unk2);

int reboot150 = 0;

int LoadRebootex(u8 *dest, u32 destSize, const u8 *src, void *unk, void *unk2)
{
	u8 *output = (u8 *)0x88fc0000;
	char *theiso;
	int i;

	theiso = GetUmdFile();
	if (theiso)
	{	
		strcpy((void *)0x88fb0000, theiso);
	}

	memcpy((void *)0x88fb0050, &config, sizeof(SEConfig));

	for (i = 0; i < (sizeof(rebootex)-0x10); i++)
	{
		output[i] = rebootex[i+0x10];		
	}

	/*if (reboot150)
	{
		return sceKernelGzipDecompress(dest, destSize, reboot, 0);
	}*/

	return UtilsForKernel_7dd07271(dest, destSize, src, unk, unk2);	
}

void KXploitString(char *str)
{
	if (str)
	{
		char *perc = strchr(str, '%');

		if (perc)
		{
			strcpy(perc, perc+1);
		}
	}
}

void Fix150Path(const char *file)
{
	char str[256];

	if (strstr(file, "ms0:/PSP/GAME/") == file)
	{
		strcpy(str, (char *)file);

		char *p = strstr(str, "__150");
		
		if (p)
		{
			strcpy((char *)file+13, "150/");
			strncpy((char *)file+17, str+14, p-(str+14));
			strcpy((char *)file+17+(p-(str+14)), p+5);		
		}
	}
}

void Fix303Path(const char *file)
{
	char str[256];

	if (strstr(file, "ms0:/PSP/GAME/") == file)
	{
		strcpy(str, (char *)file);

		char *p = strstr(str, "__303");
		
		if (p)
		{
			strcpy((char *)file+13, "303/");
			strncpy((char *)file+17, str+14, p-(str+14));
			strcpy((char *)file+17+(p-(str+14)), p+5);		
		}
	}
}

int CorruptIconPatch(char *name, int g150)
{
	char path[256];
	SceIoStat stat;

	if (g150)
	{
		sprintf(path, "ms0:/PSP/GAME150/%s%%/EBOOT.PBP", name);
	}

	else
	{
		sprintf(path, "ms0:/PSP/GAME/%s%%/EBOOT.PBP", name);
	}

	memset(&stat, 0, sizeof(stat));
	
	if (sceIoGetstat(path, &stat) >= 0)
	{
		strcpy(name, "__SCE"); // hide icon
		
		return 1;
	}
	
	return 0;
}

int GetIsoIndex(const char *file)
{
	char number[5];

	if (strstr(file, "ms0:/PSP/GAME/MMMMMISO") != file)
		return -1;

	char *p = strchr(file+17, '/');

	if (!p)
		return strtol(file+22, NULL, 10);
	
	memset(number, 0, 5);
	strncpy(number, file+22, p-(file+22));

	return strtol(number, NULL, 10);	
}

void RebootVSHWithError(u32 error)
{
	int (* sceKernelExitVSHKernel)(struct SceKernelLoadExecVSHParam *param);
	struct SceKernelLoadExecVSHParam param;	

	u32 *mod = (u32 *)sceKernelFindModuleByName("sceLoadExec");
	u32 text_addr = *(mod+27);

	sceKernelExitVSHKernel = (void *)(text_addr+0x0b34);
	
	memset(&param, 0, sizeof(param));
	memset(vshmain_args, 0, sizeof(vshmain_args));

	vshmain_args[0/4] = 0x0400;
	vshmain_args[4/4] = 0x20;
	vshmain_args[0x14/4] = error;

	param.size = sizeof(param);
	param.args = 0x0400;
	param.argp = vshmain_args;
	param.vshmain_args_size = 0x0400;
	param.vshmain_args = vshmain_args;
	param.configfile = "/kd/pspbtcnf.txt";

	sceKernelExitVSHKernel(&param);
}

/*int ReadFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0777);

	if (fd < 0)
		return fd;

	int read = sceIoRead(fd, buf, size);

	sceIoClose(fd);
	return read;
}*/

int LoadExecVSHCommonPatched(int apitype, char *file, struct SceKernelLoadExecVSHParam *param, int unk2)
{
	int k1 = pspSdkSetK1(0);
	int rebooterror = 0;
	int index;
	u32 *mod;
	u32 text_addr;
	int (* LoadExecVSHCommon)(int apitype, char *file, struct SceKernelLoadExecVSHParam *param, int unk2);

	mod = (u32 *)sceKernelFindModuleByName("sceLoadExec");
	text_addr = *(mod+27);

	LoadExecVSHCommon = (void *)(text_addr+0x0ee8);

	SetUmdFile("");
	
	index = GetIsoIndex(file);

	if (index >= 0)
	{
		if (config.executebootbin)
		{
			strcpy(file, BOOT_BIN);			
			param->args = strlen(BOOT_BIN)+1;	
		}
		else
		{		
			strcpy(file, EBOOT_BIN);			
			param->args = strlen(EBOOT_BIN)+1;			
		}

		param->argp = file;
		SetUmdFile(virtualpbp_getfilename(index));
		pspSdkSetK1(k1);
		return LoadExecVSHCommon(0x120, file, param, unk2);	
	}

	Fix150Path(file);
	Fix150Path(param->argp);
	Fix303Path(file);
	Fix303Path(param->argp);	

	if (strstr(file, "ms0:/PSP/GAME150/") == file)
	{
		reboot150 = 1;
		KXploitString(file);
		KXploitString(param->argp);		
	}

	else if (strstr(file, "ms0:/PSP/GAME/") == file)
	{
		if (config.gamekernel150)
		{
			reboot150 = 1;
			KXploitString(file);
			KXploitString(param->argp);	
		}
	}

	if (strstr(file, "UPDATE/EBOOT.BIN"))
	{
		rebooterror = 1;
	}

	else if (strstr(file, "EBOOT.BIN"))
	{
		if (config.executebootbin)
		{
			strcpy(file, BOOT_BIN);	
			param->argp = file;
		}
	}
	
	else if (strstr(file, "GAME/UPDATE/EBOOT.PBP"))
	{
		SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0777);
		u32 header[10];
		char buf[8];

		if (fd >= 0)
		{			
			sceIoRead(fd, &header, sizeof(header));
			sceIoLseek(fd, header[8]+10, PSP_SEEK_SET);
			sceIoRead(fd, buf, 8);
			
			if (strcmp(buf, "updater") == 0)
			{
				rebooterror = 1;
			}
			else
			{
				sceIoLseek(fd, 0x4DE, PSP_SEEK_SET);
				sceIoRead(fd, buf, 5);

				if (strcmp(buf, "X.YZ") == 0)
				{
					rebooterror = 1;
				}
			}

			sceIoClose(fd);
		}	
	}
	
	if (rebooterror)
	{
		RebootVSHWithError(0xDADADADA);
	}

	param->args = strlen(param->argp) + 1; // update length

	if (reboot150 == 1)
	{
		//reboot = (u8 *)malloc(51000);

		//ReadFile("flash0:/kd/reboot150.gz", reboot, 51000);
		LoadStartModule("flash0:/kd/reboot150.prx");
	}

	pspSdkSetK1(k1);

	return LoadExecVSHCommon(apitype, file, param, unk2);
}

void PatchLoadExec(u32 text_addr)
{
	_sw(0x3C0188fc, text_addr+0x16DC);
	MAKE_CALL(text_addr+0x16A0, LoadRebootex);
	UtilsForKernel_7dd07271 = (void *)(text_addr+0x1B98);

	// Allow LoadExecVSH in whatever user level
	_sw(0x1000000b, text_addr+0x0F38);
	_sw(0, text_addr+0x0F78);

	// Allow ExitVSHVSH in whatever user level
	_sw(0x10000008, text_addr+0x0720);
	_sw(0, text_addr+0x0754);

	if (sceKernelInitKeyConfig() == PSP_INIT_KEYCONFIG_VSH)
	{
		MAKE_CALL(text_addr+0x0930, LoadExecVSHCommonPatched);
		MAKE_CALL(text_addr+0x0958, LoadExecVSHCommonPatched);
		MAKE_CALL(text_addr+0x0A80, LoadExecVSHCommonPatched);
		MAKE_CALL(text_addr+0x0AA8, LoadExecVSHCommonPatched);
	}	
}

void PatchInitLoadExecAndMediaSync(char *buf)
{
	int intr = sceKernelCpuSuspendIntr();	
	
	char *filename = sceKernelInitFileName();
	u32 text_addr = (u32)buf+0xA0;

	if (filename)
	{
		if (strstr(filename, ".PBP"))
		{
			// Patch mediasync (avoid error 0x80120005 sfo error) 
			// Make return 0
			_sw(0x00008021, text_addr+0x5B8);
			ClearCaches();
			
			SetUmdFile("");
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
					if (!config.usenoumd && !config.useisofsonumdinserted)
					{
						DoAnyUmd();
					}
					else
					{
						sceKernelCpuResumeIntr(intr);
						sceIoDelDrv("umd");
						sceIoAddDrv(getumd9660_driver());
						intr = sceKernelCpuSuspendIntr();
						//DoAnyUmd();
					}

					if (config.usenoumd)
					{
						// Patch the error of no disc
						_sw(0x34020000, text_addr+0xEC);
					}
					else
					{
						// Patch the error of diferent sfo?
						_sw(0x00008021, text_addr+0xF8);
					}
				}
				else
				{
					SetUmdFile("");
				}
			}
		}
		else
		{
			SetUmdFile("");
		}
	}
	else
	{
		SetUmdFile("");
	}

	u32 *mod = (u32 *)sceKernelFindModuleByName("sceInit");
	if (mod)
	{
		PatchInit(*(mod+27));
	}

	mod = (u32 *)sceKernelFindModuleByName("sceLoadExec");
	if (mod)
	{
		PatchLoadExec(*(mod+27));
	}	

	sceKernelCpuResumeIntr(intr);

	int (* sceClockgen_driver_5F8328FD) (void) = (void *)FindProc("sceClockgen_Driver", "sceClockgen_driver", 0x5F8328FD);
	
	sceClockgen_driver_5F8328FD();

	ClearCaches();			
}

int sctrlSESetRebootType(int type)
{
	reboot150 = type;
	return 0;
}

//////////////////////////////////////////////////////////////

int (* sceRegSetKeyValueReal)(REGHANDLE hd, const char *name, void *buf, SceSize size);

SceUID gamedfd = -1, game150dfd = -1, game303dfd = -1, isodfd = -1, over303 = 0, overiso = 0;
SceUID paramsfo = -1;
int vpbpinited = 0, isoindex = 0, cachechanged = 0;
VirtualPbp *cache = NULL;
int referenced[32];

SceUID sceIoDopenPatched(const char *dirname)
{
	int res, g150 = 0;
	int k1 = pspSdkSetK1(0);

	Fix150Path(dirname);
	Fix303Path(dirname);

	if (strcmp(dirname, "ms0:/PSP/GAME") == 0)
	{
		g150 = 1;
	}

	res = sceIoDopen(dirname);

	if (g150)
	{
		gamedfd = res;
		game150dfd = sceIoDopen("ms0:/PSP/GAME150");
		over303 = 0;
		overiso = 0;				
	}

	pspSdkSetK1(k1);

	return res;
}

void ApplyNamePatch(SceIoDirent *dir, char *patch)
{
	if (dir->d_name[0] != '.')
	{
		int patchname = 1;

		if (config.hidecorrupt)
		{
			if (CorruptIconPatch(dir->d_name, 1))
				patchname = 0;
		}

		if (patchname)
		{
			strcat(dir->d_name, patch);
		}
	}
}

void ApplyIsoNamePatch(SceIoDirent *dir)
{
	if (dir->d_name[0] != '.')
	{
		memset(dir->d_name, 0, 256);
		sprintf(dir->d_name, "MMMMMISO%d", isoindex++);
	}
}

int ReadCache()
{
	SceUID fd;
	int i;

	if (!cache)
	{
		cache = (VirtualPbp *)malloc(32*sizeof(VirtualPbp));
	}

	memset(cache, 0, sizeof(VirtualPbp)*32);
	memset(referenced, 0, sizeof(referenced));

	for (i = 0; i < 0x10; i++)
	{
		fd = sceIoOpen("ms0:/PSP/SYSTEM/isocache.bin", PSP_O_RDONLY, 0777);

		if (fd >= 0)
			break;
	}

	if (i == 0x10)
		return -1;

	sceIoRead(fd, cache, sizeof(VirtualPbp)*32);
	sceIoClose(fd);

	return 0;
}

int SaveCache()
{
	SceUID fd;
	int i;

	if (!cache)
		return -1;

	for (i = 0; i < 32; i++)
	{
		if (cache[i].isofile[0] != 0 && !referenced[i])
		{
			cachechanged = 1;
			memset(&cache[i], 0, sizeof(VirtualPbp));			
		}
	}

	if (!cachechanged)
		return 0;

	cachechanged = 0;

	for (i = 0; i < 0x10; i++)
	{
		//sceIoMkdir("ms0:/PSP/SYSTEM", 0777);
		
		fd = sceIoOpen("ms0:/PSP/SYSTEM/isocache.bin", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

		if (fd >= 0)
			break;
	}

	if (i == 0x10)
		return -1;

	sceIoWrite(fd, cache, sizeof(VirtualPbp)*32);
	sceIoClose(fd);

	return 0;
}

int IsCached(char *isofile, ScePspDateTime *mtime, VirtualPbp *res)
{
	int i;

	for (i = 0; i < 32; i++)
	{
		if (cache[i].isofile[0] != 0)
		{
			if (strcmp(cache[i].isofile, isofile) == 0)
			{
				if (memcmp(mtime, &cache[i].mtime, sizeof(ScePspDateTime)) == 0)
				{
					memcpy(res, &cache[i], sizeof(VirtualPbp));
					referenced[i] = 1;
					return 1;
				}
			}
		}
	}

	return 0;
}

int Cache(VirtualPbp *pbp)
{
	int i;

	for (i = 0; i < 32; i++)
	{
		if (cache[i].isofile[0] == 0)
		{
			referenced[i] = 1;
			memcpy(&cache[i], pbp, sizeof(VirtualPbp));
			cachechanged = 1;
			return 1;
		}
	}

	return 0;
}

VirtualPbp vpbp;

int sceIoDreadPatched(SceUID fd, SceIoDirent *dir)
{	
	int res;
	u32 k1 = pspSdkSetK1(0);
	
	if (fd >= 0)
	{
		if (fd == gamedfd)
		{
			if (game150dfd >= 0)
			{
				if ((res = sceIoDread(game150dfd, dir)) > 0)
				{
					ApplyNamePatch(dir, "__150");					
					pspSdkSetK1(k1);
					return res;
				}
				else
				{
					sceIoDclose(game150dfd);
					game150dfd = -1;					
				}
			}

			if (game150dfd < 0 && game303dfd < 0 && isodfd < 0 && !over303)
			{
				game303dfd = sceIoDopen("ms0:/PSP/GAME303");
				if (game303dfd < 0)
				{
					over303 = 1;
				}
			}
			
			if (game303dfd >= 0)
			{
				if ((res = sceIoDread(game303dfd, dir)) > 0)
				{
					ApplyNamePatch(dir, "__303");					
					pspSdkSetK1(k1);
					return res;
				}
				else
				{
					sceIoDclose(game303dfd);
					game303dfd = -1;
					over303 = 1;
				}
			}

			if (game150dfd < 0 && game303dfd < 0 && isodfd < 0 && !overiso)
			{
				isodfd = sceIoDopen("ms0:/ISO");
				
				if (isodfd >= 0)
				{
					if (!vpbpinited)
					{
						virtualpbp_init();
						vpbpinited = 1;							
					}
					else
					{
						virtualpbp_reset();						
					}

					ReadCache();
					isoindex = 0;
				}
				else
				{
					overiso = 1;
				}
			}

			if (isodfd >= 0)
			{
				if ((res = sceIoDread(isodfd, dir)) > 0)
				{
					char fullpath[128];
					int  res2 = -1;
					int  docache;

					if (!FIO_S_ISDIR(dir->d_stat.st_mode))
					{
						strcpy(fullpath, "ms0:/ISO/");
						strcat(fullpath, dir->d_name);

						if (IsCached(fullpath, &dir->d_stat.st_mtime, &vpbp))
						{
							res2 = virtualpbp_fastadd(&vpbp);
							docache = 0;
						}
						else
						{
							res2 = virtualpbp_add(fullpath, &dir->d_stat.st_mtime, &vpbp);
							docache = 1;							
						}
						
						if (res2 >= 0)
						{
							ApplyIsoNamePatch(dir);

							// Fake the entry from file to directory
							dir->d_stat.st_mode = 0x11FF;
							dir->d_stat.st_attr = 0x0010;
							dir->d_stat.st_size = 0;	
							
							// Change the modifcation time to creation time
							memcpy(&dir->d_stat.st_mtime, &dir->d_stat.st_ctime, sizeof(ScePspDateTime));

							if (docache)
							{
								Cache(&vpbp);									
							}
						}
					}
					
					pspSdkSetK1(k1);
					return res;
				}

				else
				{
					sceIoDclose(isodfd);
					isodfd = -1;
					overiso = 1;
				}
			}			
		}
	}

	res = sceIoDread(fd, dir);

	if (res > 0)
	{
		if (config.hidecorrupt)
			CorruptIconPatch(dir->d_name, 0);
	}

	pspSdkSetK1(k1);
	return res;
}

int sceIoDclosePatched(SceUID fd)
{
	u32 k1 = pspSdkSetK1(0);

	if (fd == gamedfd)
	{
		gamedfd = -1;
		over303 = 0;
		overiso = 0;		
		SaveCache();		
	}

	pspSdkSetK1(k1);
	return sceIoDclose(fd);
}

SceUID sceIoOpenPatched(const char *file, int flags, SceMode mode)
{
	u32 k1 = pspSdkSetK1(0);	
	int index;
		
	Fix150Path(file);
	Fix303Path(file);

	//Kprintf("opening file; ra = %08X %s\n", sceKernelGetSyscallRA(), file);

	index = GetIsoIndex(file);
	if (index >= 0)
	{
		int res = virtualpbp_open(index);
		
		pspSdkSetK1(k1);
		return res;
	}

	if (strstr(file, "disc0:/PSP_GAME/PARAM.SFO"))
	{
		pspSdkSetK1(k1);
		paramsfo = sceIoOpen(file, flags, mode);
		return paramsfo;
	}

	pspSdkSetK1(k1);

	return sceIoOpen(file, flags, mode);
}

int sceIoClosePatched(SceUID fd)
{
	u32 k1 = pspSdkSetK1(0);	
	int res = -1;

	if (vpbpinited)
	{	
		res = virtualpbp_close(fd);		
	}

	if (fd == paramsfo)
	{
		paramsfo = -1;
	}

	pspSdkSetK1(k1);

	if (res < 0)
		return sceIoClose(fd);

	return res;
}

int sceIoReadPatched(SceUID fd, void *data, SceSize size)
{
	u32 k1 = pspSdkSetK1(0);	
	int res = -1;
	
	if (vpbpinited)
	{
		res = virtualpbp_read(fd, data, size);		
	}

	if (fd == paramsfo)
	{
		int i;

		pspSdkSetK1(k1);		
		res = sceIoRead(fd, data, size);
		pspSdkSetK1(0);

		if (res > 4)
		{
			for (i = 0; i < res-4; i++)
			{
				if (memcmp(data+i, "3.", 2) == 0)
				{
					if (strlen(data+i) == 4)
					{
						memcpy(data+i, "3.03", 4);
						break;
					}
				}
			}
		}

		pspSdkSetK1(k1);
		return res;
	}

	pspSdkSetK1(k1);

	if (res < 0)
		return sceIoRead(fd, data, size);

	return res;
}

SceOff sceIoLseekPatched(SceUID fd, SceOff offset, int whence)
{
	u32 k1 = pspSdkSetK1(0);
	int res = -1;

	if (vpbpinited)
	{
		res = virtualpbp_lseek(fd, offset, whence);
	}

	pspSdkSetK1(k1);

	if (res < 0)
		return sceIoLseek(fd, offset, whence);

	return res;
}

int sceIoLseek32Patched(SceUID fd, int offset, int whence)
{
	u32 k1 = pspSdkSetK1(0);
	int res = -1;

	if (vpbpinited)
	{
		res = virtualpbp_lseek(fd, offset, whence);		
	}

	pspSdkSetK1(k1);

	if (res < 0)
		return sceIoLseek32(fd, offset, whence);

	return res;
}

int sceIoGetstatPatched(const char *file, SceIoStat *stat)
{
	u32 k1 = pspSdkSetK1(0);
	int index;

	Fix150Path(file);
	Fix303Path(file);

	index = GetIsoIndex(file);
	if (index >= 0)
	{
		int res = virtualpbp_getstat(index, stat);
		
		pspSdkSetK1(k1);
		return res;
	}

	pspSdkSetK1(k1);

	return sceIoGetstat(file, stat);
}

int sceIoChstatPatched(const char *file, SceIoStat *stat, int bits)
{
	u32 k1 = pspSdkSetK1(0);
	int index;

	Fix150Path(file);
	Fix303Path(file);

	index = GetIsoIndex(file);
	if (index >= 0)
	{
		int res = virtualpbp_chstat(index, stat, bits);
		
		pspSdkSetK1(k1);
		return res;
	}

	pspSdkSetK1(k1);

	return sceIoChstat(file, stat, bits);
}

int sceIoRemovePatched(const char *file)
{
	u32 k1 = pspSdkSetK1(0);
	int index;

	Fix150Path(file);
	Fix303Path(file);

	index = GetIsoIndex(file);
	if (index >= 0)
	{
		int res = virtualpbp_remove(index);
		
		pspSdkSetK1(k1);
		return res;
	}

	pspSdkSetK1(k1);

	return sceIoRemove(file);
}

int sceIoRmdirPatched(const char *path)
{
	u32 k1 = pspSdkSetK1(0);
	int index;

	Fix150Path(path);
	Fix303Path(path);

	index = GetIsoIndex(path);
	if (index >= 0)
	{
		int res = virtualpbp_open(index);
		
		pspSdkSetK1(k1);
		return res;
	}

	pspSdkSetK1(k1);

	return sceIoRmdir(path);
}

int sceRegSetKeyValuePatched(REGHANDLE hd, const char *name, void *buf, SceSize size)
{
	int k1 = pspSdkSetK1(0);

	if (strcmp(name, "language") == 0)
	{
		int lang = *(int *)buf;

		if (lang == 0x09 /* korean */ ||
		    lang == 0x0A /* chinese */ ||
			lang == 0x0B) /* the other chinese */
		{
			RebootVSHWithError(0x98765432);
			// We shouldn't get here, but if that, then let's loop forever
			while (1);
		}
	}
	
	pspSdkSetK1(k1);

	return sceRegSetKeyValueReal(hd, name, buf, size);
}

void IoPatches()
{
	u32 *mod, text_addr;

	mod = (u32 *)sceKernelFindModuleByName("sceIOFileManager");

	if (mod)
	{	
		text_addr = *(mod+27);
		
		// Patcth IoFileMgrForUser nids 
		PatchSyscall(text_addr+0x055C, sceIoDopenPatched);
		PatchSyscall(text_addr+0x06A8, sceIoDreadPatched);
		PatchSyscall(text_addr+0x0758, sceIoDclosePatched);
		PatchSyscall(text_addr+0x2C00, sceIoOpenPatched);
		PatchSyscall(text_addr+0x2BC0, sceIoClosePatched);
		PatchSyscall(text_addr+0x2D18, sceIoReadPatched);
		PatchSyscall(text_addr+0x2D88, sceIoLseekPatched);
		PatchSyscall(text_addr+0x2DC0, sceIoLseek32Patched);
		PatchSyscall(text_addr+0x2EB4, sceIoGetstatPatched);
		PatchSyscall(text_addr+0x2ED4, sceIoChstatPatched);
		PatchSyscall(text_addr+0x07F8, sceIoRemovePatched);
		PatchSyscall(text_addr+0x2E74, sceIoRmdirPatched);
	}

	mod = (u32 *)sceKernelFindModuleByName("sceMSFAT_Driver"); 
	if (mod)
	{
		text_addr = *(mod+27);

		// Make an odd patch to allow sceIoGetstat be called correctly in kernel mode
		_sw(NOP, text_addr+0x2228);
	}
}

void PatchVshMain(char *buf)
{
	int intr = sceKernelCpuSuspendIntr();

	u32 text_addr = (u32)buf+0xA0;

	_sw(NOP, (u32)(text_addr+0xE5C8));
	_sw(NOP, (u32)(text_addr+0xE5D0)); 

	sceRegSetKeyValueReal = (void *)FindProc("sceRegistry_Service", "sceReg", 0x17768E14);
					
	if (sceRegSetKeyValueReal)
	{
		PatchSyscall((u32)sceRegSetKeyValueReal, sceRegSetKeyValuePatched);
	}

	IoPatches();

	sceKernelCpuResumeIntr(intr);
	ClearCaches();	
}

////////////////////////////////////////////////////////////////

/* Note: the compiler has to be configured in makefile to make 
   sizeof(wchar_t) = 2. */
wchar_t verinfo[] = L"3.03 OE-A'";

void PatchSysconfPlugin(char *buf)
{
	u32 addrlow;
	int intr = sceKernelCpuSuspendIntr();
	
	u32 text_addr = (u32)buf+0xA0;

	memcpy((void *)(text_addr+0x19818), verinfo, sizeof(verinfo));
	addrlow = _lh(text_addr+0xCBD0) & 0xFFFF;
	addrlow -= 0x3808; /* address for ver info */				
				
	// addiu a1, s4, addrlow
	_sw(0x26850000 | addrlow, text_addr+0xCD84);

	sceKernelCpuResumeIntr(intr);	
	ClearCaches();
}

//////////////////////////////////////////////////

void *block;
int drivestat = SCE_UMD_READY | SCE_UMD_MEDIA_IN;
SceUID umdcallback;

void UmdCallback()
{
	if (umdcallback >= 0)
	{
		sceKernelNotifyCallback(umdcallback, drivestat);
	}
}

int sceUmdActivatePatched(const int mode, const char *aliasname)
{
	int k1 = pspSdkSetK1(0);
	//sceIoAssign(aliasname, block, block+6, IOASSIGN_RDONLY, NULL, 0);
	sceIoAssign(aliasname, "umd0:", "isofs0:", IOASSIGN_RDONLY, NULL, 0);
	pspSdkSetK1(k1);	
	
	drivestat = SCE_UMD_READABLE | SCE_UMD_MEDIA_IN;	
	UmdCallback();
	return 0;
}

int sceUmdDeactivatePatched(const int mode, const char *aliasname)
{
	sceIoUnassign(aliasname);
	drivestat = SCE_UMD_MEDIA_IN | SCE_UMD_READY;
	
	UmdCallback();
	return 0;
}

int sceUmdGetDiscInfoPatched(SceUmdDiscInfo *disc_info)
{
	disc_info->uiSize = 8; 
	disc_info->uiMediaType = SCE_UMD_FMT_GAME; 

	return 0;
}

int sceUmdRegisterUMDCallBackPatched(SceUID cbid)
{
	umdcallback = cbid;
	UmdCallback();

	return 0;
}

int sceUmdUnRegisterUMDCallBackPatched(SceUID cbid)
{
	umdcallback = -1;
	return 0;
}

int sceUmdGetDriveStatPatched()
{
	return drivestat;
}

u32 FindUmdUserFunction(u32 nid)
{
	return FindProc("sceUmd_driver", "sceUmdUser", nid);
}

void PatchIsofsDriver(char *buf)
{
	// Patch StopModule to avoid crash at exit...
	//u32 *mod = (u32 *)sceKernelFindModuleByName("sceIsofs_driver");
	//u32 text_addr = *(mod+27);
	u32 text_addr = (u32)(buf+0xA0);

	char *iso = GetUmdFile();

	if (iso)
	{
		if (strstr(iso, "ms0:/") == iso)
		{
			if (config.usenoumd || config.useisofsonumdinserted)
			{
				/* make module exit inmediately */
				_sw(0x03E00008, text_addr+0x4238);
				_sw(0x24020001, text_addr+0x423C);

				ClearCaches();
			}
		}
	}

	/*_sw(0x03e00008, text_addr+0x4a2c);
	_sw(0x34020000, text_addr+0x4a30);*/
}

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
	int (* sceChkregGetPsCode)(u8 *);
	u8 code[8];

	if (pscode)
	{
		if (config.fakeregion)
		{
			REDIRECT_FUNCTION(pscode, sceChkregGetPsCodePatched);			
		}
		else
		{
			sceChkregGetPsCode = (void *)pscode;

			memset(code, 0, 8);
			sceChkregGetPsCode(code);

			if (code[2] == 0x06 || code[2] == 0x0A || code[2] == 0x0B
				|| code[2] == 0x0D)
			{
				REDIRECT_FUNCTION(pscode, sceChkregGetPsCodePatched);
			}
		}
	}

	u32 checkregion = FindProc("sceChkreg", "sceChkreg_driver", 0x54495B19);

	if (checkregion)
	{
		if (config.freeumdregion)
		{
			MAKE_DUMMY_FUNCTION1(checkregion);
		}
	}

	ClearCaches();
}

void SetSpeed()
{
	scePowerSetClockFrequency = (void *)FindPowerDriverFunction(0x545A7F3C);
	scePowerSetClockFrequency(config.umdisocpuspeed, config.umdisocpuspeed, config.umdisobusspeed);
	
}

void OnImposeLoad()
{
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceChkreg");

	if (mod)
	{
		sctrlSEGetConfig(&config);
		PatchChkreg();
	}
	
	if (sceKernelInitKeyConfig() == PSP_INIT_KEYCONFIG_GAME)
	{
		char *theiso = GetUmdFile();

		if (theiso)
		{
			if (strncmp(theiso, "ms0:/", 5) == 0)
			{
				if (config.usenoumd || config.useisofsonumdinserted)
				{
					sceIoDelDrv("isofs");
					sceIoAddDrv(getisofs_driver());

					//PatchIsofsDriver();
					
					if (config.usenoumd)
					{
						DoNoUmdPatches();
					}

					sceIoAssign("disc0:", "umd0:", "isofs0:", IOASSIGN_RDONLY, NULL, 0);
				}
			}			
		}		
	}

	if (sceKernelInitApitype() == PSP_INIT_APITYPE_DISC)
	{
		if (config.umdisocpuspeed == 333 || config.umdisocpuspeed == 300 ||
			config.umdisocpuspeed == 266 || config.umdisocpuspeed == 222)
			SetSpeed();
	}
}





