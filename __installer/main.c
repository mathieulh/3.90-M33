#include <pspsdk.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspusb.h>
#include <pspusbstor.h>
#include <pspnand_driver.h>
#include <psploadexec.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "systemctrl.h"
#include "vshctrl.h"
#include "recovery.h"
#include "idcanager.h"
#include "popcorn.h"
#include "galaxy.h"
#include "march33.h"
#include "satelite.h"
#include "usbdevice.h"
#include "pspbtcnf.h"

#define Writefile WriteFile

PSP_MODULE_INFO("Plutonium_Installer", 0x1000, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

#define printf	pspDebugScreenPrintf

char log[1*1024*1024] __attribute__((aligned(64)));
char *log_ptr;
char log_file[256];

u8 buf[3*1024*1024] __attribute__((aligned(64)));

int ReadFile(char *path, void *buf, int size)
{
	SceUID fd = sceIoOpen(path, PSP_O_RDONLY, 0);

	if (fd < 0)
		return fd;

	int read = sceIoRead(fd, buf, size);
	sceIoClose(fd);

	return read;
}

int WriteFile(char *path, void *buf, int size)
{
	SceUID fd = sceIoOpen(path, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (fd < 0)
		return fd;

	int w = sceIoWrite(fd, buf, size);
	sceIoClose(fd);

	return w;
}

void ExitApp()
{
	WriteFile(log_file, log, log_ptr-log);
	
	sceKernelDelayThread(300000);

	sceSysconPowerStandby();

	while (1);
}

void LogStr(char *fmt, ...)
{
	va_list list;
	
	va_start(list, fmt);
	log_ptr += vsprintf(log_ptr, fmt, list);
	va_end(list);	
}

void Unassign()
{
	sceIoUnassign("flash0:");
	sceIoUnassign("flash1:");
}

#define PSP_NAND_PAGE_USER_SIZE			512
#define	PSP_NAND_PAGE_SPARE_SIZE		16
#define PSP_NAND_PAGE_SPARE_SMALL_SIZE	(PSP_NAND_PAGE_SPARE_SIZE-4)
#define PSP_NAND_PAGE_TOTAL_SIZE		(PSP_NAND_PAGE_USER_SIZE+PSP_NAND_PAGE_SPARE_SIZE)
#define	PSP_NAND_PAGES_PER_BLOCK		32

#define PSP_NAND_BLOCK_USER_SIZE		(PSP_NAND_PAGE_USER_SIZE*PSP_NAND_PAGES_PER_BLOCK)
#define PSP_NAND_BLOCK_SPARE_SIZE		(PSP_NAND_PAGE_SPARE_SIZE*PSP_NAND_PAGES_PER_BLOCK)	
#define PSP_NAND_BLOCK_SPARE_SMALL_SIZE	(PSP_NAND_PAGE_SPARE_SMALL_SIZE*PSP_NAND_PAGES_PER_BLOCK)
#define PSP_NAND_BLOCK_TOTAL_SIZE		(PSP_NAND_PAGE_TOTAL_SIZE*PSP_NAND_PAGES_PER_BLOCK)

#define PSP_NAND_TOTAL_BLOCKS			2048
#define PSP_NAND_TOTAL_PAGES			(PSP_NAND_TOTAL_BLOCKS*PSP_NAND_PAGES_PER_BLOCK)
#define PSP_NAND_SIZE_USER				(PSP_NAND_TOTAL_BLOCKS*PSP_NAND_BLOCK_USER_SIZE)
#define PSP_NAND_SIZE_SPARE				(PSP_NAND_TOTAL_BLOCKS*PSP_NAND_BLOCK_SPARE_SIZE)
#define PSP_NAND_SIZE					(PSP_NAND_TOTAL_BLOCKS*PSP_NAND_BLOCK_TOTAL_SIZE)

#define PSP_IPL_MAX_DATA_BLOCKS	0x20
#define PSP_IPL_MAX_SIZE		(PSP_NAND_BLOCK_USER_SIZE*PSP_IPL_MAX_DATA_BLOCKS)
#define PSP_IPL_SIGNATURE		0x6DC64A38

int sceNandEraseIplBlockWithRetry(u32 ppn)
{
	if (ppn >= 0x600)
	{
		LogStr("FATAL ERROR: ppn=0x%08X outside of IPL area.\n");
		ExitApp();
	}

	return sceNandEraseBlockWithRetry(ppn);
}

int sceNandWriteIplBlockWithVerify(u32 ppn, void *user, void *spare)
{
	if (ppn >= 0x600)
	{
		LogStr("FATAL ERROR: ppn=0x%08X outside of IPL area.\n");
		ExitApp();
	}

	return sceNandWriteBlockWithVerify(ppn, user, spare);
}

u8  user[PSP_NAND_BLOCK_USER_SIZE], spare[PSP_NAND_BLOCK_SPARE_SMALL_SIZE];


int pspIplGetIpl(u8 *buf)
{
	u32 block, ppn;
	u16	blocktable[32];
	int i, res, nblocks, size;

	for (block = 4; block < 0x0C; block++)
	{
		ppn = block*PSP_NAND_PAGES_PER_BLOCK;		
		res = sceNandReadPagesRawAll(ppn, user, spare, 1);
		if (res < 0)
		{
			//LogStr("   Error reading page 0x%04X.\n", ppn);
			return res;
		}

		if (spare[5] == 0xFF) // if good block 
		{
			if (*(u32 *)&spare[8] == PSP_IPL_SIGNATURE)
				break;
		}
	}

	if (block == 0x0C)
	{
		//LogStr("   Cannot find IPL in nand!.\n");
		return -1;
	}

	for (nblocks = 0; nblocks < 32; nblocks++)
	{
		blocktable[nblocks] = *(u16 *)&user[nblocks*2];
		
		if (blocktable[nblocks] == 0)
			break;		
	}

	size = 0;

	for (i = 0; i < nblocks; i++)
	{
		ppn = blocktable[i]*PSP_NAND_PAGES_PER_BLOCK;
		res = sceNandReadBlockWithRetry(ppn, buf, NULL);
		if (res < 0)
		{
			//LogStr("   Cannot read block ppn=0x%04.\n", ppn);
			return res;
		}

		buf += PSP_NAND_BLOCK_USER_SIZE;
		
		size += PSP_NAND_BLOCK_USER_SIZE;
	}
	
	return size;
}

int pspIplClearIpl()
{
	u32 block, ppn;
	int res;

	for (block = 0; block < 0x30; block++)
	{
		ppn = block*PSP_NAND_PAGES_PER_BLOCK;

		res = sceNandEraseIplBlockWithRetry(ppn);
		if (res < 0)
		{
			//sceNandDoMarkAsBadBlock(ppn);			
		}
	}

	return 0;
}

int pspIplSetIpl(u8 *buf, u32 size)
{
	int i, res, nblocks, written;
	u32 block, ppn;
	u16 blocktable[32];
	u32 *p;
	
	nblocks = (size + (PSP_NAND_BLOCK_USER_SIZE-1)) / PSP_NAND_BLOCK_USER_SIZE;

	if (nblocks > PSP_IPL_MAX_DATA_BLOCKS)
	{
		//LogStr("   IPL too big (%d).\n");
		return -1;
	}

	// Init spare data
	for (i = 0, p = (u32 *)spare; i < PSP_NAND_PAGES_PER_BLOCK; i++, p += (PSP_NAND_PAGE_SPARE_SMALL_SIZE/4))
	{
		p[0/4] = p[8/4] = 0xFFFFFFFF;
		p[4/4] = PSP_IPL_SIGNATURE;
	}

	// Write data blocks
	block = 0x10;
	for (i = 0; i < nblocks; i++)
	{
		if (i == (nblocks-1))
		{
			int mod = size % PSP_NAND_BLOCK_USER_SIZE;
			
			if (mod != 0)
			{
				memset(user, 0xFF, PSP_NAND_BLOCK_USER_SIZE);
				memcpy(user, buf, mod);

				buf = user;
			}		
		}
			
		while (1)
		{
			ppn = block*PSP_NAND_PAGES_PER_BLOCK;
			res = sceNandWriteIplBlockWithVerify(ppn, buf, spare);
			if (res < 0)
			{
				res = sceNandEraseIplBlockWithRetry(ppn);
				if (res < 0)
				{	
					//sceNandDoMarkAsBadBlock(ppn);					
				}

				block++;
			}
			else
			{
				blocktable[i] = block;
				block++;
				break;
			}
		}

		buf += PSP_NAND_BLOCK_USER_SIZE;
	}

	// Write block table to all mirrors
	memset(user, 0, PSP_NAND_BLOCK_USER_SIZE);
	memcpy(user, blocktable, nblocks*2);
	memset(spare, 0xFF, PSP_NAND_BLOCK_SPARE_SMALL_SIZE);
	*(u32 *)&spare[4] = PSP_IPL_SIGNATURE;

	written = 0;
	
	for (block = 4; block < 0x0C; block++)
	{
		ppn = block*PSP_NAND_PAGES_PER_BLOCK;
		res = sceNandWriteIplBlockWithVerify(ppn, user, spare);
		if (res < 0)
		{
			res = sceNandEraseIplBlockWithRetry(ppn);
			if (res < 0)
			{
				//sceNandDoMarkAsBadBlock(ppn);				
			}
		}
		else
		{
			written = 1;
		}
	}

	if (!written)
	{
		//printf("   Cannot write IPL block table.\n");
		return -1;
	}

	return 0;
}

#include "nandipl.h"

void FlashIpl()
{
	sceNandLock(1);

	LogStr("Flashing custom IPL... ");

	if (pspIplClearIpl() != 0)
	{
		sceNandUnlock();
		LogStr("ClearIpl failed\r\n");
		ExitApp();	
	}

	int res = pspIplSetIpl(buf, IPL_SIZE);
	sceNandUnlock();

	if (res != 0)
	{
		LogStr("SetIpl failed\r\n");
		ExitApp();
	}

	LogStr("OK\r\n");
}

u8 md5_ipl[16] = 
{
	0x28, 0x1A, 0xE2, 0xB0, 0xAF, 0x9F, 0x3F, 0x15, 
	0xF5, 0x33, 0x68, 0x77, 0xC6, 0x62, 0xF0, 0x29
};

void ReadIpl()
{
	u8 md5[16];
	
	sceNandLock(0);

	LogStr("Reading IPL... ");

	int res = pspIplGetIpl(buf+16384);

	sceNandUnlock();
	
	if (res < 0)
	{
		LogStr("Failed\r\n");
		ExitApp();
	}

	LogStr("OK\r\n");

	LogStr("Verifying ipl... ");
	sceKernelUtilsMd5Digest(buf+16384, 221184, md5);

	if (memcmp(md5, md5_ipl, 16) != 0)
	{
		LogStr("Failed -> Not the expected IPL\r\n");
		ExitApp();
	}

	LogStr("OK\r\n");

	memcpy(buf, custom_block, 16384);

	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();

}

void FlashFile(char *file, void *buf, int size)
{
	LogStr("Flashing file %s (%d)... ", file, size);

	int res = WriteFile(file, buf, size);
	if (res != size)
	{
		LogStr("Failed -> 0x%08X\r\n", res);
		ExitApp();
	}

	LogStr("OK\r\n");
}

u8 config[72] = 
{
	0x53, 0x45, 0x43, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void Install()
{
	int retVal;

	strcpy(log_file, "ms0:/M33_360_INSTALL_LOG.TXT");
	
	LogStr("Install initialization... ");

	if (sceNandGetTotalBlocks() != 4096)
	{
		LogStr("Failed -> PSP is not Slim&Lite\r\n");
		ExitApp();
	}
	
	Unassign();

	LogStr("OK\r\n");

	ReadIpl();
	FlashIpl();

	LogStr("Loading updater modules... ");

	retVal = pspSdkLoadStartModule("ms0:/kd/nand_updater.prx", PSP_MEMORY_PARTITION_KERNEL);

	if (retVal < 0)
	{
		LogStr("Failed -> Error %08X loading module nand_updater\r\n", retVal);
		ExitApp();
	}

	retVal = pspSdkLoadStartModule("ms0:/kd/lfatfs_updater.prx", PSP_MEMORY_PARTITION_KERNEL);

	if (retVal < 0)
	{
		LogStr("Failed ->  Error %08X loading module lfatfs_updater\r\n", retVal);
		ExitApp();
	}

	LogStr("OK\r\n");

	sceKernelDelayThread(800000);

	LogStr("Assigning flashes... ");

	retVal = sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDWR, NULL, 0);
	if (retVal < 0)
	{
		LogStr("Failed -> Flash0 assign failed 0x%08X\r\n", retVal);  
		ExitApp();
	}

	retVal = sceIoAssign("flash1:", "lflash0:0,1", "flashfat1:", IOASSIGN_RDWR, NULL, 0);
	if (retVal < 0)
	{
		LogStr("Failed -> Flash1 assign failed 0x%08X\r\n", retVal);  
		ExitApp();
	}

	LogStr("OK\r\n");
	
	FlashFile("flash0:/kd/pspbtjnf_02g.bin", pspbtjnf_02g, sizeof(pspbtjnf_02g));
	FlashFile("flash0:/kd/pspbtknf_02g.bin", pspbtknf_02g, sizeof(pspbtknf_02g));
	FlashFile("flash0:/kd/pspbtlnf_02g.bin", pspbtlnf_02g, sizeof(pspbtlnf_02g));
	FlashFile("flash0:/kd/systemctrl.prx", systemctrl, sizeof(systemctrl));
	FlashFile("flash0:/kd/vshctrl.prx", vshctrl, sizeof(vshctrl));
	FlashFile("flash0:/kd/usbdevice.prx", usbdevice, sizeof(usbdevice));
	FlashFile("flash0:/vsh/module/recovery.prx", recovery, sizeof(recovery));
	FlashFile("flash0:/kd/idcanager.prx", idcanager, sizeof(idcanager));
	FlashFile("flash0:/kd/popcorn.prx", popcorn, sizeof(popcorn));
	FlashFile("flash0:/kd/galaxy.prx", galaxy, sizeof(galaxy));
	FlashFile("flash0:/kd/march33.prx", march33, sizeof(march33));
	FlashFile("flash0:/vsh/module/satelite.prx", satelite, sizeof(satelite));
	FlashFile("flash1:/config.se", config, sizeof(config));

	LogStr("\r\n\r\nInstallation succesfull, welcome to the M33 galaxy.\n");
}

void ReadIplUninstall()
{
	u8 md5[16];
	
	sceNandLock(0);

	LogStr("Reading IPL... ");

	int res = pspIplGetIpl(buf);

	sceNandUnlock();
	
	if (res < 0)
	{
		LogStr("Failed\r\n");
		return;
	}

	LogStr("OK\r\n");

	memmove(buf, buf+16384, 221184);

	LogStr("Verifying ipl... ");
	sceKernelUtilsMd5Digest(buf, 221184, md5);

	if (memcmp(md5, md5_ipl, 16) != 0)
	{
		LogStr("Failed -> Not the expected IPL\r\n");
		return;
	}

	LogStr("OK\r\n");	

	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}

void FlashIplUninstall()
{
	sceNandLock(1);

	LogStr("Flashing back SCE IPL... ");

	if (pspIplClearIpl() != 0)
	{
		sceNandUnlock();
		LogStr("ClearIpl failed\r\n");
		ExitApp();	
	}

	int res = pspIplSetIpl(buf, IPL_SIZE);
	sceNandUnlock();

	if (res != 0)
	{
		LogStr("SetIpl failed\r\n");
		ExitApp();
	}

	LogStr("OK\r\n");
}

void DeleteFile(char *file)
{
	LogStr("Deleting file %s... ");

	int res = sceIoRemove(file);

	if (res < 0)
	{
		LogStr("Failed -> 0x%08X\r\n", res);
		return;
	}
	
	LogStr("OK\r\n");
}

void Uninstall()
{
	int retVal;
	
	strcpy(log_file, "ms0:/M33_360_UNINSTALL_LOG.TXT");

	LogStr("Uninstall initialization... ");

	if (sceNandGetTotalBlocks() != 4096)
	{
		LogStr("Failed -> PSP is not Slim&Lite\r\n");
		ExitApp();
	}
	
	Unassign();

	LogStr("OK\r\n");

	ReadIplUninstall();
	FlashIplUninstall();

	LogStr("\r\n\r\nUninstall succesfull. Congratulations, you have now a closed system.\n");

}

#define UNINSTALL_BUTTONS (PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER | PSP_CTRL_TRIANGLE)

int main()
{
	SceCtrlData pad;
	
	log_ptr = log;

	pspSdkInstallNoDeviceCheckPatch();

	while (1)
	{
		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_CROSS)
		{
			Install();
			break;
		}

		if (pad.Buttons & PSP_CTRL_SQUARE)
		{
			struct SceKernelLoadExecParam param;

			memset(&param, 0, sizeof(param));
			param.size = sizeof(param);
			param.key = "game";

			sceKernelLoadExec("ms0:/kd/backup.elf", &param);
		}

		else if ((pad.Buttons & UNINSTALL_BUTTONS) == UNINSTALL_BUTTONS)
		{
			Uninstall();
			break;
		}

		sceKernelDelayThread(10000);
	}

	ExitApp();	

	return 0;

}

