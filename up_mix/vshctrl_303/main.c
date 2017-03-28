#include <pspsdk.h>
#include <pspkernel.h>
#include <psploadexec_kernel.h>
#include <pspreg.h>
#include <psprtc.h>
#include <umd9660_driver.h>
#include <isofs_driver.h>
#include <virtualpbpmgr.h>
#include <systemctrl.h>
#include <systemctrl_se.h>
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PSP_MODULE_INFO("VshControl", 0x1007, 1, 0);

#define EBOOT_BIN "disc0:/PSP_GAME/SYSDIR/EBOOT.BIN"
#define BOOT_BIN  "disc0:/PSP_GAME/SYSDIR/BOOT.BIN"

void *oe_malloc(int size);
int oe_free(void *ptr);

SEConfig config;

void ClearCaches()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}

int LoadStartModule(char *module)
{
	SceUID mod = sceKernelLoadModule(module, 0, NULL);

	if (mod < 0)
		return mod;

	return sceKernelStartModule(mod, strlen(module)+1, module, NULL, NULL);
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

void Fix310Path(const char *file)
{
	char str[256];

	if (strstr(file, "ms0:/PSP/GAME/") == file)
	{
		strcpy(str, (char *)file);

		char *p = strstr(str, "__310");
		
		if (p)
		{
			strcpy((char *)file+13, "310/");
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
	u32 *mod;
	u32 text_addr;
	int reboot150 = 0;
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
		apitype = 0x120;
	}

	Fix150Path(file);
	Fix150Path(param->argp);
	Fix310Path(file);
	Fix310Path(param->argp);	

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

	else
	{

REBOOT:
	
	LoadStartModule("flash0:/kd/reboot310.prx");
	}

	pspSdkSetK1(k1);

	return LoadExecVSHCommon(apitype, file, param, unk2);
}

int (* sceRegSetKeyValueReal)(REGHANDLE hd, const char *name, void *buf, SceSize size);

SceUID gamedfd = -1, game150dfd = -1, game310dfd = -1, isodfd = -1, over310 = 0, overiso = 0;
SceUID paramsfo = -1;
int vpbpinited = 0, isoindex = 0, cachechanged = 0;
VirtualPbp *cache = NULL;
int referenced[32];

SceUID sceIoDopenPatched(const char *dirname)
{
	int res, g150 = 0, index;
	int k1 = pspSdkSetK1(0);

	Fix150Path(dirname);
	Fix310Path(dirname);

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

	res = sceIoDopen(dirname);

	if (g150)
	{
		gamedfd = res;
		game150dfd = sceIoDopen("ms0:/PSP/GAME150");
		over310 = 0;
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

			if (game150dfd < 0 && game310dfd < 0 && isodfd < 0 && !over310)
			{
				game310dfd = sceIoDopen("ms0:/PSP/GAME310");
				if (game310dfd < 0)
				{
					over310 = 1;
				}
			}
			
			if (game310dfd >= 0)
			{
				if ((res = sceIoDread(game310dfd, dir)) > 0)
				{
					ApplyNamePatch(dir, "__310");					
					pspSdkSetK1(k1);
					return res;
				}
				else
				{
					sceIoDclose(game310dfd);
					game310dfd = -1;
					over310 = 1;
				}
			}

			if (game150dfd < 0 && game310dfd < 0 && isodfd < 0 && !overiso)
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
		over310 = 0;
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
	Fix310Path(file);

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
	Fix310Path(file);

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
	Fix310Path(file);

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
	Fix310Path(file);

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
	Fix310Path(path);

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

	if (strcmp(dir, "ms0:/PSP/GAME") == 0 || strcmp(dir, "ms0:/PSP/GAME/") == 0)
	{
		sceIoMkdir("ms0:/PSP/GAME150", mode);
		sceIoMkdir("ms0:/PSP/GAME310", mode);
		sceIoMkdir("ms0:/ISO", mode);
	}

	pspSdkSetK1(k1);
	return sceIoMkdir(dir, mode);
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

	return sceRegSetKeyValue(hd, name, buf, size);
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
		PatchSyscall(text_addr+0x2E58, sceIoMkdirPatched);
		//PatchSyscall(text_addr+0x0930, sceIoRenamePatched);
	}

	mod = (u32 *)sceKernelFindModuleByName("sceMSFAT_Driver"); 
	if (mod)
	{
		text_addr = *(mod+27);

		// Make an odd patch to allow sceIoGetstat be called correctly in kernel mode
		_sw(NOP, text_addr+0x2228);
	}
}

void PatchVshMain(u8 *buf)
{
	int intr = sceKernelCpuSuspendIntr();

	u32 text_addr = (u32)buf+0xA0;

	// Allow old sfo's.
	_sw(NOP, (u32)(text_addr+0xE5C8));
	_sw(NOP, (u32)(text_addr+0xE5D0)); 

	u32 x = FindProc("sceRegistry_Service", "sceReg", 0x17768E14);

	PatchSyscall(x, sceRegSetKeyValuePatched);

	IoPatches();

	sceKernelCpuResumeIntr(intr);
	ClearCaches();	
}

////////////////////////////////////////////////////////////////

/* Note: the compiler has to be configured in makefile to make 
   sizeof(wchar_t) = 2. */
wchar_t verinfo[] = L"3.03/10 OE";

void PatchSysconfPlugin(u8 *buf)
{
	u32 addrlow;
	int intr = sceKernelCpuSuspendIntr();
	
	u32 text_addr = (u32)(buf+0xA0);

	memcpy((void *)(text_addr+0x19818), verinfo, sizeof(verinfo));
	addrlow = _lh(text_addr+0xCBD0) & 0xFFFF;
	addrlow -= 0x3808; /* address for ver info */				
				
	// addiu a1, s4, addrlow
	_sw(0x26850000 | addrlow, text_addr+0xCD84);

	sceKernelCpuResumeIntr(intr);	
	ClearCaches();
}

void PatchMsVideoMainPlugin(u8 *buf)
{
	int intr = sceKernelCpuSuspendIntr();
	u32 text_addr = (u32)(buf+0xA0);

	// Patch resolution limit to 130560 pixels (480x272)
	//_sh(0x0001, text_addr+0x33A18); // Allow play avc <= 480*272
	_sh(0xfe00, text_addr+0x33A1C); // Allow play avc <= 480*272
	//_sh(0x0001, text_addr+0x33A80);
	_sh(0xfe00, text_addr+0x33A84);
	//_sh(0x0005, text_addr+0x35AA4); // show standard mp4 <= 480*272... they won't play anyways
	//_sh(0xfe00, text_addr+0x35AA8); // show standard mp4 <= 480*272... they won't play anyways
	//_sh(0x0001, text_addr+0x35B18);
	_sh(0xfe00, text_addr+0x35B1C);
	//_sh(0x0001, text_addr+0x35BCC); // show avc <= 480x272
	_sh(0xfe00, text_addr+0x35BD0); // show avc <= 480x272
	//_sh(0x0001, text_addr+0x3C578);
	_sh(0xfe00, text_addr+0x3C57C);
	//_sh(0x0001, text_addr+0x3C5DC);
	_sh(0xfe00, text_addr+0x3C5E0);
	//_sh(0x0001, text_addr+0x3C640);
	_sh(0xfe00, text_addr+0x3C648);
	//_sh(0x0001, text_addr+0x48E68);
	_sh(0xfe00, text_addr+0x48E6C);
	//_sh(0x0001, text_addr+0x48FAC);
	_sh(0xfe00, text_addr+0x48FB0);
	//_sh(0x0001, text_addr+0x4BCC4);
	_sh(0xfe00, text_addr+0x4BCC8);

	// Patch bitrate limit	(increase to 16384+2)
	_sh(0x4003, text_addr+0x35B6C);
	_sh(0x4003, text_addr+0x35A44);
	_sh(0x4003, text_addr+0x35AD0);

	sceKernelCpuResumeIntr(intr);
	
	ClearCaches();
}

void PatchGamePlugin(u8 *buf)
{
	u32 text_addr = (u32)(buf+0xA0);
	_sw(0x1000fff9, text_addr+0xB264);					
	ClearCaches();			
}

APRS_EVENT previous;
u64 firsttick;
int set;

int OnModuleRelocated(char *modname, u8 *modbuf)
{
	if (config.vshcpuspeed != 0 && !set)
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

	if (strcmp(modname, "vsh_module") == 0)
	{
		PatchVshMain(modbuf);
	}
	else if (strcmp(modname, "sysconf_plugin_module") == 0)
	{
		PatchSysconfPlugin(modbuf);
	}
						
	else if (strcmp(modname, "msvideo_main_plugin_module") == 0)
	{
		PatchMsVideoMainPlugin(modbuf);
	}

	else if (strcmp(modname, "game_plugin_module") == 0)
	{
		PatchGamePlugin(modbuf);					
	}

	if (!previous)
		return 0;

	return previous(modname, modbuf);
}

int module_start(SceSize args, void *argp)
{
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceLoadExec");
	u32 text_addr = *(mod+27);

	MAKE_CALL(text_addr+0x0930, LoadExecVSHCommonPatched);
	MAKE_CALL(text_addr+0x0958, LoadExecVSHCommonPatched);
	MAKE_CALL(text_addr+0x0A80, LoadExecVSHCommonPatched);
	MAKE_CALL(text_addr+0x0AA8, LoadExecVSHCommonPatched);
	MAKE_CALL(text_addr+0x0AF8, LoadExecVSHCommonPatched);

	sctrlSEGetConfig(&config);

	if (config.vshcpuspeed != 0)
	{
		sceRtcGetCurrentTick(&firsttick);
	}

	previous = sctrlHENSetOnApplyPspRelSectionEvent(OnModuleRelocated);
	
	return 0;
}
