/*
 * Method of umd emulation of 2.71 SE don't work in 3.02 OE.
 * Using devhook code instead for anyumd
 *
*/


#include <pspsdk.h>
#include <pspkernel.h>
#include <pspthreadman_kernel.h>
#include <psperror.h>

#include <string.h>
#include <oe_malloc.h>

#include "main.h"
#include "systemctrl.h"
#include "umd9660_driver.h"
#include "isoread.h"
#include "csoread.h"

char umdfile[72];
SceUID umdsema = -1;
SceUID umdfd = -1;
SceUID umdthread = -1;
SceUID umd_s1 = -1, umd_s2 = -1;

int discpointer;
int discsize=0x7FFFFFFF;
u16 disctype = 0x10;
int discout = 1;

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
		return IsofileReadSectors(params.lba, params.nsectors, params.buf);		
	}
	
	return CisofileReadSectors(params.lba, params.nsectors, params.buf);
}

int Umd9660ReadSectors2(int lba, int nsectors, void *buf)
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
		return IsofileReadSectors(lba, nsectors, buf);		
	}
	
	return CisofileReadSectors(lba, nsectors, buf);
}

int Umd9660ReadSectors(int lba, int nsectors, void *buf)
{
	params.lba = lba;
	params.nsectors = nsectors;
	params.buf = buf;
	
	return sceKernelExtendKernelStack(0x2000, (void *)Umd9660ReadSectors_, NULL);
}


char *sctrlSEGetUmdFile()
{
	return umdfile;
}

void sctrlSESetUmdFile(char *file)
{
	strncpy(umdfile, file, sizeof(umdfile)-1);
	sceIoClose(umdfd);
	umdfd = -1;
	mounted = 0;
}

int OpenIso()
{
	mounted = 0;
	sceIoClose(umdfd);
	
	umdfd = sceIoOpen(umdfile, PSP_O_RDONLY | 0x000f0000, 0);
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
		Umd9660ReadSectors(lba, 1, umdsector);
		lastLBA = lba;
	}
}

int umd_init(PspIoDrvArg* arg)
{
	if (!umdsector)	
	{
		umdsector = (u8 *)malloc(SECTOR_SIZE);	
	
		if (!umdsector)
		{
			return -1;
		}
	}

	if (umdsema < 0)
	{
		umdsema = sceKernelCreateSema("", 0, 1, 1, NULL);

		if (umdsema < 0)
		{
			return umdsema;
		}
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
		umdsector = NULL;
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

	int res = Umd9660ReadSectors(discpointer, len, data);
	if (res > 0)
		discpointer += res;

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

int umd_devctl(PspIoDrvFileArg *arg, const char *devname, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
	u32 *outdata32 = (u32 *)outdata;

	sceKernelWaitSema(umdsema, 1, NULL);

	//Kprintf("umd_devctl: %08X\n", cmd);

	switch (cmd)
	{
		/*case 0x01e380c0: case 0x01f200a1: case 0x01f200a2:
		{			
			if (!indata ||!outdata)
			{
				return SCE_ERROR_ERRNO_EINVAL;
			}

			res = ProcessDevctlRead(indata, outdata, outlen);
			sceKernelSignalSema(umdsema, 1);
			
			return res;
		}	*/	

		/*case 0x01e18030:
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
		}*/

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

int SysMemForKernel_C7E57B9C(const u8 *umdid);

static const u8 dummy_umd_id[16] = 
{
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};

static int *sceUmdManGetUmdDiscInfoPatched()
{
	int *(* sceUmdManGetUmdDiscInfo)(void);

	// New nid
	sceUmdManGetUmdDiscInfo = (void *)FindProc("sceUmdMan_driver", "sceUmdMan_driver", 0xA47D28AE);
	
	int *lp = sceUmdManGetUmdDiscInfo();

// patch DISK INFO

	int result;

	sceKernelWaitSema(umdsema, 1, NULL);
	result = discsize; // UmdGetCapacity();
	sceKernelSignalSema(umdsema, 1);

	if( result > 0)
	{
//Kprintf("Patch Info\n");
		lp[0x64/4] = 0xe0000800;        // 00
		lp[0x68/4] = 0;                 // ?
		lp[0x6c/4] = 0x00000000;        // 08
		lp[0x70/4] = result;            // 1C
		lp[0x74/4] = result;            // 24
		lp[0x80/4] = 1;                 // ?
		lp[0x84/4] = 1;                 // ?
	}	

	return lp;
}

static int UmdRead_secbuf(int lba)
{
	int result;
	
	if(lba != lastLBA)	
	{
		result = Umd9660ReadSectors2(lba, 1, umdsector); //UmdRead(iso_sector_buf,lba,0x800);
		if(result < 0 ) return result;
		//if(result != 0x800) return 0x80010163;
	}

	return 0;
}

static int umd_read_block(void *drvState,unsigned char *buf,int read_size,u32 *lba_param)
{
	int result;
	int fpos;
	int size;
	int boffs , bsize;
	int lba;

    // size
	size = lba_param[4];

    // LBA -> offset
    fpos = lba_param[2]*0x800;

    // セクタ内、オフセット
	if( lba_param[6] && (lba_param[5] || lba_param[7]))
	{
		// セクタをまたぐ時だけ、Tはのこりバイト数となる。
		boffs = 0x800-lba_param[6];
	}
	else
	{
		// １セクタ内の終結の時はTはオフセット位置
		boffs = lba_param[6];
	}
	fpos += boffs;
	lba  = lba_param[2];

	// read 1st sector
	if(boffs > 0)
	{
		bsize = 0x800 - boffs;
		if(bsize > size) bsize = size;

		if(bsize>0)
		{
			// read
			result = UmdRead_secbuf(lba);
			if(result < 0 ) return result;

			// copy
			memcpy(buf,&umdsector[boffs],bsize);
			buf  += bsize;
			size -= bsize;
			lba++;
		}
	}
	// centor sector
//	if(lba_param[5])

	int burst_size = size & 0xfffff800;
	if(burst_size)
	{
		result = Umd9660ReadSectors2(lba, burst_size / 0x800, buf);//UmdRead(buf,lba,burst_size);
		if(result < 0 ) return result;
//		if(result != 0x800) return 0x80010163;
		buf  += burst_size;
		size -= burst_size;
		lba  += burst_size/0x800;
	}

	// lat sector
//	if(lba_param[7])
	if(size>0)
	{
		// read
		result = UmdRead_secbuf(lba);
		if(result < 0 ) return result;

		// copy
		memcpy(buf,umdsector,size);
		//odata += bsize;
		//size -= bsize;
		//lba++
	}

	return 0;
}

static int read_umd_iso_file_core(int *argv)
{
	int *drvWork = (int *)argv[0];
	void *buf    = (void *)argv[1];
	int len      = argv[2];
	u32 *param   = (u32 *)argv[3];
	return umd_read_block(drvWork,buf,len,param);
}

static int UmdReadPatched(int *drvWork,void *buf,int len,u32 unk01,u32 *param)
{
	int result;

	// UMD EMU
	//result = umd_read_block(drvWork,buf,len,param);
	int argv[4];
	argv[0] = (int)drvWork;
	argv[1] = (int)buf;
	argv[2] = (int)len;
	argv[3] = (int)param;
	result = (int)sceKernelExtendKernelStack(0x2000,(void *)read_umd_iso_file_core,(void *)&argv);

	return result;
}

u32 sceGpioPortReadPatched()
{
	u32 gpio = *(u32 *)0xbe240004;

	if(discout)
	{
		// HOLD UMD no insert
		gpio &= ~0x04000000;
	}

	return gpio;
}

u32 restore[6];
u32 address[6];

int sctrlSEUmountUmd()
{
	int k1 = pspSdkSetK1(0);
	int i;
	
	if (!mounted)
	{
		pspSdkSetK1(k1);
		return -1;
	}

	umd_exit(NULL);

	discout = 1;

	SysMemForKernel_C7E57B9C(dummy_umd_id);	

	sceKernelCallSubIntrHandler(0x04, 0x1a, 0, 0);
	sceKernelDelayThread(50000);

	for (i = 0; i < sizeof(restore) / 4; i++)
	{
		if (address[i])
		{
			_sw(restore[i], address[i]);
			address[i] = 0;
		}
	}

	ClearCaches();

	pspSdkSetK1(k1);
	return 0;
}

static void GpioPatch()
{
	// using old nid	
	u32 *gpioread = (u32 *)FindProc("sceLowIO_Driver", "sceGpio_driver", 0x4250D44A);
	address[4] = (u32)gpioread;
	restore[4] = gpioread[0];
	address[5] = (u32)&gpioread[1];
	restore[5] = gpioread[1];
	REDIRECT_FUNCTION((u32)gpioread, sceGpioPortReadPatched);
}

void sctrlSESetDiscOut(int out)
{
	int k1 = pspSdkSetK1(0);
	
	GpioPatch();
	ClearCaches();

	discout = out;
	sceKernelCallSubIntrHandler(0x04,0x1a,0,0);
	sceKernelDelayThread(50000);
	pspSdkSetK1(k1);
}

void sctrlSESetDiscType(int type)
{
	int k1 = pspSdkSetK1(0);
	disctype = type;
	pspSdkSetK1(k1);
}

void DoAnyUmd()
{
	u32 *mod, text_addr;
	
	umd_init(NULL);
	SysMemForKernel_C7E57B9C(dummy_umd_id);

	mod = (u32 *)sceKernelFindModuleByName("sceUmd9660_driver");
	if (!mod)
		return;

	text_addr = *(mod+27);

	address[0] = text_addr+0x2004;
	restore[0] = _lw(text_addr+0x2004);
	MAKE_CALL(text_addr+0x2004, UmdReadPatched);

	address[1] = text_addr+0x68C4;
	restore[1] = _lw(text_addr+0x68C4);
	MAKE_JUMP(text_addr+0x68C4, sceUmdManGetUmdDiscInfoPatched);

	mod = (u32 *)sceKernelFindModuleByName("sceUmdMan_driver");
	text_addr = *(mod+27);

	address[2] = text_addr+0xD080;
	restore[2] = _lw(text_addr+0xD080);
	_sw(0x24040000 | disctype, text_addr+0xD080);

	address[3] = text_addr+0xD0F0;
	restore[3] = _lw(text_addr+0xD0F0);
	_sw(0x24040000 | disctype, text_addr+0xD0F0);

	if (address[4] == 0)
		GpioPatch();
	
	discout = 0;
	
	ClearCaches();
	
	sceKernelCallSubIntrHandler(0x04,0x1a,0,0);
	sceKernelDelayThread(50000);
}

