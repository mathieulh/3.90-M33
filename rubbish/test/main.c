#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <psputilsforkernel.h>

#include <stdio.h>
#include <string.h>

#define MODULE_NAME "dumpdddd"


PSP_MODULE_INFO(MODULE_NAME, 0x1000, 1, 1);
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

int main_thread(SceSize args, void *argp)
{
	sceKernelDelayThread(20*1000*1000);

	SceUID fd = sceIoOpen("ms0:/kDUMP.bin", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	if (fd >= 0)
	{
		sceIoWrite(fd, (void *)0x88000000, 4*1024*1024);
		sceIoClose(fd);
	}

	return sceKernelExitDeleteThread(0);
}

int module_start(SceSize args, void *argp)
{
	SceUID th;

	/*SceUID fd = sceIoOpen("ms0:/bfc_0.bin", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	if (fd >= 0)
	{
		sceIoWrite(fd, (void *)0xbfc00000, 1024);
		sceIoClose(fd);
	}*/

	th = sceKernelCreateThread("maasiread", main_thread, 0x20, 0x10000, 0, NULL);

	if (th >= 0)
		sceKernelStartThread(th, args, argp);
	
	return 0;
}

int module_stop(void)
{
	return 0;
}

