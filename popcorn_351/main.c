#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <psputilsforkernel.h>
#include <systemctrl.h>

#include <stdio.h>
#include <string.h>

PSP_MODULE_INFO("M33PopcornManager", 0x1007, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000
#define SC_OPCODE	0x0000000C
#define JR_RA		0x03e00008

#define NOP	0x00000000

#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a); 
#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a); 

#define MAKE_SYSCALL(a, n) _sw(SC_OPCODE | (n << 6), a);
#define JUMP_TARGET(x) (0x80000000 | ((x & 0x03FFFFFF) << 2))

#define REDIRECT_FUNCTION(a, f) _sw(J_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a);  _sw(NOP, a+4);
#define MAKE_DUMMY_FUNCTION0(a) _sw(0x03e00008, a); _sw(0x00001021, a+4);
#define MAKE_DUMMY_FUNCTION1(a) _sw(0x03e00008, a); _sw(0x24020001, a+4);

char filename[128];

void ClearCaches()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}

/*void WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_TRUNC | PSP_O_CREAT, 0777);

	sceIoWrite(fd, buf, size);
	sceIoClose(fd);
}

int ReadFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0777);

	int read = sceIoRead(fd, buf, size);
	sceIoClose(fd);

	return read;
}*/

int module_start(SceSize args, void *argp) __attribute__((alias("_start")));

//char *sceKernelInitFileName();

/*int sceKernelStartModulePatched(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option)
{
	u32 *mod = (u32 *)sceKernelFindModuleByUID(modid);
	u32 text_addr = *(mod+27);

	_sw(0, text_addr+0x12A9C);
	ClearCaches();

	return sceKernelStartModule(modid, argsize, argp, status, option);
}*/

void RebootVSHWithError(u32 error)
{
	struct SceKernelLoadExecVSHParam param;	
	u32 vshmain_args[0x20/4];

	memset(&param, 0, sizeof(param));
	memset(vshmain_args, 0, sizeof(vshmain_args));

	vshmain_args[0/4] = 0x0400;
	vshmain_args[4/4] = 0x20;
	vshmain_args[0x14/4] = error;

	param.size = sizeof(param);
	param.args = 0x400;
	param.argp = vshmain_args;
	param.vshmain_args_size = 0x400;
	param.vshmain_args = vshmain_args;
	param.configfile = "/kd/pspbtcnf.txt";

	sctrlKernelExitVSH(&param);
}

APRS_EVENT previous = NULL;

int OnPspRelSectionEvent(char *modname, u8 *modbuf)
{
	if (strcmp(modname, "pops") == 0)
	{
		u32 text_addr = (u32)(modbuf+0xC0);
		u32 x;

		// Patch hash check
		_sw(0, text_addr+0x13DB8);
		
		// Use our decompression function
		x = _lw(text_addr+0x15C);
		_sw(x, text_addr+0x13C64);	

		// load whatever document.dat
		_sw(0, text_addr+0x1C47C);
		_sw(0, text_addr+0x1C480);

		ClearCaches();
	}

	if (!previous)
		return 0;

	return previous(modname, modbuf);
}

/*int scePopsMan_6768B22F_Patched(u8 *buf)
{
	u32 header[0x28/4];	
	int k1 = pspSdkSetK1(0);	
	u32 *buf32 = (u32 *)buf;

	SceUID fd = sceIoOpen(sceKernelInitFileName(), PSP_O_RDONLY, 0);

	sceIoRead(fd, header, 0x28);
	sceIoLseek(fd, header[9]+0x0400, PSP_SEEK_SET);
	sceIoRead(fd, buf+0x400,  0xb3880);

	u32 pgd_off = buf32[0x1220/4] + header[9];

	u32 *mod = (u32 *)sceKernelFindModuleByName("scePops_Manager");
	u32 text_addr = *(mod+27);

	_sw(pgd_off, text_addr+0x23C0);
	_sw(0, text_addr+0x23C4);

	sceIoClose(fd);

	pspSdkSetK1(k1);
	return 0;	
}*/
int sceIoReadPatched(SceUID fd, u8 *buf, int size)
{
	u32 header[0x28/4];
	SceUID nfd = sceIoOpen(filename, PSP_O_RDONLY, 0);
		
	sceIoRead(nfd, header, 0x28);
	sceIoLseek(nfd, header[9]+0x0400, PSP_SEEK_SET);
	int res = sceIoRead(nfd, buf, size);

	sceIoClose(nfd);
	return res;
}

int sceKernelDeflateDecompress(u8 *dest, u32 destSize, const u8 *src, u32 unknown);

int scePopsManExitVSHKernelPatched(u8 *dest, u32 destSize, const u8 *src, u32 unknown)
{
	int k1 = pspSdkSetK1(0);
	u32 code = (u32)dest;
	int res;

	if (code & 0x80000000)
	{
		RebootVSHWithError(code);
		pspSdkSetK1(0);
		return 0;
	}

	res = sceKernelDeflateDecompress(dest, destSize, src, unknown);

	pspSdkSetK1(k1);
	return res;
}

int OpenEncryptedPatched(char *path, void *keys, int flags, int filemode, int offset)
{
	SceUID fd = sceIoOpen(path, flags, filemode);
	u32 signature;
	int res;

	if (fd < 0)
		return fd;

	sceIoLseek(fd, offset, PSP_SEEK_SET);
	sceIoRead(fd, &signature, 4);

	if (signature != 0x44475000)
	{
		sceIoLseek(fd, offset, PSP_SEEK_SET);
		return fd;
	}
	
	sceIoClose(fd);
	fd = sceIoOpen(path, flags | 0x40000000, filemode);

	if (fd < 0)
		return fd;

	res = sceIoIoctl(fd, 0x04100002, &offset, 4, NULL, 0);
	if (res < 0)
	{
		sceIoClose(fd);
		return res;
	}

	res = sceIoIoctl(fd, 0x04100001, keys, 0x10, NULL, 0);
	if (res < 0)
	{
		sceIoClose(fd);
		return res;
	}

	return fd;
}

int _start(SceSize args, void *argp)
{
	u32 header[0x28/4];

	strncpy(filename, sceKernelInitFileName(), 128);
	
	SceUID fd = sceIoOpen(filename, PSP_O_RDONLY, 0);

	if (fd < 0)
		return 1;

	sceIoRead(fd, header, 0x28);
	sceIoLseek(fd, header[9]+0x0400, PSP_SEEK_SET);
	sceIoRead(fd, header, 0x28);

	if (header[0] == 0x44475000)
		return 1;
	
	u32 *mod = (u32 *)sceKernelFindModuleByName("scePops_Manager");
	
	if (mod)
	{
		u32 text_addr = *(mod+27);

		sceKernelSetCompiledSdkVersion(sceKernelDevkitVersion());

		previous = sctrlHENSetOnApplyPspRelSectionEvent(OnPspRelSectionEvent);
		_sw(0x00001021, text_addr+0x178);
		_sw(0, text_addr+0x59C);
		MAKE_CALL(text_addr+0x5AC, sceIoReadPatched);
		REDIRECT_FUNCTION(text_addr+0x280, scePopsManExitVSHKernelPatched);
		REDIRECT_FUNCTION(text_addr+0xECC, OpenEncryptedPatched);

		ClearCaches();
	}

	return 0;
}

int module_stop(void)
{
	return 0;
}

