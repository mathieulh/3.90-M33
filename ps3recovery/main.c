#include <pspkernel.h>
#include <pspsdk.h>
#include <pspusb.h>
#include <pspusbstor.h>
#include <psploadexec_kernel.h>
#include <pspsuspend.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pspreg.h>
#include <psplflash_fatfmt.h>
#include <psppower.h>
#include <pspctrl.h>

PSP_MODULE_INFO("Recovery mode", 0x1000, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

#define RGB(r, g, b) (0xFF000000 | ((b)<<16) | ((g)<<8) | (r))
#define setTextColor myDebugScreenSetTextColor
#define setBackColor myDebugScreenSetBackColor
#define setXY myDebugScreenSetXY
#define delay sceKernelDelayThread
#define clearScreen myDebugScreenClear


int usbStatus = 0;
int usbModuleStatus = 0;

PspIoDrv *lflash_driver;
PspIoDrv *msstor_driver;



int (* Orig_IoOpen)(PspIoDrvFileArg *arg, char *file, int flags, SceMode mode);
int (* Orig_IoClose)(PspIoDrvFileArg *arg);
int (* Orig_IoRead)(PspIoDrvFileArg *arg, char *data, int len);
int (* Orig_IoWrite)(PspIoDrvFileArg *arg, const char *data, int len);
SceOff(* Orig_IoLseek)(PspIoDrvFileArg *arg, SceOff ofs, int whence);
int (* Orig_IoIoctl)(PspIoDrvFileArg *arg, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen);
int (* Orig_IoDevctl)(PspIoDrvFileArg *arg, const char *devname, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen);

int unit;

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

static int New_IoOpen(PspIoDrvFileArg *arg, char *file, int flags, SceMode mode)
{
	if (!lflash_driver->funcs->IoOpen)
		return -1;

	if (unit == 0)
		file = "0,0";
	else if (unit == 1)
		file = "0,1";
	else if (unit == 2)
		file = "0,2";
	else
		file = "0,3";

	return lflash_driver->funcs->IoOpen(arg, file, flags, mode);
}

static int New_IoClose(PspIoDrvFileArg *arg)
{
	if (!lflash_driver->funcs->IoClose)
		return -1;

	return lflash_driver->funcs->IoClose(arg);
}

static int New_IoRead(PspIoDrvFileArg *arg, char *data, int len)
{
	if (!lflash_driver->funcs->IoRead)
		return -1;

	return lflash_driver->funcs->IoRead(arg, data, len);
}
static int New_IoWrite(PspIoDrvFileArg *arg, const char *data, int len)
{
	if (!lflash_driver->funcs->IoWrite)
		return -1;

	return lflash_driver->funcs->IoWrite(arg, data, len);
}

static SceOff New_IoLseek(PspIoDrvFileArg *arg, SceOff ofs, int whence)
{
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

int UnassignFlashes(int f0only)
{
	if (sceIoUnassign("flash0:") < 0)
		return -1;

	if (!f0only)
	{
		if (sceIoUnassign("flash1:") < 0)
			return -1;
	}

	return 0;
}

int AssignFlashes(int f0only)
{
	if (sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDWR, NULL, 0) < 0)
		return -1;

	if (!f0only)
	{
		if (sceIoAssign("flash1:", "lflash0:0,1", "flashfat1:", IOASSIGN_RDWR, NULL, 0) < 0)
			return -1;
	}

	return 0;
}

void disableUsb(void) 
{ 
	if(usbStatus) 
	{
		sceUsbDeactivate(0);
		sceUsbStop(PSP_USBSTOR_DRIVERNAME, 0, 0);
		sceUsbStop(PSP_USBBUS_DRIVERNAME, 0, 0);

		msstor_driver->funcs->IoOpen = Orig_IoOpen;
		msstor_driver->funcs->IoClose = Orig_IoClose;
		msstor_driver->funcs->IoRead = Orig_IoRead;
		msstor_driver->funcs->IoWrite = Orig_IoWrite;
		msstor_driver->funcs->IoLseek = Orig_IoLseek;
		msstor_driver->funcs->IoIoctl = Orig_IoIoctl;
		msstor_driver->funcs->IoDevctl = Orig_IoDevctl;

		usbStatus = 0;

		if (unit >= 0)
		{
			AssignFlashes(0);
		}

		scePowerUnlock(0);
	}
}

void enableUsb(int device) 
{
	if (usbStatus)
	{
		disableUsb();
		sceKernelDelayThread(300000);
	}

	if(!usbModuleStatus) 
	{
		pspSdkLoadStartModule("flash0:/kd/semawm.prx", PSP_MEMORY_PARTITION_KERNEL);
		pspSdkLoadStartModule("flash0:/kd/usbstor.prx", PSP_MEMORY_PARTITION_KERNEL);
		pspSdkLoadStartModule("flash0:/kd/usbstormgr.prx", PSP_MEMORY_PARTITION_KERNEL);
		pspSdkLoadStartModule("flash0:/kd/usbstorms.prx", PSP_MEMORY_PARTITION_KERNEL);
		pspSdkLoadStartModule("flash0:/kd/usbstorboot.prx", PSP_MEMORY_PARTITION_KERNEL);
		
		lflash_driver = FindDriver("lflash");
		msstor_driver = FindDriver("msstor");
		
		Orig_IoOpen = msstor_driver->funcs->IoOpen;
		Orig_IoClose = msstor_driver->funcs->IoClose;
		Orig_IoRead = msstor_driver->funcs->IoRead;
		Orig_IoWrite = msstor_driver->funcs->IoWrite;
		Orig_IoLseek = msstor_driver->funcs->IoLseek;
		Orig_IoIoctl = msstor_driver->funcs->IoIoctl;
		Orig_IoDevctl = msstor_driver->funcs->IoDevctl;

		usbModuleStatus = 1;
	}

	if (device != 0)
	{
		unit = device-1;

		UnassignFlashes(0);

		msstor_driver->funcs->IoOpen = New_IoOpen;
		msstor_driver->funcs->IoClose = New_IoClose;
		msstor_driver->funcs->IoRead = New_IoRead;
		msstor_driver->funcs->IoWrite = New_IoWrite;
		msstor_driver->funcs->IoLseek = New_IoLseek;
		msstor_driver->funcs->IoIoctl = New_IoIoctl;
		msstor_driver->funcs->IoDevctl = New_IoDevctl;
	}
	else
	{
		unit = -1;
	}

	scePowerLock(0);

	sceUsbStart(PSP_USBBUS_DRIVERNAME, 0, 0);
	sceUsbStart(PSP_USBSTOR_DRIVERNAME, 0, 0);
	sceUsbstorBootSetCapacity(0x800000);
	sceUsbActivate(0x1c8);
	usbStatus = 1;
}

#define PROGRAM "ms0:/PSP/GAME/PS3NEWS_ARE_STEALERS_I_AM_NOT_GONNA_VISIT_THEM/EBOOT.PBP"

#define printf pspDebugScreenPrintf

int main(void) 
{
	enableUsb(0);
	
	pspDebugScreenInit();

	printf("The update file was corrupted and the system files couldn't be written properly "
	       "because they didn't decrypt fine.\n\n"
	       "Probably you downloaded the files from a criminal page as ps3news.\n"
		   "The author of that page damages homebrew authors, steal credits, programs, threatens people,"
		   " pays people to steal forums databases, etc.\n"
		   "And he has been about to brick your PSP.\n\n"
		   "Report that page and his administrator, Daniel Serafin (or son), living in state of New York (USA)"
		   " to the FBI or any other authorities.\n\n");

	printf("Usb is enabled. You can recover your system putting a recovery EBOOT.PBP into %s\n", PROGRAM);
	printf("\nProceed with X when you are ready.\n");

	while (1)
	{
		SceCtrlData pad;

		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_CROSS)
		{
			break;
		}

		sceKernelDelayThread(10000);
	}

	struct SceKernelLoadExecVSHParam param;

	memset(&param, 0, sizeof(param));
			
	param.size = sizeof(param);
	param.args = strlen(PROGRAM)+1;
	param.argp = PROGRAM;
	param.key = "updater";

	sceKernelLoadExecVSHMs1(PROGRAM, &param);
	       
}

