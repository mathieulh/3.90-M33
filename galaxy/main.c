#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <psputilsforkernel.h>
#include <pspthreadman_kernel.h>
#include <pspumd.h>
#include <systemctrl.h>
#include <systemctrl_se.h>
#include <umd9660_driver.h>
#include <psperror.h>

#include <stdio.h>
#include <string.h>

#include "csoread.h"


PSP_MODULE_INFO("M33GalaxyController", 0x1006, 1, 1);
PSP_MODULE_SDK_VERSION(0x03060010);

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000
#define SC_OPCODE	0x0000000C
#define JR_RA		0x03e00008

#define NOP	0x00000000

#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a); 
#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a); 

#define MAKE_SYSCALL(a, n) _sw(SC_OPCODE | (n << 6), a);
#define JUMP_TARGET(x) (0x80000000 | ((x & 0x03FFFFFF) << 2))

#define REDIRECT_FUNCTION(a, f) _sw(J_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a);  _sw(NOP, a+4);
#define MAKE_DUMMY_FUNCTION0(a) _sw(0x03e00008, a); _sw(0x00001021, a+4);
#define MAKE_DUMMY_FUNCTION1(a) _sw(0x03e00008, a); _sw(0x24020001, a+4);

/*void WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_TRUNC | PSP_O_CREAT, 0777);

	sceIoWrite(fd, buf, size);
	sceIoClose(fd);
}*/


SceUID mount_thread = -1;
SceUID umdfd = -1;
int mounted = 0, cso = 0;
char *umdfile;
int discsize = 0x7FFFFFFF;
u32 text_addr;
int done;

int (* WaitMemStick)(void);
int (* LockFdMutex)(void);
int (* UnlockFdMutex)(void);
int SysMemForKernel_C7E57B9C(const u8 *umdid);

static const u8 dummy_umd_id[16] = 
{
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};

void ClearCaches()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}

int GetDiscSize()
{
	if (cso == 0)
	{
		return sceIoLseek(umdfd, 0, PSP_SEEK_END) / SECTOR_SIZE;
	}

	return CisofileGetDiscSize(umdfd);
}

int OpenIso()
{
	mounted = 0;
	sceIoClose(umdfd);

	//Kprintf(umdfile);

	while (1)
	{	
		umdfd = sceIoOpen(umdfile, PSP_O_RDONLY | 0x000f0000, 0);
		if (umdfd > 0)
		{
			_sw(umdfd, text_addr+0x5A9C);
		
			cso = 0;
			if (CisoOpen(umdfd) >= 0)
				cso = 1;		

			discsize = GetDiscSize();
			mounted = 1;
			//Kprintf("Mounted succesfull, size %d\n", discsize);		
			return 0;
		}		

		sceKernelDelayThread(10000);
	}	

	//Kprintf("Mounted unsuccesfull.\n");
	return -1;
}

int OpenUmdImage()
{
	umdfile = GetUmdFile();

	//Kprintf("Open umd image.\n");

	WaitMemStick();

	OpenIso();

	int intr = sceKernelCpuSuspendIntr();

	_sw(0xE0000800, text_addr+0x5AAC);
	_sw(9, text_addr+0x5AB4); //
	_sw(discsize, text_addr+0x5AC8);
	_sw(discsize, text_addr+0x5AD0);
	_sw(0, text_addr+0x5A58); //

	sceKernelCpuResumeIntr(intr);

	if (!done)
	{
		done = 1;
		sceKernelDelayThread(800000);
	}

	ClearCaches();

	SysMemForKernel_C7E57B9C(dummy_umd_id);

	return 0;
}

int ReadUmdFileRetry(void *buf, int size, int fpointer)
{
	int i;

	for (i = 0; i < 0x10; i++)
	{
		if (sceIoLseek32(umdfd, fpointer, PSP_SEEK_SET) < 0)
			OpenIso();
		else
			break;
	}

	if (i == 0x10)
		return SCE_ERROR_ERRNO_ENODEV;

	for (i = 0; i < 0x10; i++)
	{
		int read = sceIoRead(umdfd, buf, size);

		if (read < 0)
			OpenIso();
		else
			return read;
	}

	return SCE_ERROR_ERRNO_ENODEV;
}

int g_args[3];

int ReadUmdFile_(int *args)
{
	int offset = args[0];
	void *buf = (void *)args[1];
	int outsize = args[2];
	int res;
	
	if (!cso)
	{
		res = ReadUmdFileRetry(buf, outsize, offset);	
	}
	else
	{
		res = CisoReadFile(buf, outsize, offset);
	}

	return res;
}

int ReadUmdFile(int offset, void *buf, int outsize)
{
	int res;
	
	LockFdMutex();

	g_args[0] = offset;
	g_args[1] = (int)buf;
	g_args[2] = outsize;

	res = sceKernelExtendKernelStack(0x2000, (void *)ReadUmdFile_, g_args);

	UnlockFdMutex();	

	return res;
}

int sceIoClosePatched(SceUID fd)
{
	int res = sceIoClose(fd);
	
	if (fd == umdfd)
	{
		umdfd = -1;
		_sw(umdfd, text_addr+0x5A9C);

		ClearCaches();
	}

	return res;
}

SceUID sceKernelCreateThreadPatched(const char *name, SceKernelThreadEntry entry, int initPriority,
                             int stackSize, SceUInt attr, SceKernelThreadOptParam *option)
{
	SceUID res = sceKernelCreateThread(name, entry, initPriority, stackSize, attr, option);
	
	if (strncmp(name, "SceNpUmdMount", 13) == 0)
	{
		mount_thread = res;
	}

	return res;
}

int sceKernelStartThreadPatched(SceUID thid, SceSize arglen, void *argp)
{
	if (thid == mount_thread)
	{
		u32 *mod;

		mod = (u32 *)sceKernelFindModuleByName("sceNp9660_driver");
		text_addr = *(mod+27);

		_sw(0x3c028000, text_addr+0x1914);
		MAKE_CALL(text_addr+0x1928, OpenUmdImage);
		MAKE_CALL(text_addr+0x1F44, ReadUmdFile);
		MAKE_CALL(text_addr+0x3410, ReadUmdFile);
		MAKE_JUMP(text_addr+0x5158, sceIoClosePatched);
		WaitMemStick = (void *)(text_addr+0x150C);
		LockFdMutex = (void *)(text_addr+0x2AC0);
		UnlockFdMutex = (void *)(text_addr+0x2B18);			

		ClearCaches();
	}

	return sceKernelStartThread(thid, arglen, argp);
}

/*STMOD_HANDLER previous;

int OnModuleStart(SceModule2 *mod)
{
	if (strcmp(mod->modname, "sceMediaSync") == 0)
	{
		int apitype = 0;
		
		switch (sceKernelInitApitype())
		{
			case 0x110:
				apitype = 0x112;
			break;

			case 0x111:
				apitype = 0x113;
			break;

			default:
				apitype = 0x123;
		}

		// Fake apitype for mediasync only
		_sw(0x24020000 | apitype, mod->text_addr+0x10);
		_sw(0x24020000 | apitype, mod->text_addr+0x7B8);
		
		// Dummy function
		_sw(0x24020000, mod->text_addr+0x7E8);		
	}
	
	return previous(mod);
}*/

int module_start(SceSize args, void *argp)
{
	SceModule2 *mod;
	
	mod = sceKernelFindModuleByName("sceThreadManager");	
	_sw((u32)sceKernelCreateThreadPatched, mod->text_addr+0x16E40);
	_sw((u32)sceKernelStartThreadPatched, mod->text_addr+0x16FD4);

	/* Make umd image not NULL
	mod = sceKernelFindModuleByName("sceInit");
	_sw(1, mod->text_addr+0x22D8);
	
	previous = sctrlHENSetStartModuleHandler(OnModuleStart);*/

	ClearCaches();

	while (1)
	{	
		SceUID fd = sceIoOpen(GetUmdFile(), PSP_O_RDONLY, 0);
		if (fd >= 0)
		{
			sceIoClose(fd);
			break;
		}

		sceKernelDelayThread(10000);
	}

	return 0;
}



