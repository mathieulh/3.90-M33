#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <psputilsforkernel.h>
#include <pspthreadman_kernel.h>

#include <stdio.h>
#include <string.h>


PSP_MODULE_INFO("M33GalaxyController", 0x1007, 1, 1);
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




int MyFunc(void *a0, void *a1)
{
	SceUID fd = sceIoOpen("ms0:/newdrm.bin", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	if (fd < 0)
	{
		return fd;
	}

	sceIoWrite(fd, a1, 0x1364);
	sceIoClose(fd);

	return -1;
}

int module_start(void)
{
	
	REDIRECT_FUNCTION(sctrlHENFindFunction("scePspNpDrm_Driver", "scePspNpDrm_driver", 0x0F9547E6), MyFunc);
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
	
	return 0;
}

