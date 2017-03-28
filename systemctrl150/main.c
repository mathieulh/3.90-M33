#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include <pspdisplay.h>
#include <pspdisplay_kernel.h>
#include <psputilsforkernel.h>
#include <pspctrl.h>
#include <psploadexec.h>
#include <psploadexec_kernel.h>
#include <pspidstorage.h>
#include <psploadcore.h>
#include <pspsysevent.h>
#include <pspreg.h>
#include <stdio.h>
#include <string.h>

#include <systemctrl.h>

#include "reboot.h"


PSP_MODULE_INFO("SystemControl150", 0x3007, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000
#define SC_OPCODE	0x0000000C
#define JR_RA		0x03e00008

#define NOP	0x00000000

#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) & 0x3FFFFFFF) >> 2), a); 
#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x3FFFFFFF) >> 2), a); 
#define MAKE_SYSCALL(a, n) _sw(SC_OPCODE | (n << 6), a);
#define JUMP_TARGET(x) ((x & 0x3FFFFFFF) << 2)
#define REDIRECT_FUNCTION(a, f) _sw(J_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a);  _sw(NOP, a+4);
#define MAKE_DUMMY_FUNCTION0(a) _sw(0x03e00008, a); _sw(0x00001021, a+4);
#define MAKE_DUMMY_FUNCTION1(a) _sw(0x03e00008, a); _sw(0x24020001, a+4);

int (* DoReboot)(int apitype, void *a1, void *a2, void *a3, void *t0);

int (* FlashfatIoOpen)(PspIoDrvFileArg *arg, char *file, int flags, SceMode mode); 
int (* FlashfatIoRemove)(PspIoDrvFileArg *arg, const char *name); 
int (* FlashfatIoMkdir)(PspIoDrvFileArg *arg, const char *name, SceMode mode); 
int (* FlashfatIoRmdir)(PspIoDrvFileArg *arg, const char *name);
int (* FlashfatIoDopen)(PspIoDrvFileArg *arg, const char *dirname);
int (* FlashfatIoGetstat)(PspIoDrvFileArg *arg, const char *file, SceIoStat *stat);
int (* FlashfatIoChstat)(PspIoDrvFileArg *arg, const char *file, SceIoStat *stat, int bits);
int (* FlashfatIoRename)(PspIoDrvFileArg *arg, const char *oldname, const char *newname); 
int (* FlashfatIoChdir)(PspIoDrvFileArg *arg, const char *dir); 

static SceUID start_thread;
static SceModule2 *last_module;
static STMOD_HANDLER stmod_handler = NULL;
static int reboot3XX = 0;
static int stop_redirection = 0;

u32 last_br, last_unk;
int (* GetMsSize)(void);

u8 keyseed[0x20];

void ClearCaches()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheInvalidateAll();
}

int sceKernelGzipDecompressPatched(u8 *dest, u32 destSize, const u8 *src, void *unknown)
{
	u8 *output = (u8 *)0x88fb0000;
	int i;

	for (i = 0; i < 0x100; i++)
	{
		output[i] = 0;		
	}

	output = (u8 *)0xbfc00200;
	for (i = 0; i < 0x10; i++)
	{
		output[i] = keyseed[i];	
	}
		
	if (reboot3XX)
	{		
		sceKernelGzipDecompress((void *)0x88fc0000, 0x4000, rebootex+0x10, NULL);
		int res = sceKernelDeflateDecompress((void *)0x88600000, destSize, reboot+0x10, NULL);

		// Do the following additional patches when going from 1.50 to 3.XX
		_sw(0, 0x886000f0);
		_sw(0, 0x88600108);	

		// Clear M33 vars
		output = (u8 *)0x88fb0000;
		for (i = 0; i < 0x100; i++)
		{
			output[i] = 0;
		} 

		// Set Boot From 150 flag to 1
		_sw(1, 0x88fb00cc);

		return res;
	}

	sceKernelGzipDecompress((void *)0x88fc0000, 0x4000, rebootex150+0x10, NULL);

	return sceKernelGzipDecompress(dest, destSize, src, unknown);
}

int DoRebootPatched(int apitype, void *a1, void *a2, void *a3, void *t0)
{
	if ((apitype & 0x200) == 0x200) /* vsh */
	{
		reboot3XX = 1;
	}

	return DoReboot(apitype, a1, a2, a3, t0);
}

int sceKernelMemsetPatched(void *buf, int ch, int size)
{
	if (reboot3XX)
	{
		buf = (void *)0x88600000;
		size = 0x200000;

		sceKernelSetDdrMemoryProtection((void *)0x88400000, 0x00400000, 0xC);
	}

	return sceKernelMemset(buf, ch, size);
}


#define RECOVERY "flash0:/kd/recovery.prx"
#define BOOT	 "ms0:/PSP/GAME/BOOT/EBOOT.PBP"

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

void LoadPlugins()
{
	u8 plugcon[15];
	char plugin[64];
	int	nvsh = 0, ngame = 0, npops = 0;
	int i;
	SceUID fd;

	memset(plugcon, 0, 20);

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
		sceIoRead(fd, plugcon, 20);
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
			}
			else
			{
				break;
			}
		}

		sceIoClose(fd);
	}

	fd = sceIoOpen("ms0:/seplugins/pops.txt", PSP_O_RDONLY, 0777);
	if (fd >= 0)
	{
		for (i = 0; i < 5; i++)
		{
			memset(plugin, 0, sizeof(plugin));
			if (ReadLine(fd, plugin) > 0)
			{							
				npops++;
			}
			else
			{
				break;
			}
		}
					
		sceIoClose(fd);	
	}	
	
	fd = sceIoOpen("ms0:/seplugins/game150.txt", PSP_O_RDONLY, 0777);
	if (fd >= 0)
	{
		for (i = 0; i < 5; i++)
		{
			memset(plugin, 0, sizeof(plugin));
			if (ReadLine(fd, plugin) > 0)
			{							
				if (plugcon[i+nvsh+ngame+npops])
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

int done = 0;

SceUID sceKernelLoadModuleWithApitypePatched(const char *path, int flags, int apitype, SceKernelLMOption *option)
{
	if (!done && sceKernelFindModuleByName("sceImpose_Driver") && !sceKernelFindModuleByName("sceChkreg"))
	{
		LoadPlugins();
		done = 1;		
	}

	return sceKernelLoadModuleWithApitype(path, flags, apitype, option);
}

int sceDisplaySetBrightnessPatched(u32 level, int unk1)
{
	last_br = level;
	last_unk = unk1;
	
	if (level < 100)
	{
		if (level >= 70)
			level = 100-level;
		else if (level >= 35)
			level = 100-level-5;
		else
			level = 100-level-10;
	}
	else if (level == 100)
	{
		level = 1;
	}

	return sceDisplaySetBrightness(level, unk1);
}

REGHANDLE lang_hk = -1;

int sceRegGetKeyInfoPatched(REGHANDLE hd, const char *name, REGHANDLE *hk, unsigned int *type, SceSize *size)
{
	int res = sceRegGetKeyInfo(hd, name, hk, type, size);
	
	if (res >= 0 && strcmp(name, "language") == 0)
	{
		if (hk)
			lang_hk = *hk;
	}

	return res;
}

int sceRegGetKeyValuePatched(REGHANDLE hd, REGHANDLE hk, void *buf, SceSize size)
{
	int res = sceRegGetKeyValue(hd, hk, buf, size);
	if (res >= 0 && hk == lang_hk)
	{
		if (*(u32 *)buf > 8)
			*(u32 *)buf = 1;

		lang_hk = -1;
	}

	return res;
}

int ValidateSeekPatched(u32 *drv_str, SceOff ofs)
{
	if ((ofs & 0x1FF))
		return 0;

	SceOff max_size = GetMsSize(); // size of partition in sectors
	max_size *= 512; // size in bytes

	if (ofs >= max_size)
		return 0;

	return 1;
}

int ValidateSeekP1Patched(u32 *drv_str, SceOff ofs)
{
	if ((ofs & 0x1FF))
		return 0;

	u32 *p = (u32 *)drv_str[0x10/4];
	u32 *q = (u32 *)p[8/4];
	SceOff max_size = (SceOff)q[4/4]; // partition_start

	max_size = GetMsSize() - max_size; // size of partition in sectors
	max_size *= 512; // size in bytes

	if (ofs >= max_size)
		return 0;

	return 1;
}

SceUID sceKernelCreateThreadPatched(const char *name, SceKernelThreadEntry entry, int initPriority,
                             int stackSize, SceUInt attr, SceKernelThreadOptParam *option)
{
	SceUID thid = sceKernelCreateThread(name, entry, initPriority, stackSize, attr, option);

	if (thid >= 0 && strncmp(name, "SceModmgrStart", 14) == 0)
	{
		start_thread = thid;
		last_module = sceKernelFindModuleByAddress((u32)entry);		
	}

	return thid;
}

int sceKernelStartThreadPatched(SceUID thid, SceSize arglen, void *argp)
{
	if (thid == start_thread)
	{
		start_thread = -1;

		if (last_module && stmod_handler)
		{
			stmod_handler(last_module);
		}
	}

	return sceKernelStartThread(thid, arglen, argp);
}

STMOD_HANDLER sctrlHENSetStartModuleHandler(STMOD_HANDLER handler)
{
	int k1 = pspSdkSetK1(0);
	
	STMOD_HANDLER res = stmod_handler;

	stmod_handler = (STMOD_HANDLER)(0x80000000 | (u32)handler);
	
	pspSdkSetK1(k1);
	return res;
}

void sctrlSEStopFlashRedirection()
{
	int k1 = pspSdkSetK1(0);	
	stop_redirection = 1;
	pspSdkSetK1(k1);	
}

int OnModuleStart(SceModule2 *mod)
{
	char *modname = mod->modname;
	u32 text_addr = mod->text_addr;

	if (strcmp(modname, "scePower_Service") == 0)
	{
		u32 lcd;
		
		_sw(0x00e02021, text_addr+0x558);

		if (sceIdStorageLookup(0x0008, 0, &lcd, 4) >= 0)
		{
			if (lcd == 0x4C434470)
			{
				mod = sceKernelFindModuleByName("sceDisplay_Service");
				_sw((u32)sceDisplaySetBrightnessPatched, mod->text_addr+0x2858);
			}
		}

		ClearCaches();
	}

	else if (strcmp(modname, "sceRegistry_Service") == 0)
	{
		_sw((u32)sceRegGetKeyInfoPatched, text_addr+0x76DC);
		_sw((u32)sceRegGetKeyValuePatched, text_addr+0x76E0);

		ClearCaches();
	}

	else if (strcmp(modname, "sceLflashFatfmt") == 0)
	{
		stop_redirection = 1;
	}

	else if (strcmp(modname, "sceMSstor_Driver") == 0)
	{
		REDIRECT_FUNCTION(text_addr+0x5138, ValidateSeekPatched);
		REDIRECT_FUNCTION(text_addr+0x51BC, ValidateSeekP1Patched);
		GetMsSize = (void *)(text_addr+0x288);

		ClearCaches();
	}

	return 0;
}

char g_file[256];

char *GetNewPath(char *file)
{
	if (stop_redirection)
		return file;
	
	if (strcmp(file, "/kd") == 0 || strcmp(file, "/KD") == 0)
	{
		strcpy(g_file, file);
		g_file[2] = 'm';
		return g_file;
	}

	if (strcmp(file, "/vsh/module") == 0 || strcmp(file, "/VSH/MODULE") == 0)
	{
		strcpy(g_file, file);
		g_file[5] = 'p';
		return g_file;
	}

	if (strncmp(file, "/kd/", 4) == 0 || strncmp(file, "/KD/", 4) == 0)
	{
		strcpy(g_file, file);
		g_file[2] = 'm';
		return g_file;
	}

	if (strncmp(file, "/vsh/module/", 12) == 0 || strncmp(file, "/VSH/MODULE/", 12) == 0)
	{
		strcpy(g_file, file);
		g_file[5] = 'p';
		return g_file;
	}		

	return file;
}

int FlashfatIoOpen_Patched(PspIoDrvFileArg *arg, char *file, int flags, SceMode mode)
{
	if (file)
	{
		return FlashfatIoOpen(arg, GetNewPath(file), flags, mode);
	}

	return FlashfatIoOpen(arg, file, flags, mode);
}

int FlashfatIoRemove_Patched(PspIoDrvFileArg *arg, const char *name)
{
	if (name)
	{
		return FlashfatIoRemove(arg, GetNewPath((char *)name));
	}

	return FlashfatIoRemove(arg, name);
}

int FlashfatIoMkdir_Patched(PspIoDrvFileArg *arg, const char *name, SceMode mode)
{
	if (name)
	{
		return FlashfatIoMkdir(arg, GetNewPath((char *)name), mode);
	}

	return FlashfatIoMkdir(arg, name, mode);
}

int FlashfatIoRmdir_Patched(PspIoDrvFileArg *arg, const char *name)
{
	if (name)
	{
		return FlashfatIoRmdir(arg, GetNewPath((char *)name));
	}

	return FlashfatIoRmdir(arg, name);
}

int FlashfatIoDopen_Patched(PspIoDrvFileArg *arg, const char *dirname)
{
	if (dirname)
	{
		return FlashfatIoDopen(arg, GetNewPath((char *)dirname));
	}

	return FlashfatIoDopen(arg, dirname);
}

int FlashfatIoGetstat_Patched(PspIoDrvFileArg *arg, const char *file, SceIoStat *stat)
{
	if (file)
	{
		return FlashfatIoGetstat(arg, GetNewPath((char *)file), stat);
	}

	return FlashfatIoGetstat(arg, file, stat);
}

int FlashfatIoChstat_Patched(PspIoDrvFileArg *arg, const char *file, SceIoStat *stat, int bits)
{
	if (file)
	{
		return FlashfatIoChstat(arg, GetNewPath((char *)file), stat, bits);
	}

	return FlashfatIoChstat(arg, file, stat, bits);
}

int FlashfatIoRename_Patched(PspIoDrvFileArg *arg, const char *oldname, const char *newname)
{
	if (oldname)
	{
		if (newname)
		{
			return FlashfatIoRename(arg, GetNewPath((char *)oldname), GetNewPath((char *)newname));
		}

		return FlashfatIoRename(arg, GetNewPath((char *)oldname), newname);
	}

	if (newname)
		return FlashfatIoRename(arg, oldname, GetNewPath((char *)newname));

	return FlashfatIoRename(arg, oldname, newname);
}

int FlashfatIoChdir_Patched(PspIoDrvFileArg *arg, const char *dir)
{
	if (dir)
	{
		return FlashfatIoChdir(arg, GetNewPath((char *)dir));
	}

	return FlashfatIoChdir(arg, dir);
}

int sceIoAddDrvPatched(PspIoDrv *drv)
{
	if (drv->name)
	{
		if (strcmp(drv->name, "flashfat") == 0)
		{			
			FlashfatIoOpen = drv->funcs->IoOpen;
			FlashfatIoRemove = drv->funcs->IoRemove;
			FlashfatIoMkdir = drv->funcs->IoMkdir;
			FlashfatIoRmdir = drv->funcs->IoRmdir;
			FlashfatIoDopen = drv->funcs->IoDopen;
			FlashfatIoGetstat = drv->funcs->IoGetstat;
			FlashfatIoChstat = drv->funcs->IoChstat;
			FlashfatIoRename = drv->funcs->IoRename;
			FlashfatIoChdir = drv->funcs->IoChdir;
			drv->funcs->IoOpen = FlashfatIoOpen_Patched;
			drv->funcs->IoRemove = FlashfatIoRemove_Patched;
			drv->funcs->IoMkdir = FlashfatIoMkdir_Patched;
			drv->funcs->IoRmdir = FlashfatIoRmdir_Patched;
			drv->funcs->IoDopen = FlashfatIoDopen_Patched;
			drv->funcs->IoGetstat = FlashfatIoGetstat_Patched;
			drv->funcs->IoChstat = FlashfatIoChstat_Patched;
			drv->funcs->IoRename = FlashfatIoRename_Patched;
			drv->funcs->IoChdir = FlashfatIoChdir_Patched;		
		}		
	}
	
	return sceIoAddDrv(drv);
}

void PatchModuleMgr()
{
	SceModule2 *mod = sceKernelFindModuleByName("sceModuleManager");

	/* bne t4, zero, +43 -> beq zero, zero, +43 : 
	   Force always to take the size of the data.psp instead of
	   the size of the PBP to avoid the error 0x80020001 */
	_sw(0x1000002A, mod->text_addr+0x3F28);	

	MAKE_JUMP(mod->text_addr+0x4714, sceKernelCreateThreadPatched);
	MAKE_JUMP(mod->text_addr+0x4724, sceKernelStartThreadPatched);
}

void PatchLoadExec()
{
	SceModule2 *mod = sceKernelFindModuleByName("sceLoadExec");

	DoReboot = (void *)(mod->text_addr+0x2138);
	MAKE_CALL(mod->text_addr+0x2090, DoRebootPatched);
	MAKE_CALL(mod->text_addr+0x2344, sceKernelGzipDecompressPatched);
	MAKE_CALL(mod->text_addr+0x232C, sceKernelMemsetPatched);
	_sw(0x3C0188FC, mod->text_addr+0x2384);	
}

void PatchIoFileMgr()
{
	SceModule2 *mod = sceKernelFindModuleByName("sceIOFileManager");

	_sw((u32)sceIoAddDrvPatched, mod->text_addr+0x4414);	
}

void PatchInit()
{
	SceModule2 *mod = sceKernelFindModuleByName("sceInit");
	
	MAKE_JUMP(mod->text_addr+0x1FD8, sceKernelLoadModuleWithApitypePatched);
}

static int EventHandler(int ev_id, char* ev_name, void* param, int* result)
{
	if (ev_id & 0x4000)
	{
		*(u32 *)0x440f0000 = 0xDADADADA;
		*(u32 *)0x440f0004 = 0x33333333;
		
		ClearCaches();
	}
	else if (ev_id == 0x1000B)
	{
		if (last_br == 100)
			sceDisplaySetBrightnessPatched(last_br, last_unk);
	}
	
	return 0;
}

static PspSysEventHandler handler =
{
	sizeof(PspSysEventHandler), 
	"", 
	0x00ffff00, 
	EventHandler,
};

int module_start(SceSize args, void *argp)
{	
	pspSdkInstallNoDeviceCheckPatch();
	pspSdkInstallNoPlainModuleCheckPatch(); 

	PatchModuleMgr();
	PatchLoadExec();
	PatchIoFileMgr();
	PatchInit();

	memcpy(keyseed, 0x883f0000, 0x20); 

	sctrlHENSetStartModuleHandler(OnModuleStart);	
	ClearCaches();

	sceKernelRegisterSysEventHandler(&handler);
	
	return 0;
}

int module_stop(void)
{
	return 0;
}

