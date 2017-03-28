#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include <malloc.h>

SceUID heapid = -1;

int mallocinit()
{
	heapid = sceKernelCreateHeap(PSP_MEMORY_PARTITION_KERNEL, 60*1024, 1, "SctrlHeap");
	
	return (heapid < 0) ? heapid : 0;
}

void *malloc(size_t size)
{
	return sceKernelAllocHeapMemory(heapid, size);
}

void free(void *p)
{
	sceKernelFreeHeapMemory(heapid, p);
}

int mallocterminate()
{
	return sceKernelDeleteHeap(heapid);
}


