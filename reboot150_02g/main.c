#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsyscon.h>

#include <stdio.h>
#include <string.h>

//#include "reboot.h"


PSP_MODULE_INFO("Reboot150", 0x1000, 1, 0);
PSP_MODULE_SDK_VERSION(0x03060010);

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000
#define SC_OPCODE	0x0000000C
#define JR_RA		0x03e00008

#define NOP	0x00000000

#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a); 
#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a); 

int sceKernelGzipDecompress(u8 *dest, u32 destSize, const u8 *src, void *unk);

int LoadRebootex(u8 *dest, u32 destSize, const u8 *src, void *unk, void *unk2)
{
	/*sceKernelSetDdrMemoryProtection((void *)0x88400000, 4*1024*1024, 0xF);

	
	sceKernelGzipDecompress((void *)0x88700000, 0x10000, rebootex+0x10, 0);

	
	return sceKernelGzipDecompress((void *)0x88400000, 0x01000000, reboot+0x10, 0);	*/
	return 0;
}

int sceKernelMemsetPatched(void *buf, int ch, int size)
{
	return sceKernelMemset((void *)0x88c00000, ch, 0x400000);
}

void ClearCaches()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}

int ReadFile(char *file, int seek, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0);
	if (fd < 0)
		return fd;

	if (seek)
		sceIoLseek(fd, seek, PSP_SEEK_SET);

	int read = sceIoRead(fd, buf, size);
	sceIoClose(fd);

	return read;
}

void WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	sceIoWrite(fd, buf, size);
	sceIoClose(fd);
}

int ReadLine(SceUID fd, char *str)
{
	char ch = 0;
	int n = 0;

	while (1)
	{	
		if (sceIoRead(fd, &ch, 1) != 1)
			break;

		if (ch <= 0x20)
		{
			if (n != 0)
				break;
		}
		else
		{
			*str++ = ch;
			n++;
		}
	}

	return n;
}

char file[64];
char path[64];

typedef struct
{
	char path[64];
	u32  addr;
	u32  size;
} RebootFile;

/*int LoadRebootFiles(u32 a0, u32 a1, u32 a2, u32 a3)
{
	int size;
	RebootFile *reb_files = (RebootFile *)0x89000000;
	u32 buf_addr = 0x89001800;
	
	memset(reb_files, 0, sizeof(RebootFile) * 80);
	
	size = ReadFile("flash0:/km/pspcnf_tbl.txt", (void *)buf_addr, 0x1000000);
	if (size > 0)
	{	
		strcpy(reb_files[0].path, "/kd/pspcnf_tbl.txt");
		reb_files[0].addr = buf_addr;
		reb_files[0].size = size;

		buf_addr += size;
		buf_addr = (buf_addr + 0xF) & ~0xF;
	}

	size = ReadFile("flash0:/km/pspbtcnf_game.txt", (void *)buf_addr, 0x1000000);
	if (size > 0)
	{	
		strcpy(reb_files[1].path, "/kd/pspbtcnf_game.txt");
		reb_files[1].addr = buf_addr;
		reb_files[1].size = size;

		buf_addr += size;
		buf_addr = (buf_addr + 0xF) & ~0xF;
	}

	reb_files += 2;
	
	SceUID fd = sceIoOpen("flash0:/km/pspbtcnf_game.txt", PSP_O_RDONLY, 0);
	if (fd >= 0)
	{
		char *f = file;
		
		while (ReadLine(fd, f) > 0)
		{			
			int read = 0;

			if (f[0] == '%')
				f++;
			
			if (memcmp(f, "/kd/", 4) == 0)
			{
				sprintf(path, "flash0:/km/%s", f+4);
				read = 1;
			}
			else if (memcmp(f, "/vsh/module/", 12) == 0)
			{
				sprintf(path, "flash0:/vsh/podule/%s", f+12);
				read = 1;
			}

			if (read)
			{
				size = ReadFile(path, (void *)buf_addr, 0x1000000);
				if (size > 0)
				{
					strcpy(reb_files->path, f);
					reb_files->addr = buf_addr;
					reb_files->size = size;
					reb_files++;

					buf_addr += size;
					buf_addr = (buf_addr + 0xF) & ~0xF;
				}
			}
			
			memset(file, 0, sizeof(file));
		}
	}

	WriteFile("ms0:/rebootfiles.bin", (void *)0x89000000, sizeof(RebootFile)*80);

	return sceKernelRebootBeforeForKernel(a0, a1, a2, a3);
}*/

void LoadIpl()
{
	int i;
	int size = ReadFile("ms0:/seplugins/ipl.bin", 0, (void *)0x88c00000, 300000);
	
	u32 *src, *dst;
	src = (u32 *)0x88c00000;
	dst = (u32 *)0x440f0000;
	for (i = 0; i < size / 4; i++)
	{
		dst[i] = src[i];
	}
	
	size = ReadFile("ms0:/seplugins/ipl_patch.bin", 0, (void *)0x88c00000, 0x3000);
	src = (u32 *)0x88c00000;
	dst = (u32 *)0x440e0000;
	for (i = 0; i < size / 4; i++)
	{
		dst[i] = src[i];
	}

	*(u32 *)0x440f00E0 = 0x09038000;
	*(u32 *)0x440f00E4 = 0;
	
	*(u32 *)0x440f0000 = 0xDADADADA;
	*(u32 *)0x440f0004 = 0x33333333;

	ClearCaches();
}

int module_start(SceSize args, void *argp)
{	
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceLoadExec");	

	u32 text_addr = *(mod+27);

	MAKE_CALL(text_addr+0x1E3C, LoadRebootex);
	MAKE_CALL(text_addr+0x1E24, sceKernelMemsetPatched);
	_sw(0x3C018870, text_addr+0x1E80);
	//MAKE_CALL(text_addr+0x21D4, LoadRebootFiles);
	
	ClearCaches();

	LoadIpl();
	sceKernelPowerUnlock(0);
	//scePower_driver_0442D852(0);
	sceSysconPowerStandby();
	sceKernelSleepThread();

	/*if (sceSysconGetWlanSwitch())
	{
		sceSysregMsifResetEnable(1);
		sceSysregMsifIoDisable(1);
		sceSysregMsifClkDisable(1);
		sceSysregMsifBusClockDisable(1);
		sceSysregMsifClkSelect(1, 0);
		sceSysregMsifDelaySelect(1, 4);
		sceSysregMsifResetDisable(1);
		sceSysconCtrlWlanPower(0);
	}*/
	
	return 0;
}

int module_stop(void)
{
	return 0;
}

