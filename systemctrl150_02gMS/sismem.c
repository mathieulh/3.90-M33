#include <pspsdk.h>
#include <pspkernel.h>

int _sceKernelQueryMemoryInfo(u32 a0, u32 a1, u32 a2)
{
	u32 dummy[16];
	int k1 = pspSdkSetK1(0);
	
	int res = sceKernelQueryMemoryInfo(a0, a1, a2 ? a2 : (u32)dummy);

	pspSdkSetK1(k1);
	return res;
}

