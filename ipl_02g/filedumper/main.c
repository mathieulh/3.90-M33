#include <pspsdk.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspusb.h>
#include <pspusbstor.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

PSP_MODULE_INFO("FlashUSB", 0x1000, 1, 0);
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

/* 1.50 specific function */
PspIoDrv *FindDriver(char *drvname)
{
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceIOFileManager");

	if (!mod)
	{
		return NULL;
	}

	u32 text_addr = *(mod+27);

	u32 *(* GetDevice)(char *) = (void *)(text_addr+0x16D4);
	u32 *u;

	u = GetDevice(drvname);

	if (!u)
	{
		return NULL;
	}

	return (PspIoDrv *)u[1];
}

PspIoDrv *lflash_driver;
PspIoDrv *msstor_driver;

int (* Old_IoIoctl)(PspIoDrvFileArg *arg, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen);

//PspIoDrvFuncs funcs;

static int New_IoOpen(PspIoDrvFileArg *arg, char *file, int flags, SceMode mode)
{
	//arg->fs_num = 1; // for flash 1

	if (!lflash_driver->funcs->IoOpen)
		return -1;

	return lflash_driver->funcs->IoOpen(arg, file, flags, mode);
}

static int New_IoClose(PspIoDrvFileArg *arg)
{
	//arg->fs_num = 1; // for flash 1

	if (!lflash_driver->funcs->IoClose)
		return -1;

	return lflash_driver->funcs->IoClose(arg);
}

static int New_IoRead(PspIoDrvFileArg *arg, char *data, int len)
{
	//arg->fs_num = 1; // for flash 1

	if (!lflash_driver->funcs->IoRead)
		return -1;

	return lflash_driver->funcs->IoRead(arg, data, len);
}
static int New_IoWrite(PspIoDrvFileArg *arg, const char *data, int len)
{
	//arg->fs_num = 1; // for flash 1

	if (!lflash_driver->funcs->IoWrite)
		return -1;

	return lflash_driver->funcs->IoWrite(arg, data, len);
}

static SceOff New_IoLseek(PspIoDrvFileArg *arg, SceOff ofs, int whence)
{
	//arg->fs_num = 1; // for flash 1

	if (!lflash_driver->funcs->IoLseek)
		return -1;

	return lflash_driver->funcs->IoLseek(arg, ofs, whence);
}

u8 data_5803[96] = 
{
	0x02, 0x00, 0x08, 0x00, 0x08, 0x00, 0x07, 0x9F, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x21, 0x21, 0x00, 0x00, 0x20, 0x01, 0x08, 0x00, 0x02, 0x00, 0x02, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static int New_IoIoctl(PspIoDrvFileArg *arg, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
	//arg->fs_num = 1; // for flash 1

	Kprintf("IoIoctl: 0x%08X  %d  %d\n", cmd, inlen, outlen);

	if (cmd == 0x02125008)
	{
		u32 *x = (u32 *)outdata;
		*x = 1; /* Enable writing */
		return 0;
	}
	else if (cmd == 0x02125803)
	{
		memcpy(outdata, data_5803, 96);
		return 0;
	}

	return -1;
}

static int New_IoDevctl(PspIoDrvFileArg *arg, const char *devname, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
	//arg->fs_num = 1;

	Kprintf("IoDevctl: 0x%08X  %d  %d\n", cmd, inlen, outlen);

	if (cmd == 0x02125801)
	{
		u8 *data8 = (u8 *)outdata;

		data8[0] = 1;
		data8[1] = 0;
		data8[2] = 0,
		data8[3] = 1;
		data8[4] = 0;
				
		return 0;
	}

	return -1;
}

void Unassign()
{
	sceIoUnassign("flash0:");
	sceIoUnassign("flash1:");
}

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

u8 buf[2*1024*1024];

char input[256], output[256], temp[256];

char *tolower(char *s)
{
	int i;

	for (i = 0; i < strlen(s); i++)
	{
		if (s[i] >= 'A' && s[i] <= 'Z')
			s[i] += 0x20;
	}

	return s;
}

int Decrypt(u32 *buf, int size)
{
	buf[0] = 5;
	buf[1] = buf[2] = 0;
	buf[3] = 0x100;
	buf[4] = size;

	int res = semaphore_4C537C72(buf, size+0x14, buf, size+0x14, 8);
	if (res < 0)
	{
		return -1;
	}

	return 0;
}

u8 check_keys0[0x10] =
{
	0x71, 0xF6, 0xA8, 0x31, 0x1E, 0xE0, 0xFF, 0x1E,
	0x50, 0xBA, 0x6C, 0xD2, 0x98, 0x2D, 0xD6, 0x2D
}; 

u8 check_keys1[0x10] =
{
	0xAA, 0x85, 0x4D, 0xB0, 0xFF, 0xCA, 0x47, 0xEB,
	0x38, 0x7F, 0xD7, 0xE4, 0x3D, 0x62, 0xB0, 0x10
};

int UnsignCheck(u8 *buf)
{
	u8 enc[0xD0+0x14];
	int iXOR, res;

	memcpy(enc+0x14, buf+0x80, 0xD0);

	for (iXOR = 0; iXOR < 0xD0; iXOR++)
	{
		enc[iXOR+0x14] ^= check_keys1[iXOR&0xF]; 
	}

	if ((res = Decrypt((u32 *)enc, 0xD0)) < 0)
	{
		printf("Decrypt failed.\n");
		return res;
	}

	for (iXOR = 0; iXOR < 0xD0; iXOR++)
	{
		enc[iXOR] ^= check_keys0[iXOR&0xF];
	}

	memcpy(buf+0x80, enc+0x40, 0x90);
	memcpy(buf+0x110, enc, 0x40);

	return 0;
}


void CopyFile(char *input, char *output)
{
	int size = ReadFile(input, buf, sizeof(buf));

	if (memcmp(buf, "~PSP", 4) == 0)
		UnsignCheck(buf);

	if (size > 0)
		WriteFile(tolower(output), buf, size);
}

char filelist[1024][256];

int ListFiles(char *path, u32 i)
{
	SceUID dfd;
	SceIoDirent dir;	

	if ((dfd = sceIoDopen(path)) > 0)
	{
		while (sceIoDread(dfd, &dir) > 0)
		{
			if(dir.d_stat.st_attr & FIO_SO_IFDIR)
			{
				if (dir.d_name[0] != '.')
				{
					sprintf(filelist[i++], "%s%s/", path, dir.d_name);				
					i = ListFiles(filelist[i-1], i);
				}
			}

			else
			{
				strcpy(filelist[i], path);
				strcat(filelist[i++], dir.d_name);
			}
		}
	}

	else
	{
		printf("Cannot open dir");
	}

	printf("returninf %d.\n", i);

	sceIoDclose(dfd);
	return i;
}

void CopyDir(char *indir, char *outdir)
{
	int n = ListFiles("flash0:/", 1);
	int i;
	
	for (i = 0; i < n; i++)
	{
		printf("%s\n", filelist[i]);
		
		if (filelist[i][strlen(filelist[i])-1] == '/')
		{
			memcpy(filelist[i], "ms0:/F0", 7);
			filelist[i][strlen(filelist[i])-1] = 0;
			sceIoMkdir(filelist[i], 0777);
		}

		else
		{
			strcpy(output, filelist[i]);			
			memcpy(output, "ms0:/F0", 7);
			CopyFile(filelist[i], output);
		}
	}
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
		ErrorExit(5000, "FATAL ERROR: ppn=0x%08X outside of IPL area.\n");
	}

	return sceNandEraseBlockWithRetry(ppn);
}

int sceNandWriteIplBlockWithVerify(u32 ppn, void *user, void *spare)
{
	if (ppn >= 0x600)
	{
		ErrorExit(5000, "FATAL ERROR: ppn=0x%08X outside of IPL area.\n");
	}

	return sceNandWriteBlockWithVerify(ppn, user, spare);
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
	u8  user[PSP_NAND_BLOCK_USER_SIZE], spare[PSP_NAND_BLOCK_SPARE_SMALL_SIZE];
	u32 *p;
	
	nblocks = (size + (PSP_NAND_BLOCK_USER_SIZE-1)) / PSP_NAND_BLOCK_USER_SIZE;

	if (nblocks > PSP_IPL_MAX_DATA_BLOCKS)
	{
		printf("IPL too big (%d). Max size is %d.\n", size, PSP_IPL_MAX_SIZE);
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
		printf("IPL block table could not be written to any mirror.\n");
		return -1;
	}

	return 0;
}

#include "nandipl.h"

void FlashIpl()
{
	int size = ReadFile("ms0:/ipl_result.bin", buf, size);

	if (size <= 0)
	{
		WriteFile("ms0:/ipl_error.bin", &size, 4);
		return;
	}
	
	sceNandLock(1);

	if (pspIplClearIpl() != 0)
	{
		sceNandUnlock();
		ErrorExit(6000, "Error in clear ipl.\n");
	}

	int res = pspIplSetIpl(buf, size);
	sceNandUnlock();

	if (res != 0)
	{
		ErrorExit(6000, "Error writing ipl.\n");
	}
}

int main()
{
	int retVal;
	
	pspDebugScreenInit();
	pspDebugScreenClear();

	pspSdkInstallNoDeviceCheckPatch();
	pspSdkInstallNoPlainModuleCheckPatch();

	/*FlashIpl();

	sceSysconPowerStandby();*/

	Unassign();

	retVal = pspSdkLoadStartModule("ms0:/kd/nand_updater.prx", PSP_MEMORY_PARTITION_KERNEL);

	if (retVal < 0)
	{
		WriteFile("ms0:/error1.bin", "e1", 2);
		ErrorExit(7000, "Error %08X loading module nand_updater.\n", retVal);
	}

	retVal = pspSdkLoadStartModule("ms0:/kd/lfatfs_updater.prx", PSP_MEMORY_PARTITION_KERNEL);

	if (retVal < 0)
	{
		WriteFile("ms0:/error2.bin", "e2", 2);
		ErrorExit(7000, "Error %08X loading module lfatfs_updater.\n", retVal);
	}

	sceKernelDelayThread(800000);

	retVal = sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDWR, NULL, 0);
	
	printf("assign = 0x%08X\n", retVal);
	
	/*WriteFile("ms0:/flash0asign.bin", &retVal, 4);	
	CopyDir("flash0:/", "ms0:/F0/");
		sceSysconPowerStandby();*/
	int size;

	/*size = ReadFile("ms0:/pspbtjnf_02g.bin", buf, sizeof(buf));

	printf("size = %d.\n", size);

	size = WriteFile("flash0:/kd/pspbtjnf_02g.bin", buf, size);
	printf("Write = 0x%08X\n", size);*/
	
	size = ReadFile("ms0:/systemctrl_02g.prx", buf, sizeof(buf));

	printf("size = %d.\n", size);

	size = WriteFile("flash0:/kd/systemctrl_02g.prx", buf, size);
	printf("Write = 0x%08X\n", size);

	/*size = ReadFile("ms0:/usbdevice.prx", buf, sizeof(buf));

	printf("size = %d.\n", size);

	size = WriteFile("flash0:/kd/usbdevice.prx", buf, size);
	printf("Write = 0x%08X\n", size);

	size = ReadFile("ms0:/recovery.prx", buf, sizeof(buf));

	printf("size = %d.\n", size);

	size = WriteFile("flash0:/vsh/module/recovery.prx", buf, size);
	printf("Write = 0x%08X\n", size);*/



	/*size = ReadFile("ms0:/pspbtcnf_02g_oe.bin", buf, sizeof(buf));
	
	size = WriteFile("flash0:/kd/pspbtjnf_02g.bin", buf, size);
	WriteFile("ms0:/btcnf.bin", &size, 4);

	size = ReadFile("ms0:/usbdevice.prx", buf, sizeof(buf));

	size = WriteFile("flash0:/kd/usbdevice.prx", buf, size);
	WriteFile("ms0:/usbdev.bin", &size, 4);

	size = ReadFile("ms0:/recovery.prx", buf, sizeof(buf));

	size = WriteFile("flash0:/vsh/module/recovery.prx", buf, size);
	WriteFile("ms0:/rec.bin", &size, 4);*/
	
	sceKernelDelayThread(3*1000*1000);
	sceSysconPowerStandby();
	
	return 0;
}

