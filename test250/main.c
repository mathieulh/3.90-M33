#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <psputilsforkernel.h>
#include <systemctrl.h>
#include <umd9660_driver.h>

#include <stdio.h>
#include <string.h>


PSP_MODULE_INFO("pspGalaxyController", 0x1007, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

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

void ClearCaches()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}

/*void WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_TRUNC | PSP_O_CREAT, 0777);

	sceIoWrite(fd, buf, size);
	sceIoClose(fd);
}*/

/*int ReadFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0777);

	int read = sceIoRead(fd, buf, size);
	sceIoClose(fd);

	return read;
}*/

SceUID mount_thread = -1;
SceUID umdfd = -1;
int mounted = 0;
char *umdfile;
u32 text_addr;


int (* WaitMemStick)(void);
int (* LockFdMutex)(void);
int (* UnlockFdMutex)(void);
int SysMemForKernel_C7E57B9C(const u8 *umdid);

static const u8 dummy_umd_id[16] = 
{
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};


/*PspDebugRegBlock DebugRegs;

void _sceDebugExceptionHandler(void);

// Mnemonic register names 
static const unsigned char regName[32][5] =
{
    "zr", "at", "v0", "v1", "a0", "a1", "a2", "a3",
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", 
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"
};

// Taken from the ps2, might not be 100% correct 
static const char *codeTxt[32] =
{
        "Interrupt", "TLB modification", "TLB load/inst fetch", "TLB store",
        "Address load/inst fetch", "Address store", "Bus error (instr)",
        "Bus error (data)", "Syscall", "Breakpoint", "Reserved instruction",
        "Coprocessor unusable", "Arithmetic overflow", "Unknown 13", "Unknown 14",
        "FPU Exception", "Unknown 16", "Unknown 17", "Unknown 18",
        "Unknown 20", "Unknown 21", "Unknown 22", "Unknown 23",
        "Unknown 24", "Unknown 25", "Unknown 26", "Unknown 27",
        "Unknown 28", "Unknown 29", "Unknown 30", "Unknown 31"
};

void DebugTrap()
{
	PspDebugRegBlock *regs = &DebugRegs;
	int i;
	
	Kprintf("Debug trap.\n");

	Kprintf("Exception - %s\n", codeTxt[(regs->cause >> 2) & 31]);
    Kprintf("EPC       - %08X\n", regs->epc);
    Kprintf("Cause     - %08X\n", regs->cause);
    Kprintf("Status    - %08X\n", regs->status);
    Kprintf("BadVAddr  - %08X\n", regs->badvaddr);
    for(i = 0; i < 32; i+=4)
    {
		Kprintf("%s:%08X %s:%08X %s:%08X %s:%08X\n", regName[i], regs->r[i],
		regName[i+1], regs->r[i+1], regName[i+2], regs->r[i+2], regName[i+3], regs->r[i+3]);
    }
	
	WriteFile("ms0:/exception_dump.bin", &DebugRegs, sizeof(DebugRegs));
	WriteFile("ms0:/kdump_crash_351.bin", (void *)0x88000000, 4*1024*1024);

	sceKernelSleepThread();
}

int InstallErrorHandler()
{
	u32 addr;

    addr = (u32) _sceDebugExceptionHandler;
    addr |= 0x80000000;

    return sceKernelRegisterDefaultExceptionHandler((void *) addr);
}*/

int OpenIso()
{
	mounted = 0;
	sceIoClose(umdfd);

	//Kprintf(umdfile);
	
	umdfd = sceIoOpen(umdfile, PSP_O_RDONLY | 0x000f0000, 0777);
	if (umdfd >= 0)
	{
		cso = 0;
		if (CisoOpen(umdfd) >= 0)
			cso = 1;		

		discsize = GetDiscSize();
		lastLBA = -1;
		mounted = 1;
		//Kprintf("Mounted succesfull, size %d\n", discsize);
		return 0;
	}

	//Kprintf("Mounted unsuccesfull.\n");
	return -1;
}


int OpenUmdImage()
{
	umdfile = GetUmdFile();

	WaitMemStick();

	OpenIso();

	_sw(umdfd, text_addr+0x4D1C);
	_sw(0xE0000800, text_addr+0x4D2C);
	_sw(9, text_addr+0x4D34); //
	_sw(discsize, text_addr+0x4D48);
	_sw(discsize, text_addr+0x4D50);

	_sw(0, text_addr+0x4CD8); //

	Kprintf("Discsize %d\n", discsize);

	SysMemForKernel_C7E57B9C(dummy_umd_id);

	

	/*int (* umd_activate)(int u, char *dr);
	umd_activate = (void *)sctrlHENFindFunction("sceNp9660_driver", "sceUmd", 0xC6183D47);

	int res = umd_activate(1, "disc0:");
	WriteFile("ms0:/umd_activate.bin", &res, 4);

	Kprintf("Activate %d\n", res);

	SceUID fd = sceIoOpen("isofs0:/PSP_GAME/SYSDIR/EBOOT.BIN", PSP_O_RDONLY, 0);
	WriteFile("ms0:/isofs_fd.bin", &fd, 4);

	Kprintf("fd = %08X.\n", fd);

	sceIoClose(fd);

	fd = sceIoOpen("disc0:/PSP_GAME/SYSDIR/EBOOT.BIN", PSP_O_RDONLY, 0);
	WriteFile("ms0:/eboot_fd.bin", &fd, 4);

	Kprintf("fd = 0x%08X.\n");*/

	Kprintf("Called = 1.\n");

	called = 1;
	
	ClearCaches();
	return 0;
}

int ReadUmdFile(int offset, void *buf, int outsize)
{
	Kprintf("Read Umd File.\n");
	
	LockFdMutex();

	Kprintf("locked.\n");

	Kprintf("offset 0x%08X\n", offset);
	Kprintf("buf = 0x%08X\n", buf);
	Kprintf("outsize = 0x%08X\n", outsize);

	sceIoLseek(iso_fd, offset, PSP_SEEK_SET);
	int res = sceIoRead(iso_fd, buf, outsize);

	Kprintf("unlockded.\n");

	UnlockFdMutex();

	Kprintf("end rea 0x%08X\n", res);

	return res;
}

int sceIoClosePatched(SceUID fd)
{
	int res = sceIoClose(fd);
	
	if (fd == umdfd)
	{
		umdfd = -1;
		_sw(umdfd, text_addr+0x4D1C);

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

		_sw(0x3c028000, text_addr+0x1808);
		MAKE_CALL(text_addr+0x181C, OpenUmdImage);
		MAKE_CALL(text_addr+0x1D68, ReadUmdFile);
		MAKE_CALL(text_addr+0x2DAC, ReadUmdFile);
		MAKE_JUMP(text_addr+0x4348, sceIoClosePatched);
		WaitMemStick = (void *)(text_addr+0x1220);
		LockFdMutex = (void *)(text_addr+0x2290);
		UnlockFdMutex = (void *)(text_addr+0x22CC);
		
		ClearCaches();
	}

	return sceKernelStartThread(thid, arglen, argp);
}

/*int sceIoDevctlPatched(const char *dev, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
	u32 ra;

	asm("sw $ra, 0(%0)\n" :: "r"(&ra));	
	Kprintf("Devctl 0x%08X, ra=0x%08X\n", cmd, ra);

	int res = sceIoDevctl(dev, cmd, indata, inlen, outdata, outlen);

	Kprintf("devctl res = 0x%08X\n", res);

	return res;
}*/

int module_start(SceSize args, void *argp)
{
	u32 *mod, text_addr;
	
	mod = (u32 *)sceKernelFindModuleByName("sceThreadManager");
	text_addr = *(mod+27);

	_sw((u32)sceKernelCreateThreadPatched, text_addr+0x16BC0);
	_sw((u32)sceKernelStartThreadPatched, text_addr+0x16D54);

	/*mod = (u32 *)sceKernelFindModuleByName("sceIOFileManager");
	text_addr = *(mod+27);
	_sw((u32)sceIoDevctlPatched, text_addr+0x5FE4);*/

	ClearCaches();

	return 0;
}

int module_stop(void)
{
	return 0;
}

