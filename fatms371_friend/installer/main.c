#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <kubridge.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "fatms371_friend.h"

PSP_MODULE_INFO("FatMs_Inst", 0x800, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VSH);

#define printf    pspDebugScreenPrintf

#define FRIEND_PRX	"/kd/fatms371_friend.prx"

typedef struct BtcnfHeader
{
	u32 signature; // 0
	u32 devkit;		// 4
	u32 unknown[2];  // 8
	u32 modestart;  // 0x10
	int nmodes;  // 0x14
	u32 unknown2[2];  // 0x18
	u32 modulestart; // 0x20
	int nmodules;  // 0x24
	u32 unknown3[2]; // 0x28
	u32 modnamestart; // 0x30
	u32 modnameend; // 0x34
	u32 unknown4[2]; // 0x38
}  __attribute__((packed)) BtcnfHeader;

typedef struct ModeEntry
{
	u16 maxsearch;
	u16 searchstart; //
	int mode1;
	int mode2;
	int reserved[5];
} __attribute__((packed)) ModeEntry;

typedef struct ModuleEntry
{
	u32 stroffset;
	int reserved;
	u16 flags;
	u8 loadmode;
	u8 signcheck;
	int reserved2;
	u8  hash[0x10];
} __attribute__((packed)) ModuleEntry;

u8 g_buf[3000000], g_buf2[3000000];

u8 encrypted_md5[16] = 
{
	0xA9, 0xBB, 0x73, 0x3F, 0xE3, 0x1B, 0x14, 0x85, 
	0xBC, 0xA7, 0xA3, 0x84, 0xA3, 0xF4, 0x60, 0xEA
};

u8 decrypted_md5[16] = 
{
	0xCD, 0xD1, 0xD1, 0x92, 0xFE, 0x18, 0x88, 0xB4,
	0x3B, 0x75, 0xEC, 0x1E, 0x37, 0x1E, 0x18, 0xEB
};

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
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0);
	
	if (fd < 0)
	{
		return fd;
	}

	int read = sceIoRead(fd, buf, size);

	sceIoClose(fd);
	return read;
}

int WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	
	if (fd < 0)
	{
		return fd;
	}

	int written = sceIoWrite(fd, buf, size);

	sceIoClose(fd);
	return written;
}

int ChangeBtcnfBuf(u8 *buf, int size)
{
	BtcnfHeader *header = (BtcnfHeader *)buf;
	ModuleEntry *modules, friend;
	char *names;
	int i, j, stroffset;

	if (header->signature != 0x0F803001)
	{
		return -1;
	}

	stroffset = size-header->modnamestart;

	strcpy(buf+size, FRIEND_PRX);
	size += strlen(FRIEND_PRX)+1+sizeof(ModuleEntry);

	header->modnameend = size;
		
	modules = (ModuleEntry *)((u32)header + header->modulestart);
	names = (char *)((u32)header + header->modnamestart);

	for (i = 0; i < header->nmodules; i++)
	{
		if (strcmp(names+modules[i].stroffset, "/kd/fatmsmod.prx") == 0)
		{
			strcpy(names+modules[i].stroffset, "/kd/fatms371.prx");
			break;
		}

		else if (strcmp(names+modules[i].stroffset, FRIEND_PRX) == 0)
		{
			return -3;
		}
	}

	if (i == header->nmodules)
		return -2;
	
	memset(&friend, 0, sizeof(friend));
	friend.stroffset = stroffset;
	friend.flags = 0x4F;
	friend.loadmode = 1;
	friend.signcheck = 0x80;

	for (j = 0; j < 0x10; j++)
	{
		friend.hash[j] = 0xDA;
	}
	
	u32 fatmsoffset = header->modulestart+(i*sizeof(ModuleEntry));

	memcpy(g_buf2, g_buf, size-sizeof(ModuleEntry));
	memcpy(g_buf+fatmsoffset, &friend, sizeof(ModuleEntry));
	memcpy(g_buf+fatmsoffset+sizeof(ModuleEntry), g_buf2+fatmsoffset, size-sizeof(ModuleEntry)-fatmsoffset);

	header->modnamestart += sizeof(ModuleEntry);
	header->nmodules++;

	ModeEntry *modes = (ModeEntry *)(buf+header->modestart);

	for (j = 0; j < header->nmodes; j++)
	{
		modes[j].maxsearch++;
	}
	
	return size;
}

int ChangeBtcnfFile(char *file)
{
	printf("Patching %s... ", file);
	
	int size = ReadFile(file, g_buf, sizeof(g_buf));

	if (size <= 0)
	{
		printf("Cannot open it for reading.\n");
		return -4;
	}

	size = ChangeBtcnfBuf(g_buf, size);

	switch (size)
	{
		case -1:
			printf("Invalid BTCNF version, this program is not compatible with this firmware.\n");
			return -1;
		break;

		case -2:
			printf("WTF, fatmsmod not found.\n"); // Shouldn't happen...
			return -2;
		break;

		case -3:
			printf("Patch was already applied to this file.\n");
			return -3;
		break;
	}

	if (WriteFile(file, g_buf, size) != size)
	{
		printf("Cannot write back to the file\n");
		return -5;
	}

	printf("OK\n");
	return 0;
}

void Reassign()
{
	if (sceIoUnassign("flash0:") < 0)
	{
		ErrorExit(5000, "Error in unassign.\n");
	}

	if (sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDWR, NULL, 0) < 0)
	{
		ErrorExit(5000, "Error in assign.\n");
	}
}

int main(void)
{  
	int update = 0;
	u8 md5[16];
	SceIoStat stat;
	
	pspDebugScreenInit();

	Reassign();

	int size = ReadFile("ms0:/fatmsmod.prx", g_buf, sizeof(g_buf));
	if (size <= 0)
	{
		if (size == 0x80010002)
		{
			ErrorExit(5000, "ms0:/fatmsmod.prx doesn't exist.\n");
		}

		ErrorExit(5000, "Error 0x%08X reading ms0:/fatmsmod.prx.\n");
	}

	sceKernelUtilsMd5Digest(g_buf, size, md5);

	if (memcmp(md5, encrypted_md5, 16) != 0 && memcmp(md5, decrypted_md5, 16) != 0)
	{
		ErrorExit(5000, "ms0:/fatmsmod.prx has an invalid md5.\n"
			            "Use only encrypted or decrypted files, not signchecked.\n");
	}

	if (sceIoGetstat("flash0:/kd/fatms371_friend.prx", &stat) >= 0)
		update = 1;

	printf("Wrtiting fatms371.prx... ");

	size = WriteFile("flash0:/kd/fatms371.prx", g_buf, size);
	if (size <= 0)
	{
		ErrorExit(5000, "ERROR 0x%08X.\n", size);	
	}

	printf("OK\n");

	printf("Writing fatms371_friend.prx... ");

	if (WriteFile("flash0:/kd/fatms371_friend.prx", fatms371_friend, sizeof(fatms371_friend)) != sizeof(fatms371_friend))
	{
		ErrorExit(5000, "ERROR");	
	}

	printf("OK\n");

	if (!update)
	{
		if (kuKernelGetModel() == PSP_MODEL_STANDARD)
		{
			ChangeBtcnfFile("flash0:/kd/pspbtjnf.bin");
			ChangeBtcnfFile("flash0:/kd/pspbtknf.bin");
			ChangeBtcnfFile("flash0:/kd/pspbtlnf.bin");
		}
		else
		{
			ChangeBtcnfFile("flash0:/kd/pspbtjnf_02g.bin");
			ChangeBtcnfFile("flash0:/kd/pspbtknf_02g.bin");
			ChangeBtcnfFile("flash0:/kd/pspbtlnf_02g.bin");
		}
	}

	ErrorExit(5000, "\nFinished.\n\n");

    return 0;
}

