#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <psputilsforkernel.h>
#include <systemctrl.h>

#include <stdio.h>
#include <string.h>

PSP_MODULE_INFO("WifiTest", 0x1007, 1, 1);
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

int MyTestA(u32 unk)
{
	Kprintf("MyTestA %08X\n", unk);

	u32 *mod = sceKernelFindModuleByName("sceWlan_Driver");
	u32 text_addr = *(mod+27);
	int (* MyT)(u32);

	MyT = (void *)(text_addr+(0x88248010-0x8823f300));

	int res = MyT(unk);

	Kprintf("result = %08X\n", res);

	return res;

}

APRS_EVENT previous = NULL;

int OnPspRelSectionEvent(char *modname, u8 *modbuf)
{
	//Kprintf("ms video plugin loaded.\n");

	if (strcmp(modname, "music_main_plugin_module") == 0)
	{
		Kprintf("ms video plugin loaded.\n");

		u32 *mod = sceKernelFindModuleByName("sceWlan_Driver");
		Kprintf("mod = %08X\n", mod);

		u32 text_addr = *(mod+27);
		Kprintf("text_addr = %08X\n", text_addr);

		_sw(0, text_addr+0x882415e8-0x8823f300);

		/*MAKE_CALL(text_addr+0x88241188-0x8823f300, MyTestA);
		MAKE_CALL(text_addr+0x88241390-0x8823f300, MyTestA);
		MAKE_CALL(text_addr+0x88243530-0x8823f300, MyTestA);
		MAKE_CALL(text_addr+0x88243b58-0x8823f300, MyTestA);
		MAKE_CALL(text_addr+0x88247f8c-0x8823f300, MyTestA);*/

		int (* power_function)(int, int, int) = (void *)sctrlHENFindFunction("scePower_Service", "scePower_driver", 0x737486F2);

		power_function(333, 333, 166);
		
		ClearCaches();
	}

	if (!previous)
		return 0;

	return previous(modname, modbuf);
}

int module_start(SceSize args, void *argp)
{
	Kprintf("Entering wifi test.\n");
	previous = sctrlHENSetOnApplyPspRelSectionEvent(OnPspRelSectionEvent);

	return 0;
}

int module_stop(void)
{
	return 0;
}

