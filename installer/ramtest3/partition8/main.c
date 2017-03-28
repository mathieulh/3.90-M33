#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <psputilsforkernel.h>
#include <pspsysmem_kernel.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <psploadcore.h>
#include <pspmodulemgr_kernel.h>
#include <pspiofilemgr_kernel.h>

#include <systemctrl.h>

PSP_MODULE_INFO("Partition8Manager", 0x1006, 1, 0);

int Partition8TotalFreeMemsize()
{
	int k1 = pspSdkSetK1(0);

	int res = sceKernelPartitionTotalFreeMemSize(8);

	pspSdkSetK1(k1);
	return res;
}

int Partition8MaxFreeMemsize()
{
	int k1 = pspSdkSetK1(0);

	int res = sceKernelPartitionMaxFreeMemSize(8);

	pspSdkSetK1(k1);
	return res;
}

void *Partition8Malloc(int size)
{
	int k1 = pspSdkSetK1(0);
	SceUID uid;

	uid = sceKernelAllocPartitionMemory(8, "", PSP_SMEM_Low, size, NULL);
	if (uid < 0)
	{
		pspSdkSetK1(k1);
		return NULL;
	}

	pspSdkSetK1(k1);
	return sceKernelGetBlockHeadAddr(uid);
}

int module_start(SceSize args, void *argp)
{
	return 0;
}

int module_stop(void)
{
	return 0;
}
