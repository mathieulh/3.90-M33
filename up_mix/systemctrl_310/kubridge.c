#include <pspsdk.h>
#include <pspkernel.h>
#include <pspmodulemgr_kernel.h>
#include <pspthreadman_kernel.h>
#include <pspinit.h>
#include <kubridge.h>


#include <string.h>

SceUID kuKernelLoadModule(const char *path, int flags, SceKernelLMOption *option)
{
	int k1, res;

	k1 = pspSdkSetK1(0);
	res = sceKernelLoadModule(path, flags, option);
	pspSdkSetK1(k1);

	return res;
}

SceUID kuKernelLoadModuleWithApitype2(int apitype, const char *path, int flags, SceKernelLMOption *option)
{
	int k1, res;

	k1 = pspSdkSetK1(0);
	res = sceKernelLoadModuleWithApitype2(apitype, path, flags, option);
	pspSdkSetK1(k1);

	return res;
}

int kuKernelInitApitype()
{
	return sceKernelInitApitype();
}

int kuKernelInitFileName(char *filename)
{
	int k1 = pspSdkSetK1(0);
	
	strcpy(filename, sceKernelInitFileName());

	pspSdkSetK1(k1);
	return 0;
}

int kuKernelBootFrom()
{
	return sceKernelBootFrom();
}

int kuKernelInitKeyConfig()
{
	return sceKernelInitKeyConfig();
}

int kuKernelGetUserLevel(void)
{
	int k1, res;

	k1 = pspSdkSetK1(0);
	res = sceKernelGetUserLevel();
	pspSdkSetK1(k1);

	return res;
}

int kuKernelSetDdrMemoryProtection(void *addr, int size, int prot)
{
	int k1, res;

	k1 = pspSdkSetK1(0);
	res = sceKernelSetDdrMemoryProtection(addr, size, prot);
	pspSdkSetK1(k1);

	return res;
}






