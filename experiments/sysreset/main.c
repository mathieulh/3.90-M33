#include <pspsdk.h>
#include <pspkernel.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

PSP_MODULE_INFO("MyTest", 0x1000, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

#define printf	pspDebugScreenPrintf

void ErrorExit(int milisecs, char *fmt, ...)
{
	va_list list;
	char msg[256];	

	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);

	printf(msg);

	sceKernelDelayThread(milisecs*1000);
	sceKernelExitGame();
}

int WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (fd < 0)
		return fd;

	int written = sceIoWrite(fd, buf, size);
	sceIoClose(fd);

	return written;
}

#define NAND_STATUS (*((volatile unsigned *)0xBD101004))
#define NAND_COMMAND (*((volatile unsigned *)0xBD101008))
#define NAND_ADDRESS (*((volatile unsigned *)0xBD10100C))
#define NAND_READDATA (*((volatile unsigned *)0xBD101300))
#define NAND_ENDTRANS (*((volatile unsigned *)0xBD101014))

u32 commands[20] =
{
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 
	0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01,
	0x00, 0x00, 0x01, 0xFF 
};

void SetActiveNand(u32 unit)
{
	// unit 0 -> UP, unit 1 -> internal nand
	int i;
	
	commands[19] = unit;

	for (i = 0; i < 20; i++)
	{
		NAND_COMMAND = commands[i];
		NAND_ADDRESS = 0;
		NAND_ENDTRANS = 1;
	}
}

int (* func)(u8 *a0);
u8 structure[0x60];
u32 text_addr;

void DoFunc()
{
	u32 *mod = sceKernelFindModuleByName("sceSYSCON_Driver");
	text_addr = *(mod+27);
	
	func = (void *)text_addr+0x17DC;
}

void ChangeNand()
{
	sceNandLock(0);
	SetActiveNand(1);
	sceNandUnlock();
}

void DoStructure()
{
	structure[0xC] = 0x32;
	structure[0xD] = 3;
	structure[0xE] =  1;
	structure[0xF] =  0;
	structure[0x10] = 0;
	structure[0x11] = 0;

    structure[0xF] = 0xC9;

	/*structure[0x11] = 0xFF;
	structure[0x12] = 0xFF;
	structure[0x13] = 0xFF;
	structure[0x14] = 0xFF;
	structure[0x15] = 0xFF;
	structure[0x16] = 0xFF;
	structure[0x17] = 0xFF;
	structure[0x18] = 0xFF;
	structure[0x19] = 0xFF;
	structure[0x1A] = 0xFF;
	structure[0x1B] = 0xFF;
	structure[0x1C] = 0xFF;

	printf("Doing reboot...\n");

	for (i = 0; i < 0x10; i++)
		structure[0x1C+i] = 0xFF;*/
	memset(structure+0x11, 0xFF, 0x10+12);
}

int SuperFunc(u8 *structure)
{
	int v1, t1, a0, t0;

	Kprintf("point 1: %08X\n", *(u32 *)0xbfc00058);

	
	sceGpioPortRead();
	sceGpioPortClear(8);

	while (_lw(0xbe58000c) & 4)
	{
		v1 = _lw(0xbe580008);
	}

	int v0 = _lw(0xbe58000C);
	_sw(3, 0xbe580020);

	Kprintf("point 2.\n");

	
	int a3 = structure[0xD]+1;

	int a2 = 0;

	do
	{

		t1 = structure[0xC] << 8; 
		a0 = t1 | structure[0xD]; 
		a2 = a2 + 2;
		t0 = a2 < a3;

		v1 = _lw(0xbe58000C);
		structure += 2;

		_sw(a0, 0xbe580008);
	} while (t0);
	
	Kprintf("%08X\n", *(u32 *)0xbfc00058);

	_sw(6, 0xbe580004);
	Kprintf("%08X\n", *(u32 *)0xbfc00058);
	sceGpioPortSet(8);

	Kprintf("%08X\n", *(u32 *)0xbfc00058);

	return 0;

}

void me_run(void);
void me_end(void);

void me_main()
{
	//*(u32 *)0xbfc00058 = 0xDADADADA;

	while (1)
	{
		*(u32 *)0xbfc00058 = 0xDADADADA;
	}
}

void AsmTest()
{
	int res_v0, res_v1;
	
	asm("li $v0, 0x00012345\n"
		"li $v1, 0x00000777\n"
		"li $a0, 0x00009612\n"
		"li $a1, 0x00006891\n"
		"li $t0, 0x00002698\n"
		"li $t1, 0x00007132\n"
		"addu	$v0, $t0, $a0\n"
		"nop\n"
		"nop\n"
		"sw $v0, 0(%0)\n" :: "r"(&res_v0) 
		);

	printf("v0 = %08X\n", res_v0); // v1 = 0x777 v0 = 0x2698

}

int main()
{
	u32 *st32 = (u32 *)structure;
	int i; 
	
	pspDebugScreenInit();
	pspDebugScreenClear();

	pspDebugSioInit();
	pspDebugSioSetBaud(115200);
	pspDebugSioInstallKprintf();
	Kprintf("Kprintf installed.\n");

	printf("huola\n");

	memcpy((void *)0xbfc00040, me_run, (int)(me_end - me_run));
	sceKernelDcacheWritebackInvalidateAll();

	sceSysregMeResetEnable();
	sceSysregMeBusClockEnable();
	sceSysregMeResetDisable();
	//sceSysregVmeResetDisable(); 

	printf("me done.\n");
	AsmTest();
	

	DoFunc();

	printf("func = %08X\n", func);
	
	printf("Rebooting psp...\n");
	ChangeNand();

	sceKernelDelayThread(2*1000*1000);

	//sceSysconResetDevice(1, 1);

	DoStructure();

	AsmTest();

	while (1)
	{
		volatile u32 *count = (u32*) 0x09A00000;

		pspDebugScreenSetXY(0, 0);
		//pspDebugScreenPrintf("ME Basic Example, press Home to exit\n");
		sceKernelDcacheWritebackInvalidateAll();
		pspDebugScreenPrintf("ME Counter: %08x\n", *count);
		sceKernelDelayThread(20000);
	}


	/*int s5= sceKernelCpuSuspendIntr();

	st32[0] = 0;
	st32[1] = 0x10000;
	st32[2] = 0xFFFFFFFF;
	st32[0xB] = 0;*/

	//printf("syscon cmd exec... %08X\n", sceSysconCmdExecAsync(structure, 0, 0, 0));
	printf("func = %08X\n", SuperFunc(structure));

	ErrorExit(10000, "Done. Rebooting with ipl...\n");

		
	return 0;
}

