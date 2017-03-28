#include <pspsdk.h>
#include <pspkernel.h>
#include <pspthreadman_kernel.h>
#include <psperror.h>

#include <string.h>

#include "malloc.h"
#include "umd9660.h"
#include "isoread.h"
#include "csoread.h"


char *umdfile;
SceUID umdsema;
SceUID umdfd = -1;

int discpointer;
int discsize=0x7FFFFFFF;

int lastLBA = -1;

u8 *umdsector = NULL;
u8 *umdpvd = NULL;

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
	int result;
} ReadSectorsParams;

ReadSectorsParams params;

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
		return IsofileReadSectors(params.lba, params.nsectors, params.buf);		
	}
	
	return CisofileReadSectors(params.lba, params.nsectors, params.buf);
}

int Umd9660ReadSectors2(int lba, int nsectors, void *buf)
{	
	Kprintf("umd read.\n");
	
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
		Kprintf("read not mounted.\n");
		return SCE_ERROR_ERRNO_ENODEV;
	}

	int res;

	if (!cso)
	{
		res = IsofileReadSectors(lba, nsectors, buf);	
		Kprintf("end iso read.\n");
		return res;
	}
	
	res = CisofileReadSectors(lba, nsectors, buf);
	Kprintf("end cso read.\n");
}

int Umd9660ReadSectors(int lba, int nsectors, void *buf)
{
	Kprintf("begin read stacked, lba = 0x%08X\n", lba);
	
	params.lba = lba;
	params.nsectors = nsectors;
	params.buf = buf;
	
	int res = sceKernelExtendKernelStack(0x2000, (void *)Umd9660ReadSectors_, NULL);
	Kprintf("end read stacked.\n");
	return res;
}

void WaitMs()
{
	u32 status = 0;

	while (1)
	{
		if (sceIoDevctl("mscmhc0:", 0x02025801, 0, 0, &status, 4) >= 0)
		{		
			if (status == 4)
				break;
		}
		
		sceKernelDelayThread(20000);
	}
}

int OpenIso()
{
	Kprintf("Wait Ms.\n");
	WaitMs();
	Kprintf("Wait Ms finished.\n");

	mounted = 0;
	sceIoClose(umdfd);

	Kprintf(umdfile);
	
	umdfd = sceIoOpen(umdfile, PSP_O_RDONLY | 0x000f0000, 0777);
	if (umdfd >= 0)
	{
		cso = 0;
		if (CisoOpen(umdfd) >= 0)
			cso = 1;		

		discsize = GetDiscSize();
		lastLBA = -1;
		mounted = 1;
		Kprintf("Mounted succesfull, size %d\n", discsize);
		return 0;
	}

	Kprintf("Mounted unsuccesfull.\n");
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
	Kprintf("ReadOnseSector, lba 0x%08X.\n", lba);
	
	if (lba != lastLBA)
	{
		lastLBA = lba;
		Umd9660ReadSectors(lba, 1, umdsector); //***		
	}
}

int umd_init(PspIoDrvArg* arg)
{
	Kprintf("umd_init.\n");
	
	umdsector = (u8 *)oe_malloc(SECTOR_SIZE);	
	umdpvd = (u8 *)oe_malloc(SECTOR_SIZE);
	
	if (!umdsector || !umdpvd)
	{
		return -1;
	}

	umdsema = sceKernelCreateSema("EcsUmd9660DeviceFile", 0, 1, 1, NULL);

	if (umdsema < 0)
	{
		return umdsema;
	}

	while (!mounted)
	{
		Kprintf("Attempting to open iso.\n");
		OpenIso();
		sceKernelDelayThread(20000);
	}

	//Kprintf("umd_init.\n");

	Umd9660ReadSectors2(0x10, 1, umdpvd);

	return 0;
}

int umd_exit(PspIoDrvArg* arg)
{
	sceKernelWaitSema(umdsema, 1, NULL);
	
	if (umdsector)
	{
		oe_free(umdsector);
	}

	if (umdpvd)
	{
		oe_free(umdpvd);
	}

	if (umdsema >= 0)
	{
		sceKernelDeleteSema(umdsema);
		umdsema = -1;		
	}	

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

	//Kprintf("umd open.\n");
	
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

	//Kprintf("umd_read");

	if (discpointer + len > discsize)
	{
		len = discsize - discpointer;
	}

	int res = Umd9660ReadSectors(discpointer, len, data); //***

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

	Kprintf("devctl read, outsize 0x%08X.\n", outdatasize);

	if (indata[6] != 0 && indata[7] != 0)
	{
		dataoffset = SECTOR_SIZE - dataoffset;		
	}

	if (datasize != outdatasize)
		Kprintf("Warning: not matching\n");
	
	if (datasize <= 0)
	{
		return 0;
	}

	if (dataoffset != 0)
	{
		/* Read and write the first incomplete sector */

		Kprintf("dataoffset 0x%08X.\n", dataoffset);
		int x = (SECTOR_SIZE - dataoffset);

		if (x > datasize)
			x = datasize;
		
		ReadOneSector(lba);
		memcpy(p, umdsector+dataoffset, x);

		Kprintf("x = 0x%08X.\n", x);

		lba += 1;
		p += x;
		datasize -= x;
		remaining = datasize;
	}

	dirsectors = datasize / SECTOR_SIZE; /* n of sectors that can be directly written */
		
	if (dirsectors != 0)
	{
		Kprintf("dirsectors 0x%08X\n", dirsectors);
		
		Umd9660ReadSectors(lba, dirsectors, p); //***
		p += (dirsectors*SECTOR_SIZE);
		lba += dirsectors;
		remaining = datasize - (dirsectors*SECTOR_SIZE);
	}

	if ((datasize % SECTOR_SIZE) != 0)
	{
		// Read one and write the remaining
		ReadOneSector(lba);
		memcpy(p, umdsector, remaining);
		Kprintf("Remaining data 0x%08X.\n", remaining);
	}

	if (outdatasize == 0x30680)
	{
		WriteFile("ms0:/last_out.bin", outdata, outdatasize);
		sceKernelDelayThread(1000000);
	}

	return 0;	
}

void WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	sceIoWrite(fd, buf, size);
	sceIoClose(fd);
}

int umd_devctl(PspIoDrvFileArg *arg, const char *devname, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
	u32 *outdata32 = (u32 *)outdata;
	int res;

	sceKernelWaitSema(umdsema, 1, NULL);

	Kprintf("umd_devctl: %08X\n", cmd);

	// Require implementation: 01e180d3 -> return 0x80010086
	// 01f100a3 -> return 0
	// 01e080a8 -> return 0x80010086

	switch (cmd)
	{
		case 0x01e28035:
		{
			*outdata32 = (u32)umdpvd;
			sceKernelSignalSema(umdsema, 1);
			return 0;
		}
	
		case 0x01e280a9:
		{
			*outdata32 = 0x800;
			sceKernelSignalSema(umdsema, 1);
			return 0;
		}
	
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
		
		case 0x01e38034:
		{
			if (!indata || !outdata)
			{
				return SCE_ERROR_ERRNO_EINVAL;
			}

			*outdata32 = 0;
			
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

		case 0x01f20003:
		{
			*outdata32 = discsize;
			sceKernelSignalSema(umdsema, 1);
			return 0;
		}

		case 0x01e180d3: case 0x01e080a8:
		{
			sceKernelSignalSema(umdsema, 1);
			return 0x80010086;
		}

		case 0x01f100a3:
		{
			sceKernelSignalSema(umdsema, 1);
			return 0;
		}
	}

	 Kprintf("unknown devctl.\n");	

	//while (1) _sw(0, 0);
	WriteFile("ms0:/unknown_devctl.bin", &cmd, 4);
	WriteFile("ms0:/unknown_devctl_indata.bin", indata, inlen);


	sceKernelSignalSema(umdsema, 1);
	return 0x80010086;
}

int sceUmd9660_driver_2C6C3F4C()
{
	Kprintf("9660 1.\n");
	return 0;
}

int sceUmd9660_driver_44EF600C()
{
	Kprintf("9660 2.\n");
	return 0;
}

int sceUmd9660_driver_C7CD9CE8()
{
	Kprintf("9660 3.\n");
	return 0;
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

int SysMemForKernel_C7E57B9C(const u8 *umdid);

static const u8 dummy_umd_id[16] = 
{
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};

int InitUmd9660()
{
	int res;
	
	umdfile = GetUmdFile();
	res = sceIoAddDrv(&umd_driver);

	if (res < 0)
		return res;
	
	SysMemForKernel_C7E57B9C(dummy_umd_id);		

	Kprintf("umd9660 inited.\n");
	
	return 0;
}

