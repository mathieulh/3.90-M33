#include <pspsdk.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspnand_driver.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <psputility.h>

#include "reboot303.h"
#include "systemctrl150_310.h"
#include "reboot310.h"
#include "reboot150.h"
#include "recovery.h"
#include "systemctrl_310.h"
#include "vshctrl_303.h"



PSP_MODULE_INFO("303_10update", 0x1000, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

#define printf pspDebugScreenPrintf

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

int ReadFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0777);

	if (fd < 0)
		return -1;

	int read = sceIoRead(fd, buf, size);

	if (sceIoClose(fd) < 0)
		return -1;

	return read;
}


int WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (fd < 0)
		return -1;

	int written = sceIoWrite(fd, buf, size);

	if (sceIoClose(fd) < 0)
		return -1;

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

u32 commands_2[20] =
{
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01,
	0x01, 0x01, 0x00, 0xFF
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

u8 unk()
{
	int i;
	u8 read; // read?

	commands_2[19] = 1; 

	for (i = 0; i < 20; i++)
	{
		NAND_COMMAND = commands_2[i];
	}

	read = (u8)NAND_READDATA;

	commands_2[19] = 0; 

	for (i = 0; i < 20; i++)
	{
		NAND_COMMAND = commands_2[i];
	}

	return read;
}

void SetActiveFlash(int unit)
{
	sceIoUnassign("flash0:");
	sceIoUnassign("flash1:");

	sceNandLock(0);

	SetActiveNand(unit);
	unk();

	sceNandUnlock();

	sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", 0, IOASSIGN_RDWR , 0);
	sceIoAssign("flash1:", "lflash0:0,1", "flashfat1:", 0, IOASSIGN_RDWR , 0);
}

u8 buf[100000];

u8 sha1_303[20] = 
{
	0x81, 0x32, 0x02, 0xD7, 0xF8, 0xBA, 0xA9, 0xB6, 
	0xD1, 0xD3, 0x38, 0xC2, 0x8F, 0x7D, 0x16, 0x2E, 
	0xF7, 0x10, 0x2B, 0xBB
};

u8 sha1_310[20] = 
{
	0x33, 0x2F, 0x53, 0xE0, 0xD0, 0x52, 0xFE, 0x6A, 
	0x13, 0x4F, 0x03, 0x8B, 0xE3, 0x71, 0xB1, 0xF2, 
	0x8F, 0x20, 0x6F, 0x84
};

u8 sha1[20];

int main()
{
	int size, i;
	
	pspDebugScreenInit();

	size = ReadFile("flash0:/vsh/etc/index.dat", buf, 512);

	sceKernelUtilsSha1Digest(buf, size, sha1);

	if (memcmp(sha1, sha1_303, 20) != 0)
	{
		ErrorExit(6000, "3.03 OE-C is not installed in UP nand.\n");
	}

	size = ReadFile("flash0:/kd/rtc.prx", buf, sizeof(buf));
	if (size <= 0)
	{
		ErrorExit(6000, "Error reading rtc.prx (up).\n");
	}

	for (i = 0; i < size-0x10; i++)
	{
		if (memcmp(buf+i, "reboot.bin", 10) == 0)
		{
			memcpy(reboot303+0x47C, buf+i+0x10, 0xBE6E);
			memcpy(systemctrl150_310+0xA4C, buf+i+0x10, 0xBE6E);
			break;
		}
	}	

	if (i == (size-0x10))
	{
		ErrorExit(6000, "Cannot find reboot.bin. Not 3.03 OE-C.\n");
	}

	SetActiveFlash(1);

	size = ReadFile("flash0:/vsh/etc/index.dat", buf, 512);
	sceKernelUtilsSha1Digest(buf, size, sha1);

	if (memcmp(sha1, sha1_310, 20) != 0)
	{
		ErrorExit(6000, "3.10 OE-A (or A') is not installed in PSP nand.\n");
	}

	size = ReadFile("flash0:/kd/rtc.prx", buf, sizeof(buf));
	if (size <= 0)
	{
		ErrorExit(6000, "Error reading rtc.prx (psp).\n");
	}

	for (i = 0; i < size-0x10; i++)
	{
		if (memcmp(buf+i, "reboot.bin", 10) == 0)
		{
			memcpy(reboot310+0x60C, buf+i+0x10, 0xAD88);
			break;
		}
	}	

	if (i == (size-0x10))
	{
		ErrorExit(6000, "Cannot find reboot.bin. Not 3.10 OE-A (or A').\n");
	}

	size = ReadFile("flash0:/kn/reboot150.prx", buf, sizeof(buf));
	if (size <= 0)
	{
		ErrorExit(6000, "Error reading reboot150.prx (psp).\n");
	}

	for (i = 0; i < size-0x10; i++)
	{
		if (memcmp(buf+i, "reboot.bin", 10) == 0)
		{
			memcpy(reboot150+0x3c8, buf+i+0x10, 0xC5FB);
			break;
		}
	}	

	if (i == (size-0x10))
	{
		ErrorExit(6000, "Cannot find reboot.bin 1.50. Not 3.10 OE-A (or A').\n");
	}

	printf("Writing reboot303.prx (PSP nand)... %08X\n", WriteFile("flash0:/kn/reboot303.prx", reboot303, sizeof(reboot303)));
	printf("Writing rtc.prx (1.50 core) (PSP nand)... %08X\n", WriteFile("flash0:/kd/rtc.prx", systemctrl150_310, sizeof(systemctrl150_310)));
	printf("Writing systemctrl.prx (PSP nand)... %08X\n", WriteFile("flash0:/kn/systemctrl.prx", systemctrl_310, sizeof(systemctrl_310)));
	
	SetActiveFlash(0);
	sceKernelDelayThread(100000);

	printf("Writing reboot310.prx (UP nand)... %08X\n", WriteFile("flash0:/kn/reboot310.prx", reboot310, sizeof(reboot310)));
	printf("Writing reboot150.prx (UP nand)... %08X\n", WriteFile("flash0:/kn/reboot150.prx", reboot150, sizeof(reboot150)));
	printf("Writing vshctrl.prx (UP nand)... %08X\n", WriteFile("flash0:/kn/vshctrl.prx", vshctrl_303, sizeof(vshctrl_303)));
	printf("Writing recovery.prx (UP nand)... %08X\n", WriteFile("flash0:/kd/recovery.prx", recovery, sizeof(recovery)));

	printf("\nDone. Press X to exit to XMB.\n");

	while (1)
	{
		SceCtrlData pad;

		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_CROSS)
			break;

		sceKernelDelayThread(10000);
	}
	
	sceKernelExitGame();
	
	return 0;
}
