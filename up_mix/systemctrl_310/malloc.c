#include <pspsdk.h>
#include <pspkernel.h>
#include <pspinit.h>
#include <pspsysmem_kernel.h>
#include <malloc.h>

SceUID heapid = -1;

int mallocinit()
{
	int size;

	if (sceKernelInitKeyConfig() == PSP_INIT_KEYCONFIG_POPS)
	{
		return 0;
	}
	else if (sceKernelInitKeyConfig() == PSP_INIT_KEYCONFIG_VSH)
	{
		size = 14*1024;
	}
	else 
	{
		size = 45*1024;
	}	
	
	heapid = sceKernelCreateHeap(PSP_MEMORY_PARTITION_KERNEL, size, 1, "SctrlHeap");
	
	return (heapid < 0) ? heapid : 0;
}

void *oe_malloc(size_t size)
{
	return sceKernelAllocHeapMemory(heapid, size);
}

void oe_free(void *p)
{
	sceKernelFreeHeapMemory(heapid, p);
}

int mallocterminate()
{
	return sceKernelDeleteHeap(heapid);
}


