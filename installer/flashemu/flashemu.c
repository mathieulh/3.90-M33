#include <pspsdk.h>
#include <pspkernel.h>
#include <pspthreadman_kernel.h>

#include <stdio.h>
#include <string.h>

#include "flashemu.h"

char path[260];
SceUID flashemu_sema;

#define Lock()		sceKernelWaitSema(flashemu_sema, 1, NULL)
#define UnLock()	sceKernelSignalSema(flashemu_sema, 1)

void WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	sceIoWrite(fd, buf, size);
	sceIoClose(fd);
}

void BuildPath(const char *file)
{
	strcpy(path, "ms0:/TM/DC5");
	strncat(path, file, 256);	
}

static int FlashEmu_IoInit(PspIoDrvArg* arg)
{
	//Kprintf("IoInit.\n");
	flashemu_sema = sceKernelCreateSema("FlashSema", 0, 1, 1, NULL);
	
	return 0;
}

static int FlashEmu_IoExit(PspIoDrvArg* arg)
{
	//Kprintf("IoExit.\n");

	return 0;
}

void WaitMS()
{
	while (1)
	{
		SceUID fd = sceIoOpen("ms0:/TM/DC5/ipl.bin", PSP_O_RDONLY, 0);
		if (fd >= 0)
		{
			sceIoClose(fd);
			break;
		}

		sceKernelDelayThread(20000);
	}

	
}

static int open_main(int *argv)
{
	PspIoDrvFileArg *arg = (PspIoDrvFileArg *)argv[0];
	const char *file = (const char *)argv[1];
	int flags = argv[2];
	SceMode mode = (SceMode)argv[3];

	Lock();
	BuildPath(file);

	SceUID fd = sceIoOpen(path, flags, mode);
	arg->arg = (void *)fd;

	if (fd < 0)
	{
		//Kprintf("Error 0x%08X in IoOpen, file %s\n", fd, path);
		UnLock();
		return -1;
	}

	
	UnLock();

	return 0;
}

static int FlashEmu_IoOpen(PspIoDrvFileArg *arg, char *file, int flags, SceMode mode)
{
	int argv[4];

	argv[0] = (int)arg;
	argv[1] = (int)file;
	argv[2] = (int)flags;
	argv[3] = (int)mode;

	return sceKernelExtendKernelStack(0x4000, (void *)open_main, (void *)argv);	
}

static int close_main(PspIoDrvFileArg *arg)
{
	int res = 0;
	
	if ((SceUID)arg->arg >= 0)
	{
		res = sceIoClose((SceUID)arg->arg);	
	}
	else
	{
		res = 0;
	}

	arg->arg = (void *)-1;	
	
	if (res < 0)
	{
		//Kprintf("Error 0x%08X in IoClose.\n", res);
		return res;
	}	

	return 0;
}

static int FlashEmu_IoClose(PspIoDrvFileArg *arg)
{
	Lock();
	int res = sceKernelExtendKernelStack(0x4000, (void *)close_main, (void *)arg);
	UnLock();

	return res;
}

static int read_main(int *argv)
{
	PspIoDrvFileArg *arg = (PspIoDrvFileArg *)argv[0];
	char *data = (char *)argv[1];
	int len = argv[2];
	int res;

	Lock();
	if ((SceUID)arg->arg >= 0)
	{
		res = sceIoRead((SceUID)arg->arg, data, len);
	}
	else
	{
		res = -1;
	}
	UnLock();

	return res;
}

static int FlashEmu_IoRead(PspIoDrvFileArg *arg, char *data, int len)
{
	int argv[3];

	argv[0] = (int)arg;
	argv[1] = (int)data;
	argv[2] = (int)len;
	
	return sceKernelExtendKernelStack(0x4000, (void *)read_main, (void *)argv);
}

static int write_main(int *argv)
{
	PspIoDrvFileArg *arg = (PspIoDrvFileArg *)argv[0];
	const char *data = (const char *)argv[1];
	int len = argv[2];
	int res;

	Lock();
	if ((SceUID)arg->arg >= 0)
	{
		res = sceIoWrite((SceUID)arg->arg, data, len);
	}
	else
	{
		res = -1;
	}
	UnLock();

	return res;
}

static int FlashEmu_IoWrite(PspIoDrvFileArg *arg, const char *data, int len)
{
	int argv[3];

	argv[0] = (int)arg;
	argv[1] = (int)data;
	argv[2] = (int)len;
	
	return sceKernelExtendKernelStack(0x4000, (void *)write_main, (void *)argv);	
}

static int lseek_main(int *argv)
{
	PspIoDrvFileArg *arg = (PspIoDrvFileArg *)argv[0];
	u32 ofs = (u32)argv[1];
	int whence = argv[2];

	Lock();
	int res = sceIoLseek((SceUID)arg->arg, ofs, whence);
	UnLock();

	return res;
}

static SceOff FlashEmu_IoLseek(PspIoDrvFileArg *arg, SceOff ofs, int whence)
{
	int argv[3];
	
	argv[0] = (int)arg;
	argv[1] = (int)ofs;
	argv[2] = (int)whence;

	return (SceOff)sceKernelExtendKernelStack(0x4000, (void *)lseek_main, (void *)argv);
}

static int FlashEmu_IoIoctl(PspIoDrvFileArg *arg, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
	int res;
	
	//Kprintf("Ioctl cmd 0x%08X\n", cmd);
	
	Lock();

	switch(cmd)
	{
		case 0x00008003: // FW1.50
		case 0x00208003: // FW2.00 load prx  "/vsh/module/opening_plugin.prx"
					
			res = 0;
		
		break;

		case 0x00208006: // load prx			
			res = 0;
		break;

		case 0x00208007: // after start FW2.50			
			res = 0;
		break;

		case 0x00208081: // FW2.00 load prx "/vsh/module/opening_plugin.prx"
			res = 0;
		break;

		case 0x00208082: // FW2.80 "/vsh/module/opening_plugin.prx"
			res = 0x80010016; // opening_plugin.prx , mpeg_vsh,prx , impose_plugin.prx
		break;

		case 0x00005001: // vsh_module : system.dreg / system.ireg
			// Flush
			//res = sceKernelExtendKernelStack(0x4000, (void *)close_main, (void *)arg);
			res = 0;
		break;

		default:
			//Kprintf("Unknow ioctl 0x%08X\n", cmd);
			res = 0xffffffff;				
	}

	UnLock();
	return res;
}

static int remove_main(int *argv)
{
	const char *name = (const char *)argv[1];

	Lock();
	BuildPath(name);

	int res = sceIoRemove(path);
	
	UnLock();
	return res;
}

static int FlashEmu_IoRemove(PspIoDrvFileArg *arg, const char *name)
{
	int argv[2];

	argv[0] = (int)arg;
	argv[1] = (int)name;
	
	return sceKernelExtendKernelStack(0x4000, (void *)remove_main, (void *)argv);	
}

static int mkdir_main(int *argv)
{
	const char *name = (const char *)argv[1];
	SceMode mode = (SceMode)argv[2];

	Lock();
	BuildPath(name);

	int res = sceIoMkdir(path, mode);

	UnLock();
	return res;
}

static int FlashEmu_IoMkdir(PspIoDrvFileArg *arg, const char *name, SceMode mode)
{
	int argv[3];

	argv[0] = (int)arg;
	argv[1] = (int)name;
	argv[2] = mode;

	return sceKernelExtendKernelStack(0x4000, (void *)mkdir_main, (void *)argv);
}

static int rmdir_main(int *argv)
{
	const char *name = (const char *)argv[1];
	
	Lock();
	BuildPath(name);

	int res = sceIoRmdir(path);

	UnLock();
	return res;
}

static int FlashEmu_IoRmdir(PspIoDrvFileArg *arg, const char *name)
{
	int argv[2];

	argv[0] = (int)arg;
	argv[1] = (int)name;	

	return sceKernelExtendKernelStack(0x4000, (void *)rmdir_main, (void *)argv);
}

static int dopen_main(int *argv)
{
	PspIoDrvFileArg *arg = (PspIoDrvFileArg *)argv[0];
	const char *dirname = (const char *)argv[1];
	
	Lock();
	BuildPath(dirname);

	int dfd = sceIoDopen(path);
	if (dfd < 0)
	{
		//Kprintf("Error 0x%08X in Dopen.\n", dfd);
		UnLock();
		return dfd;
	}

	arg->arg = (void *)dfd;	

	UnLock();
	return 0;
}

static int FlashEmu_IoDopen(PspIoDrvFileArg *arg, const char *dirname)
{
	int argv[2];

	argv[0] = (int)arg;
	argv[1] = (int)dirname;	

	return sceKernelExtendKernelStack(0x4000, (void *)dopen_main, (void *)argv);
}

static int dclose_main(PspIoDrvFileArg *arg)
{
	Lock();
	
	int res = sceIoDclose((SceUID)arg->arg);
	if (res < 0)
	{
		//Kprintf("Error 0x%08X in IoDclose.\n", res);
	}
	else
	{
		res = 0;
	}

	arg->arg = (void *)-1;	

	UnLock();
	return res;
}

static int FlashEmu_IoDclose(PspIoDrvFileArg *arg)
{
	return sceKernelExtendKernelStack(0x4000, (void *)dclose_main, (void *)arg);
}

static int dread_main(int *argv)
{
	PspIoDrvFileArg *arg = (PspIoDrvFileArg *)argv[0];
	SceIoDirent *dir = (SceIoDirent *)argv[1];
	
	Lock();
	int res = sceIoDread((SceUID)arg->arg, dir);
	UnLock();

	return res;
}

static int FlashEmu_IoDread(PspIoDrvFileArg *arg, SceIoDirent *dir)
{
	int argv[2];

	argv[0] = (int)arg;
	argv[1] = (int)dir;
	
	return sceKernelExtendKernelStack(0x4000, (void *)dread_main, (void *)argv);
}

static int getstat_main(int *argv)
{
	const char *file = (const char *)argv[1];
	SceIoStat *stat = (SceIoStat *)argv[2];
	
	Lock();
	BuildPath(file);

	int res = sceIoGetstat(path, stat);

	UnLock();
	return res;
}

static int FlashEmu_IoGetstat(PspIoDrvFileArg *arg, const char *file, SceIoStat *stat)
{
	int argv[3];

	argv[0] = (int)arg;
	argv[1] = (int)file;
	argv[2] = (int)stat;

	return sceKernelExtendKernelStack(0x4000, (void *)getstat_main, (void *)argv);
}

static int chstat_main(int *argv)
{
	const char *file = (const char *)argv[1];
	SceIoStat *stat = (SceIoStat *)argv[2];
	int bits = (int)argv[3];
	
	Lock();
	BuildPath(file);

	int res = sceIoChstat(path, stat, bits);

	UnLock();
	return res;
}

static int FlashEmu_IoChstat(PspIoDrvFileArg *arg, const char *file, SceIoStat *stat, int bits)
{
	int argv[4];

	argv[0] = (int)arg;
	argv[1] = (int)file;
	argv[2] = (int)stat;
	argv[3] = (int)bits;

	return sceKernelExtendKernelStack(0x4000, (void *)chstat_main, (void *)argv);
}

static int rename_main(int *argv)
{
	const char *oldname = (const char *)argv[1];
	const char *newname = (const char *)argv[2];	
	
	Lock();
	BuildPath(oldname);
	
	int res = sceIoRename(oldname, newname);

	UnLock();
	return res;
}

static int FlashEmu_IoRename(PspIoDrvFileArg *arg, const char *oldname, const char *newname)
{
	int argv[3];

	argv[0] = (int)arg;
	argv[1] = (int)oldname;
	argv[2] = (int)newname;

	return sceKernelExtendKernelStack(0x4000, (void *)rename_main, (void *)argv);
}

static int chdir_main(int *argv)
{
	const char *dir = (const char *)argv[1];
	
	Lock();
	BuildPath(dir);

	int res = sceIoChdir(path);

	UnLock();
	return res;
}

static int FlashEmu_IoChdir(PspIoDrvFileArg *arg, const char *dir)
{
	int argv[2];

	argv[0] = (int)arg;
	argv[1] = (int)dir;	

	return sceKernelExtendKernelStack(0x4000, (void *)chdir_main, (void *)argv);
}

static int FlashEmu_IoMount(PspIoDrvFileArg *arg, const char *asgn_name, const char *dev_name, int wr_mode, int unk, int unk2)
{
	//Kprintf("IoMount: %s %s %08X %08X %08X \n", asgn_name, dev_name, wr_mode, unk, unk2);
	return 0;
}

static int FlashEmu_IoUmount(PspIoDrvFileArg *arg)
{
	//Kprintf("IoUmount, fs_num %d\n", arg->fs_num);
	return 0;
}

static int FlashEmu_IoDevctl(PspIoDrvFileArg *arg, const char *devname, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
	int res = 0;

	switch (cmd)
	{
		case 0x5802:
			res = 0;
		
		break;

		default:
			//Kprintf("Unknown devctl command 0x%08X\n", res);
			while (1);
	}

	return res;
}

int FlashEmu_IoUnk21(PspIoDrvFileArg *arg)
{
	// return same as original driver
	return 0x8001b000;
}

static PspIoDrvFuncs flashemu_funcs =
{
	FlashEmu_IoInit,
	FlashEmu_IoExit,
	FlashEmu_IoOpen,
	FlashEmu_IoClose,
	FlashEmu_IoRead,
	FlashEmu_IoWrite,
	FlashEmu_IoLseek,
	FlashEmu_IoIoctl,
	FlashEmu_IoRemove,
	FlashEmu_IoMkdir,
	FlashEmu_IoRmdir,
	FlashEmu_IoDopen,
	FlashEmu_IoDclose,
	FlashEmu_IoDread,
	FlashEmu_IoGetstat,
	FlashEmu_IoChstat,
	FlashEmu_IoRename,
	FlashEmu_IoChdir,
	(void *)FlashEmu_IoMount,
	FlashEmu_IoUmount,
	FlashEmu_IoDevctl,
	FlashEmu_IoUnk21
};

static PspIoDrv flashemu =
{
	"flashfat",
	0x10,
	1,
	"FAT over USB Mass",
	&flashemu_funcs
};

int installed = 0;
PspIoDrv *orig_flashfat = NULL;
PspIoDrvFuncs orig_funcs;

static PspIoDrv *FindDriver(char *drvname)
{
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceIOFileManager");

	if (!mod)
	{
		return NULL;
	}

	u32 text_addr = *(mod+27);

	u32 *(* GetDevice)(char *) = (void *)(text_addr+0x16D4);
	u32 *u;

	u = GetDevice(drvname);

	if (!u)
	{
		return NULL;
	}

	return (PspIoDrv *)u[1];
}

int InstallFlashEmu()
{
	WaitMS();
	
	if (installed)
		return -1;

	orig_flashfat = FindDriver("flashfat");
	
	if (!orig_flashfat)
	{
		sceIoAddDrv(&flashemu);	
		sceIoAssign("flash0:", "flashfat0:", NULL, IOASSIGN_RDONLY, NULL, 0);
		sceIoAssign("flash1:", "flashfat1:", NULL, IOASSIGN_RDWR, NULL, 0);
	}
	else
	{
		memcpy(&orig_funcs, orig_flashfat->funcs, sizeof(PspIoDrvFuncs));
		
		orig_flashfat->funcs->IoInit = FlashEmu_IoInit;
		orig_flashfat->funcs->IoExit = FlashEmu_IoExit;
		orig_flashfat->funcs->IoOpen = FlashEmu_IoOpen;
		orig_flashfat->funcs->IoClose = FlashEmu_IoClose;
		orig_flashfat->funcs->IoRead = FlashEmu_IoRead;
		orig_flashfat->funcs->IoWrite = FlashEmu_IoWrite;
		orig_flashfat->funcs->IoLseek = FlashEmu_IoLseek;
		orig_flashfat->funcs->IoIoctl = FlashEmu_IoIoctl;
		orig_flashfat->funcs->IoRemove = FlashEmu_IoRemove;
		orig_flashfat->funcs->IoMkdir = FlashEmu_IoMkdir;
		orig_flashfat->funcs->IoRmdir = FlashEmu_IoRmdir;
		orig_flashfat->funcs->IoDopen = FlashEmu_IoDopen;
		orig_flashfat->funcs->IoDclose = FlashEmu_IoDclose;
		orig_flashfat->funcs->IoDread = FlashEmu_IoDread;
		orig_flashfat->funcs->IoGetstat = FlashEmu_IoGetstat;
		orig_flashfat->funcs->IoChstat = FlashEmu_IoChstat;
		orig_flashfat->funcs->IoRename = FlashEmu_IoRename;
		orig_flashfat->funcs->IoChdir = FlashEmu_IoChdir;
		orig_flashfat->funcs->IoMount = (void *)FlashEmu_IoMount;
		orig_flashfat->funcs->IoUmount = FlashEmu_IoUmount;
		orig_flashfat->funcs->IoDevctl = FlashEmu_IoDevctl;
		orig_flashfat->funcs->IoUnk21 = FlashEmu_IoUnk21;		
	}

	installed = 1;
	//Kprintf("FlashEmu installed.\n");
	
	return 0;
}

int UninstallFlashEmu()
{
	if (!installed)
		return -1;

	if (!orig_flashfat)
	{
		sceIoUnassign("flash0:");
		sceIoUnassign("flash1:");
		sceIoUnassign("flash2:");
		sceIoUnassign("flash3:");
		sceIoDelDrv("flashfat");
	}
	else
	{
		orig_flashfat->funcs->IoInit = orig_funcs.IoInit;
		orig_flashfat->funcs->IoExit = orig_funcs.IoExit;
		orig_flashfat->funcs->IoOpen = orig_funcs.IoOpen;
		orig_flashfat->funcs->IoClose = orig_funcs.IoClose;
		orig_flashfat->funcs->IoRead = orig_funcs.IoRead;
		orig_flashfat->funcs->IoWrite = orig_funcs.IoWrite;
		orig_flashfat->funcs->IoLseek = orig_funcs.IoLseek;
		orig_flashfat->funcs->IoIoctl = orig_funcs.IoIoctl;
		orig_flashfat->funcs->IoRemove = orig_funcs.IoRemove;
		orig_flashfat->funcs->IoMkdir = orig_funcs.IoMkdir;
		orig_flashfat->funcs->IoRmdir = orig_funcs.IoRmdir;
		orig_flashfat->funcs->IoDopen = orig_funcs.IoDopen;
		orig_flashfat->funcs->IoDclose = orig_funcs.IoDclose;
		orig_flashfat->funcs->IoDread = orig_funcs.IoDread;
		orig_flashfat->funcs->IoGetstat = orig_funcs.IoGetstat;
		orig_flashfat->funcs->IoChstat = orig_funcs.IoChstat;
		orig_flashfat->funcs->IoRename = orig_funcs.IoRename;
		orig_flashfat->funcs->IoChdir = orig_funcs.IoChdir;
		orig_flashfat->funcs->IoMount = orig_funcs.IoMount;
		orig_flashfat->funcs->IoUmount = orig_funcs.IoUmount;
		orig_flashfat->funcs->IoDevctl = orig_funcs.IoDevctl;
		orig_flashfat->funcs->IoUnk21 = orig_funcs.IoUnk21;
	}

	installed = 0;
	//Kprintf("FlashEmu uninstalled.\n");

	return 0;
}



















