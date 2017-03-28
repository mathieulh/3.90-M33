#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsysmem_kernel.h>

#include <stdio.h>
#include <string.h>

#include "mediaman.h"
#include "psperror.h"

int drivestat;
int umdcallback;
int errorstat;
int mediaman_sema;

static void UmdCallback()
{
	if (umdcallback >= 0)
	{
		sceKernelNotifyCallback(umdcallback, drivestat);
	}
}

int sceUmdActivate(const int mode, const char *aliasname)
{
	int k1 = pspSdkSetK1(0);
	int res = 0;

	Kprintf("Activate.\n");

	if (strcmp(aliasname, "disc0:") == 0)
	{
		u32 unk = 1;

		int res = sceIoAssign(aliasname, "umd0:", "isofs0:", IOASSIGN_RDONLY, &unk, 4);
		
		if (res >= 0)
		{
			drivestat = SCE_UMD_MEDIA_IN | SCE_UMD_READY | SCE_UMD_READABLE;
			UmdCallback();
		}
	}
	else
	{
		res = SCE_ERROR_ERRNO_EINVAL;
	}	
	
	pspSdkSetK1(k1);
	return res;
}

int sceUmdDeactivate(const int mode, const char *aliasname)
{
	int k1 = pspSdkSetK1(0);

	Kprintf("Deactivate.\n");

	int res = sceIoUnassign(aliasname);

	if (res >= 0)
	{	
		drivestat = SCE_UMD_MEDIA_IN | SCE_UMD_READY;
		UmdCallback();
	}
	
	pspSdkSetK1(k1);
	return res;
}

int sceUmdGetDiscInfo(SceUmdDiscInfo *disc_info)
{
	int k1 = pspSdkSetK1(0);
	int res = 0;

	Kprintf("Disc info.\n");

	if (disc_info && disc_info->uiSize == 8)
	{
		disc_info->uiMediaType = SCE_UMD_FMT_GAME;
	}
	else
	{
		res = SCE_ERROR_ERRNO_EINVAL;
	}

	pspSdkSetK1(k1);
	return res;
}

int sceUmdRegisterUMDCallBack(SceUID cbid)
{
	int k1 = pspSdkSetK1(0);
	int res = 0;

	Kprintf("register umdcallback.\n");

	if (sceKernelGetThreadmanIdType(cbid) == SCE_KERNEL_TMID_Callback)
	{
		umdcallback = cbid;
		UmdCallback();
	}
	else
	{
		res = SCE_ERROR_ERRNO_EINVAL;
	}

	pspSdkSetK1(k1);
	return res;
}

int sceUmdUnRegisterUMDCallBack(SceUID cbid)
{
	int k1 = pspSdkSetK1(0);
	int res = 0;
	uidControlBlock *block;
	
	if (sceKernelGetUIDcontrolBlock(cbid, &block) != 0 || cbid != umdcallback)
	{
		res = SCE_ERROR_ERRNO_EINVAL;
	}
	else
	{
		umdcallback = -1;
	}

	pspSdkSetK1(k1);
	return 0;
}

int sceUmdCheckMedium()
{
	Kprintf("check medium.\n");
	return 1;
}

int sceUmdGetErrorStat()
{
	int k1 = pspSdkSetK1(0);
	
	Kprintf("geterrorstat.\n");
	int res = errorstat;
	
	pspSdkSetK1(k1);
	return res;
}

int sceUmdGetErrorStatus()
{
	Kprintf("get error status.\n");
	return errorstat;
}

void sceUmdSetErrorStatus(int error)
{
	Kprintf("set error status.\n");
	errorstat = error;
	WriteFile("ms0:/seterrorstatus.bin", &error, 4);
}

int sceUmdGetDriveStat()
{
	int k1 = pspSdkSetK1(0);
	
	int res = drivestat;
	Kprintf("get drive stat.\n");
	
	pspSdkSetK1(k1);
	return res;
}

int sceUmdGetDriveStatus()
{
	Kprintf("get drive status.\n");
	return drivestat;
}

void sceUmdSetDriveStatus(int status)
{
	Kprintf("set drive status.\n");
	WriteFile("ms0:/drive_status.bin", &status, 4);
	return;
	
	int intr = sceKernelCpuSuspendIntr();

	if (status & SCE_UMD_MEDIA_OUT)
	{
		drivestat &= ~(SCE_UMD_MEDIA_IN | SCE_UMD_MEDIA_CHG | SCE_UMD_NOT_READY | SCE_UMD_READY | SCE_UMD_READABLE);
	}
	else if (status & (SCE_UMD_MEDIA_IN | SCE_UMD_MEDIA_CHG | SCE_UMD_NOT_READY | SCE_UMD_READY | SCE_UMD_READABLE))
	{
		drivestat &= ~(SCE_UMD_MEDIA_OUT);
	}
	
	if (status & SCE_UMD_NOT_READY)
	{
		drivestat &= ~(SCE_UMD_READY | SCE_UMD_READABLE);
	}
	else if (status & (SCE_UMD_READY | SCE_UMD_READABLE))
	{
		drivestat &= ~(SCE_UMD_NOT_READY);
	}

	drivestat |= status;

	if (drivestat & SCE_UMD_READABLE)
	{
		drivestat |= SCE_UMD_READY;
	}

	if (drivestat & SCE_UMD_READY)
	{
		drivestat |= SCE_UMD_MEDIA_IN;
		sceUmdSetErrorStatus(0);
	}

	sceKernelCpuResumeIntr(intr);
}

static int WaitDriveStat(int stat, u32 timer, int cb)
{
	int k1 = pspSdkSetK1(0);
	int res = 0;
	int (*wait_sema)(SceUID, int, SceUInt *);

	Kprintf("wait drive stat.\n");

	if (stat & (SCE_UMD_MEDIA_OUT | SCE_UMD_MEDIA_IN | SCE_UMD_NOT_READY | SCE_UMD_READY | SCE_UMD_READABLE))
	{
		if (stat & SCE_UMD_READABLE)
			drivestat |= SCE_UMD_READABLE;

		else if (stat & (SCE_UMD_MEDIA_IN | SCE_UMD_READY))
		{
		}

		else if (stat & (SCE_UMD_MEDIA_OUT | SCE_UMD_NOT_READY))
		{
			wait_sema = (cb) ? sceKernelWaitSemaCB : sceKernelWaitSema;
			
			WriteFile("ms0:/critical_point.bin", &cb, 4);
			if (timer != 0)
			{
				wait_sema(mediaman_sema, 1, &timer);
			}
			else
			{
				wait_sema(mediaman_sema, 1, NULL);
			}
		}
	}
	else
	{
		res = SCE_ERROR_ERRNO_EINVAL;
	}

	WriteFile("ms0:/wait_finished.bin", &stat, 4);

	pspSdkSetK1(k1);
	return res;
}

int sceUmdWaitDriveStat(int stat)
{
	return WaitDriveStat(stat, 0, 0);
}

int sceUmdWaitDriveStatCB(int stat, int timer)
{
	return WaitDriveStat(stat, timer, 1);
}

int sceUmdWaitDriveStatWithTimer(int stat, int timer)
{
	return WaitDriveStat(stat, timer, 0);
}

int sceUmdCancelWaitDriveStat()
{
	int k1 = pspSdkSetK1(0);

	int res = sceKernelCancelSema(mediaman_sema);

	pspSdkSetK1(k1);
	return res;
}

int sceUmdReplaceProhibit()
{
	return 0;
}

int sceUmdReplacePermit()
{
	return 0;
}

int sceUmd_659587F7(int error)
{
	Kprintf("659587F7.\n");
	
	if (error == 0)
		return 0;

	if (sceKernelGetCompiledSdkVersion() != 0)
		return error;

	switch (error)
	{
		case 0x8001005b:
			error = 0x80010024;
	
		case 0x80010070:
			error = 0x80010062;
		break;

		case 0x80010071:
			error = 0x80010067;
		break;
	
		case 0x80010074:
			error = 0x8001006e;
		break;

		case 0x80010086:
			error = 0x8001b000;
		break;

		case 0x80010087:
			error = 0x8001007b;
		break;

		case 0x8001b006:
			error = 0x8001007c;	
		break;
	}

	return error;
}

int InitMediaMan()
{
	drivestat = SCE_UMD_READABLE | SCE_UMD_READY | SCE_UMD_MEDIA_IN;
	umdcallback = -1;
	errorstat = 0;

	mediaman_sema = sceKernelCreateSema("MediaManSema", 0, 0, 1, NULL);

	if (mediaman_sema < 0)
		return mediaman_sema;

	Kprintf("Mediaman inited.\n");

	return 0;
}



