#include <pspsdk.h>
#include <pspkernel.h>

static int thid = -1;

SceUID sceKernelCreateMutex(const char *name, SceUInt attr, int initCount, void *opt)
{
	return sceKernelCreateSema(name, 0, !initCount, 1, NULL);	
}

int sceKernelLockMutex(SceUID mtxid, int lockcount,	SceUInt *timeout)
{
	int t =  sceKernelGetThreadId();
	int res = 0;
	
	if (t != thid)
	{
		res = sceKernelWaitSema(mtxid, 1, timeout);
		thid = t;
	}

	return res;
}

int sceKernelUnlockMutex(SceUID mtxid, int unlockcount)
{
	return sceKernelSignalSema(mtxid, 1);
}

int sceKernelDeleteMutex(SceUID mtxid)
{
	return sceKernelDeleteSema(mtxid);
}

int _sceKernelDelayThread(SceUInt delay)
{
	return sceKernelDelayThread(delay);
}

SceUID _sceKernelCreateEventFlag(const char *name, int attr, int bits, SceKernelEventFlagOptParam *opt)
{
	return sceKernelCreateEventFlag(name, attr, bits, opt);
}

int _sceKernelSetEventFlag(SceUID evid, u32 bits)
{
	return sceKernelSetEventFlag(evid, bits);
}

int _sceKernelWaitEventFlag(int evid, u32 bits, u32 wait, u32 *outBits, SceUInt *timeout)
{
	return sceKernelWaitEventFlag(evid, bits, wait, outBits, timeout);
}

int _sceKernelDeleteEventFlag(int evid)
{
	return sceKernelDeleteEventFlag(evid);
}

int _sceKernelCreateFpl(const char *name, int part, int attr, unsigned int size, unsigned int blocks, struct SceKernelFplOptParam *opt)
{
	return sceKernelCreateFpl(name, part, attr, size, blocks, opt);
}

int _sceKernelAllocateFpl(SceUID uid, void **data, unsigned int *timeout)
{
	return sceKernelAllocateFpl(uid, data, timeout);
}

int _sceKernelDeleteFpl(SceUID uid)
{
	return sceKernelDeleteFpl(uid);
}



