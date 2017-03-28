#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include <psploadexec_kernel.h>
#include <pspreg.h>
#include <pspctrl.h>
#include <psprtc.h>
#include <pspusb.h>
#include <pspusbstor.h>
#include <psppower.h>
#include <umd9660_driver.h>
#include <isofs_driver.h>
#include <virtualpbpmgr.h>
#include <systemctrl.h>
#include <systemctrl_se.h>
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PSP_MODULE_INFO("VshControl", 0x1007, 1, 1);
PSP_MODULE_SDK_VERSION(0x03060010);

#define EBOOT_BIN "disc0:/PSP_GAME/SYSDIR/EBOOT.BIN"
#define BOOT_BIN  "disc0:/PSP_GAME/SYSDIR/BOOT.BIN"
#define PROGRAM	  "ms0:/PSP/GAME/BOOT/EBOOT.PBP"

void *oe_malloc(int size);
int oe_free(void *ptr);

SEConfig config;
u8 videoiso_mounted;

void ClearCaches()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}

/*void WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	sceIoWrite(fd, buf, size);
	sceIoClose(fd);
}*/

int LoadReboot(char *file)
{
	SceUID mod = sceKernelLoadModule("flash0:/kd/reboot150.prx", 0, NULL);

	if (mod < 0)
	{
		mod = sceKernelLoadModule("ms0:/seplugins/reboot150.prx", 0, NULL);
	}

	if (mod < 0)
		return mod;

	return sceKernelStartModule(mod, 0, NULL, NULL, NULL);
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

void Fix380Path(const char *file)
{
	char str[256];

	if (strstr(file, "ms0:/PSP/GAME/") == file)
	{
		strcpy(str, (char *)file);

		char *p = strstr(str, "__380");
		
		if (p)
		{
			strcpy((char *)file+13, "380/");
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
	struct SceKernelLoadExecVSHParam param;	
	u32 vshmain_args[0x20/4];

	memset(&param, 0, sizeof(param));
	memset(vshmain_args, 0, sizeof(vshmain_args));

	vshmain_args[0/4] = 0x0400;
	vshmain_args[4/4] = 0x20;
	vshmain_args[0x14/4] = error;

	param.size = sizeof(param);
	param.args = 0x400;
	param.argp = vshmain_args;
	param.vshmain_args_size = 0x400;
	param.vshmain_args = vshmain_args;
	param.configfile = "/kd/pspbtcnf.txt";

	sctrlKernelExitVSH(&param);
}

int LoadExecVSHCommonPatched(int apitype, char *file, struct SceKernelLoadExecVSHParam *param, int unk2)
{
	int k1 = pspSdkSetK1(0);
	int rebooterror = 0;
	int index;
	int reboot150 = 0;

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

		if (config.umdmode == MODE_MARCH33)
		{
			sctrlSESetBootConfFileIndex(1);			
		}
		else if (config.umdmode == MODE_NP9660)
		{
			sctrlSESetBootConfFileIndex(2);
		}

		pspSdkSetK1(k1);
		return sctrlKernelLoadExecVSHWithApitype(0x120, file, param);	
	}

	Fix150Path(file);
	Fix150Path(param->argp);
	Fix380Path(file);
	Fix380Path(param->argp);	

	if (strstr(file, "ms0:/PSP/GAME150/") == file && !strstr(file, "ms0:/PSP/GAME/UPDATE/"))
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

	else if (strstr(file, "EBOOT.BIN"))
	{
		if (config.executebootbin)
		{
			strcpy(file, BOOT_BIN);	
			param->argp = file;
		}
	}
	
	if (rebooterror)
	{
		RebootVSHWithError(0xDADADADA);
	}

	param->args = strlen(param->argp) + 1; // update length

	if (reboot150 == 1)
	{
		LoadReboot(file);		
	}

	pspSdkSetK1(k1);

	return sctrlKernelLoadExecVSHWithApitype(apitype, file, param);
}

void ReturnToDisc()
{
	sctrlSEUmountUmd();
	sceKernelCallSubIntrHandler(0x04,0x1a,0,0);
	videoiso_mounted = 0;
}

int (* sceRegSetKeyValueReal)(REGHANDLE hd, const char *name, void *buf, SceSize size);

SceUID gamedfd = -1, game150dfd = -1, game380dfd = -1, isodfd = -1, over380 = 0, overiso = 0;
SceUID paramsfo = -1;
int vpbpinited = 0, isoindex = 0, cachechanged = 0;
VirtualPbp *cache = NULL;
u8 referenced[32];

SceUID sceIoDopenPatched(const char *dirname)
{
	int res, g150 = 0, index;
	int k1 = pspSdkSetK1(0);

	Fix150Path(dirname);
	Fix380Path(dirname);	

	index = GetIsoIndex(dirname);
	if (index >= 0)
	{
		int res = virtualpbp_open(index);
		
		pspSdkSetK1(k1);
		return res;
	}

	if (strcmp(dirname, "ms0:/PSP/GAME") == 0)
	{
		g150 = 1;		
	}

	pspSdkSetK1(k1);
	res = sceIoDopen(dirname);
	pspSdkSetK1(0);

	if (g150)
	{
		gamedfd = res;
		game150dfd = sceIoDopen("ms0:/PSP/GAME150");
		over380 = 0;
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
		cache = (VirtualPbp *)oe_malloc(32*sizeof(VirtualPbp));		
	}

	memset(cache, 0, sizeof(VirtualPbp)*32);
	memset(referenced, 0, sizeof(referenced));

	for (i = 0; i < 0x10; i++)
	{
		fd = sceIoOpen("ms0:/PSP/SYSTEM/isocache.bin", PSP_O_RDONLY, 0);

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

	if (vpbpinited)
	{
		res = virtualpbp_dread(fd, dir);		
		if (res >= 0)
		{
			pspSdkSetK1(k1);
			return res;
		}
	}
	
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

			if (game150dfd < 0 && game380dfd < 0 && isodfd < 0 && !over380)
			{
				game380dfd = sceIoDopen("ms0:/PSP/GAME380");
				if (game380dfd < 0)
				{
					over380 = 1;
				}
			}
			
			if (game380dfd >= 0)
			{
				if ((res = sceIoDread(game380dfd, dir)) > 0)
				{
					ApplyNamePatch(dir, "__380");					
					pspSdkSetK1(k1);
					return res;
				}
				else
				{
					sceIoDclose(game380dfd);
					game380dfd = -1;
					over380 = 1;
				}
			}

			if (game150dfd < 0 && game380dfd < 0 && isodfd < 0 && !overiso)
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
NEXT:
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
							
							if (videoiso_mounted)
								ReturnToDisc();
						}
					}
					else
					{
						goto NEXT;
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
	int res;

	if (vpbpinited)
	{	
		res = virtualpbp_close(fd);	
		if (res >= 0)
		{
			pspSdkSetK1(k1);
			return res;
		}
	}

	if (fd == gamedfd)
	{
		gamedfd = -1;
		over380 = 0;
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
	Fix380Path(file);

	index = GetIsoIndex(file);
	if (index >= 0)
	{
		if (videoiso_mounted)
			ReturnToDisc();
		
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
						memcpy(data+i, "3.90", 4);
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
	Fix380Path(file);

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
	Fix380Path(file);

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
	Fix380Path(file);

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
	Fix380Path(path);

	index = GetIsoIndex(path);
	if (index >= 0)
	{
		int res = virtualpbp_rmdir(index);
		
		pspSdkSetK1(k1);
		return res;
	}

	pspSdkSetK1(k1);

	return sceIoRmdir(path);
}

int sceIoMkdirPatched(const char *dir, SceMode mode)
{
	int k1 = pspSdkSetK1(0);

	if (strcmp(dir, "ms0:/PSP/GAME") == 0)
	{
		sceIoMkdir("ms0:/PSP/GAME150", mode);
		sceIoMkdir("ms0:/PSP/GAME380", mode);
		sceIoMkdir("ms0:/ISO", mode);
		sceIoMkdir("ms0:/ISO/VIDEO", mode);
	}

	pspSdkSetK1(k1);
	return sceIoMkdir(dir, mode);
}

///////////////////////////////////////////////////////

u64 firsttick;
u8 set;
u8 return_iso;

int (* vshmenu_ctrl)(SceCtrlData *pad_data, int count);

static SceUID satelite = -1, usbdev = -1;
static SceKernelLMOption lmoption;

int sceCtrlReadBufferPositivePatched(SceCtrlData *pad_data, int count)
{
	int res = sceCtrlReadBufferPositive(pad_data, count);
	int k1 = pspSdkSetK1(0);
	
	if (!set && config.vshcpuspeed != 0)
	{	
		u64 curtick;
		u32 t;

		sceRtcGetCurrentTick(&curtick);
		curtick -= firsttick;
		t = (u32)curtick;

		if (t >= (10*1000*1000))
		{
			set = 1;
			SetSpeed(config.vshcpuspeed, config.vshbusspeed);
		}
	}
	
	if (!sceKernelFindModuleByName("VshCtrlSatelite"))
	{
		if (!sceKernelFindModuleByName("htmlviewer_plugin_module") && !sceKernelFindModuleByName("sceVshOSK_Module"))
		{
			if (pad_data->Buttons & PSP_CTRL_SELECT)
			{
				sceKernelSetDdrMemoryProtection((void *)0x08400000, 4*1024*1024, 0xF); 

				lmoption.size = sizeof(lmoption);
				lmoption.mpidtext = 5;
				lmoption.mpiddata = 5;
				lmoption.access = 1;
				satelite = sceKernelLoadModule("flash0:/vsh/module/satelite.prx", 0, &lmoption);

				if (satelite >= 0)
				{
					char *argp;
					SceSize args;

					if (videoiso_mounted)
					{
						argp = GetUmdFile();
						args = strlen(argp)+1;
					}
					else
					{
						argp = NULL;
						args = 0;
					}
					
					sceKernelStartModule(satelite, args, argp, NULL, NULL);

					pad_data->Buttons &= ~(PSP_CTRL_SELECT);
				}
			}
		}
	}
	else
	{
		if (vshmenu_ctrl)
		{
			vshmenu_ctrl(pad_data, count);
		}
		else if (satelite >= 0)
		{
			if (sceKernelStopModule(satelite, 0, NULL, NULL, NULL) >= 0)
			{
				sceKernelUnloadModule(satelite);
			}
		}
	}

	pspSdkSetK1(k1);
	return res;
}

int vctrlVSHRegisterVshMenu(int (* ctrl)(SceCtrlData *, int))
{
	int k1 = pspSdkSetK1(0);

	vshmenu_ctrl = (void *)(0x80000000 | (u32)ctrl);

	pspSdkSetK1(k1);
	return 0;
}

int vctrlVSHExitVSHMenu(SEConfig *conf, char *videoiso, int disctype)
{
	int k1 = pspSdkSetK1(0);
	int oldspeed = config.vshcpuspeed;
	
	vshmenu_ctrl = NULL;
	memcpy(&config, conf, sizeof(SEConfig));
	SetConfig(&config);

	if (set)
	{
		if (config.vshcpuspeed != oldspeed)
		{		
			if (config.vshcpuspeed)			
				SetSpeed(config.vshcpuspeed, config.vshbusspeed);
			else
				SetSpeed(222, 111);
		}
	}

	if (!videoiso)
	{
		if (videoiso_mounted)
			ReturnToDisc();
	}
	else
	{
		sctrlSESetDiscType(disctype);

		if (!videoiso_mounted)
		{
			sctrlSESetDiscOut(1);	
			videoiso_mounted = 1;
		}
		else
		{
			if (strcmp(GetUmdFile(), videoiso) == 0)
			{
				pspSdkSetK1(k1);
				return 0;
			}

			sctrlSEUmountUmd();
		}

		sctrlSEMountUmdFromFile(videoiso, 0, 0);
	}

	pspSdkSetK1(k1);
	return 0;
}

int sceUsbStartPatched(const char* driverName, int size, void *args)
{
	int k1 = pspSdkSetK1(0);
	
	if (!strcmp(driverName, PSP_USBSTOR_DRIVERNAME) && config.usbdevice >= 1 && config.usbdevice <= 5)
	{
		if (videoiso_mounted && config.usbdevice == 5)
		{
			return_iso = 1;
			ReturnToDisc();
		}		
		
		usbdev = sceKernelLoadModule("flash0:/kd/usbdevice.prx", 0, NULL);
		if (usbdev >= 0)
		{
			if (sceKernelStartModule(usbdev, 0, NULL, NULL, NULL) >= 0)
			{
				int res = pspUsbDeviceSetDevice(config.usbdevice-1, 0, 0);
				if (res < 0)
				{
					pspUsbDeviceFinishDevice();
				}				
			}
		}
	}

	pspSdkSetK1(k1);
	return sceUsbStart(driverName, size, args);
}

int sceUsbStopPatched(const char* driverName, int size, void *args)
{
	int res = sceUsbStop(driverName, size, args);
	int k1 = pspSdkSetK1(0);
		
	if (!strcmp(driverName, PSP_USBSTOR_DRIVERNAME) && usbdev >= 0 && config.usbdevice >= 1 && config.usbdevice <= 5)
	{
		pspUsbDeviceFinishDevice();

		if (sceKernelStopModule(usbdev, 0, NULL, NULL, NULL) >= 0)
		{
			sceKernelUnloadModule(usbdev);
			usbdev = -1;
		}
		
		if (return_iso && config.usbdevice == 5)
		{
			sctrlSESetDiscOut(1);	
			sctrlSEMountUmdFromFile(GetUmdFile(), 0, 0);
			videoiso_mounted = 1;
			return_iso = 0;
		}
	}

	pspSdkSetK1(k1);
	return res;
}

///////////////////////

int sceIoDevctlPatched(const char *dev, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
	int res = sceIoDevctl(dev, cmd, indata, inlen, outdata, outlen);
	
	if (cmd == 0x01E18030 && videoiso_mounted)
	{
		res = 1;
	}

	return res;
}

/*int (* CheckRegion)(int a, int b);

int CheckRegionPatched(int a, int b)
{
	Kprintf("CheckRegion %08X  %08X.\n", a, b);
	int res = CheckRegion(a, b);
	Kprintf("CheckRegion res %d.\n", res);
	return res;
}

int (* UmdValidate)(u32 a, u32 b);
int UmdValidatePatched(u32 a, u32 b)
{
	Kprintf("UmdValidate %08X  %08X.\n", a, b);
	int res = UmdValidate(a, b);
	Kprintf("UmdValidate res %08X\n", res);
	return res;
}

int (* AtaFunc)(u32 *x, u32 *y);
int AtaFuncPatched(u32 *x, u32 *y)
{
	Kprintf("AtaFunc x[0] %08X, x[1] %08X, x[2] %08X, y[0] %08X, y[1] %08X, y[2] %08X.\n", x[0], x[1], x[2], y[0], y[1], y[2]);
	int res = AtaFunc(x, y);
	Kprintf("Result of AtaFunc %08X.\n", res);
	return res;
}

int (* Umd1F9A)(int);

int Umd1F9APatched(int x)
{
	int res = Umd1F9A(x);
	Kprintf("Res of Umd1F9A: %08X.\n", res);
	return res;
}

int (* Umdx9)(u32 *, int, int, int);

int Umdx9Patched(u32 *x, int y, int u, int v)
{
	Kprintf("Umdx9 %08X %08X %08X %08X x[4] %08X x[5] %08X.\n", x, y, u, v, x[4], x[5]);
	x[4] = 0x22;	
	int res = Umdx9(x, y, u, v);
	Kprintf("Umdx9 res %08X.\n", res);
	return res;
}*/

void IoPatches()
{
	SceModule2 *mod;
	u32 text_addr;

	mod = sceKernelFindModuleByName("sceIOFileManager");
	text_addr = mod->text_addr;

	// Patcth IoFileMgrForUser syscalls 
	PatchSyscall(text_addr+0x142C, sceIoDopenPatched);
	PatchSyscall(text_addr+0x15A8, sceIoDreadPatched);
	PatchSyscall(text_addr+0x1658, sceIoDclosePatched);
	PatchSyscall(text_addr+0x3C70, sceIoOpenPatched);
	PatchSyscall(text_addr+0x3C30, sceIoClosePatched);
	PatchSyscall(text_addr+0x3D88, sceIoReadPatched);
	PatchSyscall(text_addr+0x3DF8, sceIoLseekPatched);
	PatchSyscall(text_addr+0x3E30, sceIoLseek32Patched);
	PatchSyscall(text_addr+0x3F24, sceIoGetstatPatched);
	PatchSyscall(text_addr+0x3F44, sceIoChstatPatched);
	PatchSyscall(text_addr+0x16F8, sceIoRemovePatched);
	PatchSyscall(text_addr+0x3EE4, sceIoRmdirPatched);
	PatchSyscall(text_addr+0x3EC8, sceIoMkdirPatched);
	//PatchSyscall(text_addr+0x0930, sceIoRenamePatched);

	// Fatmsmod changed in 3.80+, and patch is not needed anymore
	// Make an odd patch to allow sceIoGetstat be called correctly in kernel mode
	// mod = sceKernelFindModuleByName("sceMSFAT_Driver"); 
	//_sw(NOP, mod->text_addr+0x22D8);	
}

static int IsMarch33()
{
	pspTime time;

	sceRtcGetCurrentClockLocalTime(&time);

	if (time.day == 2 && time.month == 4)
		return 1;

	return 0;
}

int sceRtcGetCurrentClockLocalTimePatched(pspTime *time)
{
	int res = sceRtcGetCurrentClockLocalTime(time);
	int k1 = pspSdkSetK1(0);

	if (time->day == 2 && time->month == 4)
	{
		time->day = 33;
		time->month = 3;
	}

	pspSdkSetK1(k1);
	return res;
}

void PatchVshMain(u32 text_addr)
{
	// Allow old sfo's.
	_sw(NOP, (u32)(text_addr+0xEAFC));
	_sw(NOP, (u32)(text_addr+0xEB04)); 

	IoPatches();

	SceModule2 *mod = sceKernelFindModuleByName("sceVshBridge_Driver");

	if (!config.novshmenu)
	{
		MAKE_CALL(mod->text_addr+0x1D0, sceCtrlReadBufferPositivePatched);
		PatchSyscall(FindProc("sceController_Service", "sceCtrl", 0x1F803938), sceCtrlReadBufferPositivePatched);
	}	

	// For umd video iso
	MAKE_CALL(mod->text_addr+0x5CC, sceIoDevctlPatched);

	PatchSyscall(FindProc("sceUSB_Driver", "sceUsb", 0xAE5DE6AF), sceUsbStartPatched);
	PatchSyscall(FindProc("sceUSB_Driver", "sceUsb", 0xC2464FA0), sceUsbStopPatched);


	if (IsMarch33())
	{
		mod = sceKernelFindModuleByName("scePaf_Module");
		if (mod)
		{
			// Change sceGmoModelAnimate(1.0f / 60.0f) -> sceGmoModelAnimate(12.0f / 60.0f)
		
			_sw(_lw(mod->text_addr+0xFB87C)-4, mod->text_addr+0xFB87C); // change address of float loading by previous unused space
			// Change the before unused variable to 12.0f/60.0f
			_sw(0x3E4CCCCC, mod->text_addr+0x1A38EC);

		}
	}

	ClearCaches();	
}

////////////////////////////////////////////////////////////////

/* Note: the compiler has to be configured in makefile to make 
   sizeof(wchar_t) = 2. */
wchar_t verinfo[] = L"3.90 M33  ";
char nick[] = "March 33";

void PatchSysconfPlugin(u32 text_addr)
{
	u32 addrlow, addrhigh;		
	int version = sctrlSEGetVersion() & 0xF;

	if (version)
	{
		((char *)verinfo)[16] = '-';
		((char *)verinfo)[18] = version+'1';
	}

	if (IsMarch33())
	{
		((char *)verinfo)[4] = ((char *)verinfo)[6] = '3';

		// Change MAC
		_sw(0x24070033, text_addr+0x12170);
		_sw(0x24080033, text_addr+0x12174);
		_sw(0x24090033, text_addr+0x12178);
		_sw(0x240A0033, text_addr+0x1217c);
		_sw(0x240B0033, text_addr+0x12180);
		_sw(0x24020033, text_addr+0x12184);

		// Change nick
		memcpy((void *)(text_addr+0x20B88), nick, sizeof(nick));
		addrhigh = (text_addr+0x20B88) >> 16;
		addrlow = (text_addr+0x20B88) & 0xFFFF;

		// lui s1, addrhigh
		_sw(0x3c110000 | addrhigh, text_addr+0x12324);
		// ori s1, s1, addrlow
		_sw(0x36310000 | addrlow, text_addr+0x12328);
		_sw(0x10000014, text_addr+0x1232c);
	}
	
	memcpy((void *)(text_addr+0x20B54), verinfo, sizeof(verinfo));
	
	addrhigh = (text_addr+0x20B54) >> 16;
	addrlow = (text_addr+0x20B54) & 0xFFFF;

	// lui v0, addrhigh
	_sw(0x3c020000 | addrhigh, text_addr+0x1242C);
	// ori v0, v0, addrlow
	_sw(0x34420000 | addrlow, text_addr+0x12430);

	ClearCaches();
}

#ifndef USE_STATIC_PATCHES

void PatchMsVideoMainPlugin(u32 text_addr)
{
	/* 3.10
	// Patch resolution limit to 130560 pixels (480x272)
	_sh(0xfe00, text_addr+0x33FD8); // Allow play avc <= 480*272
	_sh(0xfe00, text_addr+0x34040);
	//_sh(0xfe00, text_addr+0x35F64); // show standard mp4 <= 480*272... they won't play anyways
	_sh(0xfe00, text_addr+0x35FD8);
	_sh(0xfe00, text_addr+0x3608C); // show avc <= 480x272
	_sh(0xfe00, text_addr+0x3C958);
	_sh(0xfe00, text_addr+0x3C9BC);
	_sh(0xfe00, text_addr+0x3CA24);
	_sh(0xfe00, text_addr+0x48EB0);
	_sh(0xfe00, text_addr+0x48FF4);
	_sh(0xfe00, text_addr+0x4C8D0);*/

	// 3.90
	// Patch resolution limit to 130560 pixels (480x272)
	_sh(0xfe00, text_addr+0x2CE64);
	_sh(0xfe00, text_addr+0x2CED0);
	_sh(0xfe00, text_addr+0x2F040);
	_sh(0xfe00, text_addr+0x2F0B4);
	_sh(0xfe00, text_addr+0x2F174);
	_sh(0xfe00, text_addr+0x34FF4);
	_sh(0xfe00, text_addr+0x35108);
	_sh(0xfe00, text_addr+0x581B0);
	
	// Patch bitrate limit 768+2 (increase to 16384+2)
	_sh(0x4003, text_addr+0x2EFDC);
	_sh(0x4003, text_addr+0x2F068);

	// Patch bitrate limit 4000 (increase to 16384+2)
	_sh(0x4003, text_addr+0x2F110);
	_sh(0x4003, text_addr+0x346B0);
	
	ClearCaches();
}

void PatchGamePlugin(u32 text_addr)
{
	/*_sw(0x1000ffdd, text_addr+0x1090C);
	_sw(0x24030000, text_addr+0x10910);*/

	// New Patch (3.71+)	
	_sw(0x03e00008, text_addr+0x1020C);
	_sw(0x00001021, text_addr+0x10210);	

	if (config.hidepics)
	{
		// Hide pic0.png+pic1.png
		// mov v0, v1
		_sw(0x00601021, text_addr+0xE3AC);
		_sw(0x00601021, text_addr+0xE3B8);
	}

	// Patch to bypass videout check (3.71)... but still games won't work when booted in normal cable
	//_sw(0, text_addr+0xD64C);
	//_sw(0x5000ffec, text_addr+0xD654);
	
	ClearCaches();			
}

#endif

char update_url[] = "http://updates.dark-alex.org/updatelist.txt";

int up_url_offsets[16] = 
{ 
	0x0000ABD0, 0x0000AC14, 0x0000AC58, 0x0000AC9C,
	0x0000ACE4, 0x0000AD2C, 0x0000AD74, 0x0000ADBC,
	0x0000AE04, 0x0000AE4C, 0x0000AE94, 0x0000AEDC,
	0x0000AF24, 0x0000AF6C, 0x0000AFB4, 0x0000AFFC
};

void PatchUpdatePlugin(u32 text_addr)
{
	int se_version, high, low, i;

	se_version = sctrlSEGetVersion();
	high = (se_version >> 16);
	low = (se_version&0xFFFF);
	
	// lui a1, high
	_sw(0x3C050000 | high, text_addr+0x1818); 
	// ori a0, a1, low
	_sw(0x34A40000 | low, text_addr+0x19A0); 
     
	for(i = 0; i < 16; i++)
	{
		strcpy((char*)(text_addr+up_url_offsets[i]), update_url);
	}

	ClearCaches();
}

STMOD_HANDLER previous;

int OnModuleStart(SceModule2 *mod)
{
	char *modname = mod->modname;
	u32 text_addr = mod->text_addr;

	if (strcmp(modname, "vsh_module") == 0)
	{
		PatchVshMain(text_addr);
	}
	
	else if (strcmp(modname, "sysconf_plugin_module") == 0)
	{
		PatchSysconfPlugin(text_addr);
	}
			
	else if (strcmp(modname, "msvideo_main_plugin_module") == 0)
	{
		PatchMsVideoMainPlugin(text_addr);
	}

	else if (strcmp(modname, "game_plugin_module") == 0)
	{
		PatchGamePlugin(text_addr);					
	}

	else if (strcmp(modname, "update_plugin_module") == 0)
	{
		if (!config.notusedaxupd)
			PatchUpdatePlugin(text_addr);
	}


	if (!previous)
		return 0;

	return previous(mod);
}

int module_start(SceSize args, void *argp)
{
	SceModule2 *mod = sceKernelFindModuleByName("sceLoadExec");
	
	MAKE_CALL(mod->text_addr+0x0D60, LoadExecVSHCommonPatched);
	MAKE_CALL(mod->text_addr+0x0D88, LoadExecVSHCommonPatched);
	MAKE_CALL(mod->text_addr+0x0ED8, LoadExecVSHCommonPatched);
	MAKE_CALL(mod->text_addr+0x0F00, LoadExecVSHCommonPatched);

	PatchSyscall(FindProc("sceRTC_Service", "sceRtc", 0xE7C27D1B), sceRtcGetCurrentClockLocalTimePatched);

	sctrlSEGetConfig(&config);	

	if (config.vshcpuspeed != 0)
	{
		sceRtcGetCurrentTick(&firsttick);
	}
	
	ClearCaches();
	previous = sctrlHENSetStartModuleHandler(OnModuleStart);
	
	return 0;
}
