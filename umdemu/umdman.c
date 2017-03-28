#include <pspsdk.h>
#include <pspkernel.h>

#include "psperror.h"
#include "umdman.h"

int id_iecallback;
u32 gp_iecallback;
void *arg_iecallback;
int (* iecallback)(int id, void *arg, int unk);

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

int sceUmdManRegisterImposeCallBack(int id, void *callback)
{
	Kprintf("Register Impose.\n");
	
	return 0;
}

int sceUmdManUnRegisterImposeCallback(int id)
{
	Kprintf("UnRegister Impose.\n");
	return 0;
}

static void NotifyInsertEjectCallback(int u)
{
	if (iecallback)
	{
		WriteFile("ms0:/iecallback_arg_bef.bin", (void *)arg_iecallback, 0x100);
		asm("lw $gp, 0(%0)\n" :: "r"(&gp_iecallback));
		int res = iecallback(id_iecallback, arg_iecallback, u);	
		WriteFile("ms0:/iecallback_res.bin", &res, 4);
		WriteFile("ms0:/iecallback_arg_aft.bin", (void *)arg_iecallback, 0x100);
	}
}

int (* F1)(void *a0, void *a1, void *a2, void *a3, void *t0);
int (* F2)(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1);

int MyF2(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1)
{
	WriteFile("ms0:/ff2_param0.bin", a0, 0x100);
	
	int res = F2(a0, a1, a2, a3, t0, t1);

	WriteFile("ms0:/ff2_res.bin", &res, 4);
	WriteFile("ms0:/ff2_param4_retres.bin", t1, 4);

	return res;
}

int MyF1(void *a0, void *a1, void *a2, void *a3, void *t0)
{
	WriteFile("ms0:/ff1_param0.bin", a0, 0x100);
		
	int res = F1(a0, a1, a2, a3, t0);

	WriteFile("ms0:/ff1_res.bin", &res, 4);
	WriteFile("ms0:/ff1_param4_retres.bin", t0, 0x100);

	return res;
}

int sceUmdManRegisterInsertEjectUMDCallBack(int id, void *callback, void *arg)
{
	Kprintf("Register Insert Eject.\n");
	
	if (id_iecallback != 0)
		return SCE_ERROR_ERRNO_ENOMEM;
	
	id_iecallback = id;
	asm("sw $gp, 0(%0)\n" :: "r"(&gp_iecallback));
	arg_iecallback = arg;
	iecallback = callback;	

	u32 *mod =  (u32 *)sceKernelFindModuleByName("sceIsofs_driver");
	u32 text_addr = *(mod+27);

	_sw(0x00001021, text_addr+0x4044);
	_sw(0x00001021, text_addr+0x407C);
	_sw(0x00001021, text_addr+0x4130);
	_sw(0x00001021, text_addr+0x430C);

	MAKE_CALL(text_addr+0x1D80, MyF2);
	F2 = (void *)(text_addr+0x1080);

	MAKE_CALL(text_addr+0x3A24, MyF1);
	F1 = (void *)(text_addr+0x19A0);

	_sh(0xDAD0, text_addr+0x38D0);
	_sh(0xDAD1, text_addr+0x39E8);

	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();

	WriteFile("ms0:/isocallback_arg.bin", arg, 0x100);

	NotifyInsertEjectCallback(1);
	
	return 0;
}

int sceUmdManUnRegisterInsertEjectUMDCallBack(int id)
{
	Kprintf("UnRegister Insert Eject.\n");
	
	if (id != id_iecallback)
	{
		return SCE_ERROR_ERRNO_ENOENT;
	}	

	id_iecallback = 0;
	gp_iecallback = 0;
	arg_iecallback = 0;
	iecallback = NULL;	
	
	return 0;
}

int sceUmdMan_driver_60933ECD()
{
	Kprintf("6093.\n");
	
	return 0;
}

int InitUmdMan()
{
	Kprintf("UmdMan Inited.\n");
	
	return 0;
}

