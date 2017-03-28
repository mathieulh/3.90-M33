#include <pspsdk.h>
#include <pspkernel.h>
#include <pspthreadman_kernel.h>
#include <psperror.h>

#include <string.h>
#include <malloc.h>

#include "umd9660_driver.h"
#include "isoread.h"
#include "csoread.h"

char umdfile[128];
SceUID umdsema;
SceUID umdfd = -1;
SceUID umdthread = -1;
SceUID umd_s1 = -1, umd_s2 = -1;

int discpointer;
int discsize=0x7FFFFFFF;

int lastLBA = -1;

u8 *umdsector = NULL;

int cso = 0, mounted = 0;

int GetDiscSize()
{
	if (cso == 0)
		return IsofileGetDiscSize(umdfd);

	return CisofileGetDiscSize(umdfd);
}

typedef struct
{
	int lba;
	int nsectors;
	void *buf;
	int  *eod;
	int result;
} ReadSectorsParams;

ReadSectorsParams params;

/*int umd_thread(SceSize args, void *argp)
{
	while (1)
	{
		sceKernelWaitSema(umd_s1, 1, NULL);
		
		if (!mounted)
		{
			int i;

			for (i = 0; i < 0x10; i++)
			{
				if (sceIoLseek32(umdfd, 0, PSP_SEEK_CUR) < 0)
					OpenIso();
				else
					break;
			}
		}

		if (!mounted)
		{
			params.result = SCE_ERROR_ERRNO_ENODEV;
		}

		else if (!cso)
		{
			params.result = IsofileReadSectors(params.lba, params.nsectors, params.buf, params.eod);		
		}
		else
		{
			params.result = CisofileReadSectors(params.lba, params.nsectors, params.buf, params.eod);
		}

		sceKernelSignalSema(umd_s2, 1);
	}
}*/
int Umd9660ReadSectors_()
{
	if (!mounted)
	{
		int i;

		for (i = 0; i < 0x10; i++)
		{
			if (sceIoLseek32(umdfd, 0, PSP_SEEK_CUR) < 0)
				OpenIso();
			else
				break;
		}
	}

	if (!mounted)
	{
		return SCE_ERROR_ERRNO_ENODEV;
	}

	if (!cso)
	{
		return IsofileReadSectors(params.lba, params.nsectors, params.buf, params.eod);		
	}
	
	return CisofileReadSectors(params.lba, params.nsectors, params.buf, params.eod);
}


int Umd9660ReadSectors(int lba, int nsectors, void *buf, int *eod)
{
	params.lba = lba;
	params.nsectors = nsectors;
	params.buf = buf;
	params.eod = eod;

	return sceKernelExtendKernelStack(0x1000, (void *)Umd9660ReadSectors_, NULL);
}


char *GetUmdFile()
{
	return umdfile;
}

void SetUmdFile(char *file)
{
	strncpy(umdfile, file, 128);
	umdfile[127] = 0;
	sceIoClose(umdfd);
	umdfd = -1;
	mounted = 0;
}

int OpenIso()
{
	mounted = 0;
	sceIoClose(umdfd);
	
	umdfd = sceIoOpen(umdfile, PSP_O_RDONLY, 0777);
	if (umdfd >= 0)
	{
		cso = 0;
		if (CisoOpen(umdfd) >= 0)
			cso = 1;		

		discsize = GetDiscSize();
		lastLBA = -1;
		mounted = 1;
		return 0;
	}

	return -1;
}

int ReadUmdFileRetry(void *buf, int size, int fpointer)
{
	int i;

	for (i = 0; i < 0x10; i++)
	{
		if (sceIoLseek32(umdfd, fpointer, PSP_SEEK_SET) < 0)
			OpenIso();
		else
			break;
	}

	if (i == 0x10)
		return SCE_ERROR_ERRNO_ENODEV;

	for (i = 0; i < 0x10; i++)
	{
		int read = sceIoRead(umdfd, buf, size);

		if (read < 0)
			OpenIso();
		else
			return read;
	}

	return SCE_ERROR_ERRNO_ENODEV;
}

void ReadOneSector(int lba)
{
	if (lba != lastLBA)
	{
		Umd9660ReadSectors(lba, 1, umdsector, NULL);
		lastLBA = lba;
	}
}

int umd_init(PspIoDrvArg* arg)
{
	umdsector = (u8 *)malloc(SECTOR_SIZE);	
	
	if (!umdsector)
	{
		return -1;
	}

	umdsema = sceKernelCreateSema("EcsUmd9660DeviceFile", 0, 1, 1, NULL);

	if (umdsema < 0)
	{
		return umdsema;
	}

	/*umd_s1 = sceKernelCreateSema("s1", 0, 0, 1, NULL);
	if (umd_s1 < 0)
	{
		return umd_s1;
	}	

	umd_s2 = sceKernelCreateSema("s2", 0, 0, 1, NULL);
	if (umd_s2 < 0)
	{
		return umd_s2;
	}	

	umdthread = sceKernelCreateThread("Umd9660Worker", umd_thread, 0x18, 0x20000, 0, NULL);

	if (umdthread < 0)
	{
		return umdthread;
	}

	sceKernelStartThread(umdthread, 0, NULL);*/

	return 0;
}

int umd_exit(PspIoDrvArg* arg)
{
	sceKernelWaitSema(umdsema, 1, NULL);
	
	if (umdsector)
	{
		free(umdsector);
	}

	/*if (umdthread >= 0)
	{
		sceKernelDeleteThread(umdthread);
		umdthread = -1;
	}*/

	if (umdsema >= 0)
	{
		sceKernelDeleteSema(umdsema);
		umdsema = -1;		
	}

	/*if (umd_s1 >= 0)
	{
		sceKernelDeleteSema(umd_s1);
		umd_s1 = -1;
	}

	if (umd_s2 >= 0)
	{
		sceKernelDeleteSema(umd_s2);
		umd_s2 = -1;
	}*/

	return 0;
}

int umd_mount(PspIoDrvFileArg *arg)
{
	return 0;
}

int umd_umount(PspIoDrvFileArg *arg)
{
	return 0;
}

int umd_open(PspIoDrvFileArg *arg, char *file, int flags, SceMode mode)
{
	int i;
	
	sceKernelWaitSema(umdsema, 1, NULL);
	
	for (i = 0; i < 0x10; i++)
	{
		if (sceIoLseek32(umdfd, 0, PSP_SEEK_SET) < 0)
			OpenIso();
		else
			break;
	}

	if (i == 0x10)
	{
		sceKernelSignalSema(umdsema, 1);
		return SCE_ERROR_ERRNO_ENODEV;
	}

	arg->arg = 0;
	discpointer = 0;	
	
	sceKernelSignalSema(umdsema, 1);
	return 0;
}

int umd_close(PspIoDrvFileArg *arg)
{
	sceKernelWaitSema(umdsema, 1, NULL);
	sceKernelSignalSema(umdsema, 1);

	return 0;
}

int umd_read(PspIoDrvFileArg *arg, char *data, int len)
{
	sceKernelWaitSema(umdsema, 1, NULL);

	if (discpointer + len > discsize)
	{
		len = discsize - discpointer;
	}

	int res = Umd9660ReadSectors(discpointer, len, data, NULL);

	sceKernelSignalSema(umdsema, 1);
	return res;
}

SceOff umd_lseek(PspIoDrvFileArg *arg, SceOff ofs, int whence)
{
	sceKernelWaitSema(umdsema, 1, NULL);

	if (whence == PSP_SEEK_SET)
	{
		discpointer = ofs;
	}
	else if (whence == PSP_SEEK_CUR)
	{		
		discpointer += ofs;
	}
	else if (whence == PSP_SEEK_END)
	{
		discpointer = discsize-ofs;
	}
	else
	{
		sceKernelSignalSema(umdsema, 1);
		return SCE_ERROR_ERRNO_EINVAL;
	}

	if (discpointer > discsize)
		discpointer = discsize;

	sceKernelSignalSema(umdsema, 1);
	return discpointer;
}

int umd_ioctl(PspIoDrvFileArg *arg, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
	u32 *outdata32 = (u32 *)outdata;

	sceKernelWaitSema(umdsema, 1, NULL);

	if (cmd != 0x01d20001)
	{
		sceKernelSignalSema(umdsema, 1);
		return -1;	
	}
	
	outdata32[0] = discpointer;
	sceKernelSignalSema(umdsema, 1);

	return 0;
}

int ProcessDevctlRead(u32 *indata, u8 *outdata, int outdatasize)
{
	int dataoffset = indata[6];
	int lba = indata[2];
	int datasize = indata[4];
	int dirsectors;
	u8 *p = outdata;
	int remaining = datasize;

	if (indata[6] != 0 && indata[7] != 0)
	{
		dataoffset = SECTOR_SIZE - dataoffset;		
	}
	
	if (datasize <= 0)
	{
		return 0;
	}

	if (dataoffset != 0)
	{
		/* Read and write the first incomplete sector */
		int x = (SECTOR_SIZE - dataoffset);

		if (x > datasize)
			x = datasize;
		
		ReadOneSector(lba);
		memcpy(p, umdsector+dataoffset, x);

		lba += 1;
		p += x;
		datasize -= x;
		remaining = datasize;
	}

	dirsectors = datasize / SECTOR_SIZE; /* n of sectors that can be directly written */
		
	if (dirsectors != 0)
	{
		Umd9660ReadSectors(lba, dirsectors, p, NULL);
		p += (dirsectors*SECTOR_SIZE);
		lba += dirsectors;
		remaining = datasize - (dirsectors*SECTOR_SIZE);
	}

	if ((datasize % SECTOR_SIZE) != 0)
	{
		// Read one and write the remaining
		ReadOneSector(lba);
		memcpy(p, umdsector, remaining);
	}

	return 0;	
}


int umd_devctl(PspIoDrvFileArg *arg, const char *devname, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
	u32 *outdata32 = (u32 *)outdata;
	int res;

	sceKernelWaitSema(umdsema, 1, NULL);

	switch (cmd)
	{
		case 0x01e380c0: case 0x01f200a1: case 0x01f200a2:
		{			
			if (!indata ||!outdata)
			{
				return SCE_ERROR_ERRNO_EINVAL;
			}

			res = ProcessDevctlRead(indata, outdata, outlen);
			sceKernelSignalSema(umdsema, 1);
			
			return res;
		}		

		case 0x01e18030:
		{
			// region related
			sceKernelSignalSema(umdsema, 1);
			return 1; 
		}
		
		case 0x01e38012:
		{
			if (outlen < 0)
				outlen += 3;

			memset(outdata32, 0, outlen);
			outdata32[0] = 0xe0000800;
			outdata32[7] = outdata32[9] = discsize;
			outdata32[2] = 0;

			sceKernelSignalSema(umdsema, 1);
			return 0;
		}

		case 0x01f20001: /* get disc type */
		{
			outdata32[1] = 0x10; /* game */
			outdata32[0] = 0xFFFFFFFF; 
			
			sceKernelSignalSema(umdsema, 1);
			return 0;
		}

		case 0x01f00003:
		{
			sceKernelSignalSema(umdsema, 1); /* activate driver */
			return 0;
		}

		case 0x01f20002:
		{
			outdata32[0] = discsize;
			sceKernelSignalSema(umdsema, 1);
			return 0;
		}
	}

	sceKernelDelayThread(3000000);
	while (1) _sw(0, 0);

	sceKernelSignalSema(umdsema, 1);
	return -1;
}

PspIoDrvFuncs umd_funcs = 
{ 
	umd_init,
	umd_exit,
	umd_open,
	umd_close,
	umd_read,
	NULL,
	umd_lseek,
	umd_ioctl,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,/*umd_mount,*/
	NULL,/*umd_umount,*/
	umd_devctl,
	NULL
};

PspIoDrv umd_driver = { "umd", 0x4, 0x800, "UMD9660", &umd_funcs };


PspIoDrv *getumd9660_driver()
{
	return &umd_driver;
}

