#include <pspsdk.h>
#include <pspkernel.h>
#include <pspinit.h>
#include <pspsysmem_kernel.h>
#include <malloc.h>

SceUID heapid = -1;

int mallocinit()
{
	int size;
	int initmode = sceKernelInitMode();

    if (initmode == PSP_INIT_MODE_VSH)
	{
		size = 14*1024;
	}
	else if (initmode == PSP_INIT_MODE_GAME)
	{ 
		if (sceKernelInitApitype() == 0x123)
			return 0;
		
		size = 45*1024;
	}
	else
	{
		return 0;
	}
		
	heapid = sceKernelCreateHeap(PSP_MEMORY_PARTITION_KERNEL, size, 1, "");
	
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


