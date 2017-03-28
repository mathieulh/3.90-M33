#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <psploadexec_kernel.h>

#include <stdio.h>
#include <string.h>


PSP_MODULE_INFO("M33IdCanager_Driver", 0x5006, 1, 0);
PSP_MODULE_SDK_VERSION(0x03060010);

int (* popsman_setkeys)(int dummy, u8 *, u8 *) = NULL;

int sceIdMgrRegisterCallback(int (* func)(int dummy, u8 *, u8 *))
{
	popsman_setkeys = func;
	
	return 0;
}

int ReadFile(char *file, void *buf, int size, int mode)
{
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, mode);

	if (fd < 0)
		return fd;

	int read = sceIoRead(fd, buf, size);
	sceIoClose(fd);

	return read;
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

/*unsigned char g_keys[16] = 
{
	0x4D, 0x42, 0x5C, 0xCE, 0xA9, 0x3A, 0xF2, 0xC2, 0x0C, 0xFB, 0x1F, 0xF1, 0x20, 0x0B, 0xBE, 0x22
};*/


#define RETURN_DELETE_FPL(x) sceKernelDeleteFpl(fpl); return x

int GetLicense(char *filename, u8 *buf, u8 *buf2)
{
	u32 *header = (u32 *)buf2;
	SceUID fd;

	fd = sceIoOpen(filename, PSP_O_RDONLY, 0);
	if (fd < 0)
	{
		return fd;
	}

	int res = sceIoRead(fd, buf2, 0x200);
	if (res < 0)
		return -1;

	if (header[0] != 0x50425000)
	{
		return -1;
	}

	res = sceIoLseek(fd, header[0x20/4]+0x560, PSP_SEEK_SET);
	if (res < 0)
		return res;

	res = sceIoRead(fd, buf, 0x34);
	*(u32 *)&buf[0x30] = buf[0x33] + (buf[0x32] << 8) + (buf[0x31] << 16) + (buf[0x30] << 24);

	sceIoClose(fd);
	return res;
}

int BuildLicensePath(char *license, char *buf)
{
	memset(buf, 0, 0x50);
	memcpy(buf, "ms0:/PSP/LICENSE/", 0x11);
	strcat(buf, license);
	strcat(buf, ".rif");

	return 0;
}

int ReadLicense(char *file, char *license, void *buf)
{
	SceIoStat stat; 
	
	int res = sceIoGetstat(file, &stat);

	if (res < 0)
		return res;

	if (stat.st_size != 152)
	{
		return 0x80010016;
	}

	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0);

	if (fd < 0)
		return fd;

	int read = sceIoRead(fd, buf, 152);

	sceIoClose(fd);

	if (read < 0)
		return read;

	if (read < 152)
		return 0x80010016;

	if (strncmp(buf+0x10, license, 0x30) != 0)
		return 0x80010016;

	return 0;
}

int ReadFlash2Act(void *buf)
{
	SceIoStat stat; 

	int res = sceIoAssign("flash2:", "lflash0:0,2", "flashfat2:", IOASSIGN_RDONLY, NULL, 0);
	if (res < 0)
		return 0x80020327;
	
	res = sceIoGetstat("flash2:/act.dat", &stat); 

	if (res < 0 || stat.st_size != 4152)
	{
		sceIoUnassign("flash2:");
		return 0x80020327;
	}

	SceUID fd = sceIoOpen("flash2:/act.dat", PSP_O_RDONLY, 0);

	if (fd < 0)
	{
		sceIoUnassign("flash2:");
		return 0x80010327;
	}

	int read = sceIoRead(fd, buf, 4152);
	sceIoClose(fd);
	sceIoUnassign("flash2:");

	if (read < 4152)
	{		
		return 0x80020327;
	}
	
	return 0;
}

int scePspNpDrm_driver_0F9547E6(void *, void *, void*, u32);


int GetOriginalKeys(char *filename, void *keys, char *keyfile)
{
	u8 *data = NULL;
	
	SceUID fpl = sceKernelCreateFpl("EcsIDCanager", PSP_MEMORY_PARTITION_KERNEL, 0, 0x1364, 1, NULL);

	if (fpl < 0)
	{
		return 0xCA000002;
	}

	if (sceKernelAllocateFpl(fpl, (void *)&data, NULL) < 0)
	{
		RETURN_DELETE_FPL(0xCA000003);
	}

	if (GetLicense(filename, data+0x10d0, data+0x1154) < 0)
	{
		RETURN_DELETE_FPL(0xCA000004);
	}

	if (BuildLicensePath((char *)data+0x10d0, (char *)data+0x1104) < 0)
	{
		RETURN_DELETE_FPL(0xCA000008);
	}

	if (ReadLicense((char *)data+0x1104, (char *)data+0x10D0, data+0x1038) < 0)
	{
		RETURN_DELETE_FPL(0xCA000005);
	}

	if (ReadFlash2Act(data) < 0)
	{
		RETURN_DELETE_FPL(0xCA000006);
	}

	if (scePspNpDrm_driver_0F9547E6(data+0x1354, data, data+0x1038, *(u32 *)&data[0x10D0+0x30]) < 0)
	{
		RETURN_DELETE_FPL(0xCA000007); 
	}

	memcpy(keys, data+0x1354, 0x10);
	WriteFile(keyfile, keys, 0x10);
	
	if (popsman_setkeys)
		popsman_setkeys(0x10, data+0x1354, data+0x1354);

	RETURN_DELETE_FPL(0);
}

int SetKeys(char *filename, void *thekeys)
{		
	int res;
	char path[64];
	char *p;	

	strcpy(path, filename);
	p = strrchr(path, '/');

	if (!p)
	{
		return 0xCA000000;
	}

	strcpy(p+1, "KEYS.BIN");

	res = ReadFile(path, thekeys, 0x10, 0);

	if (res != 0x10)
	{
		SceUID fd = sceIoOpen(filename, PSP_O_RDONLY, 0);

		if (fd >= 0)
		{
			u32 header[0x28/4];

			sceIoRead(fd, header, 0x28);
			sceIoLseek(fd, header[0x20/4], PSP_SEEK_SET);
			sceIoRead(fd, header, 4);
			sceIoClose(fd);

			if (header[0] == 0x464C457F) /* ELF */
			{
				memset(thekeys, 0xDA, 0x10);
				goto SET_KEYS;
			}
		}
		
		res = GetOriginalKeys(filename, thekeys, path);

		if (res < 0)
		{
			return res;
		}
		else
			return 0;
	}	

SET_KEYS:

	if (popsman_setkeys)
		res = popsman_setkeys(0x10, thekeys, thekeys);
	else
		res = 0;

	if (res < 0)
	{
		return 0xCA000001;
	}

	return 0;
}

int sceKernelRegisterGetIdFunc(void *callback);

int module_start(SceSize args, void *argp)
{
	sceKernelRegisterGetIdFunc(SetKeys);
	
	return 0;
}

int module_stop()
{
	return 0;
}




