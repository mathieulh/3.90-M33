#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <psputilsforkernel.h>
#include <systemctrl.h>

#include <stdio.h>
#include <string.h>

#include "icon.h"

PSP_MODULE_INFO("M33PopcornManager", 0x1007, 1, 1);
PSP_MODULE_SDK_VERSION(0x03060010);

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

int new_school;
int use_dracula = 1;

void ClearCaches()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}

void WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_TRUNC | PSP_O_CREAT, 0777);

	sceIoWrite(fd, buf, size);
	sceIoClose(fd);
}/*

int ReadFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0777);

	int read = sceIoRead(fd, buf, size);
	sceIoClose(fd);

	return read;
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

STMOD_HANDLER previous = NULL;

int OnModuleStart(SceModule2 *mod)
{
	if (strcmp(mod->modname, "pops") == 0)
	{
		u32 x;

		// New patch hash check (3.71+)
		// Nullify comparison with 0 (0 = bad here)
		_sw(0, mod->text_addr+0xFADC);
		
		// Use our decompression function
		x = _lw(mod->text_addr+0x177F0);
		_sw(x, mod->text_addr+0xFA7C);	

		// Set a variable to 0x100000 (gap between psar and iso)
		// and patch stores to it (3.71+)
		if (!new_school)
		{
			// 3.90: the patch doesn't seem necessary any more			
		}

		_sw(0, mod->text_addr+0x17DD8); // load whatever document.dat
		_sw(0, mod->text_addr+0x17DDC);

		// New patch in 3.80+. Not 80x80 icons crashes pops
		// li a1, sizeof(icon)
		if (use_dracula)
		{
			_sw(0x24050000 | sizeof(icon), mod->text_addr+0x28558);
			_sw(x, mod->text_addr+0x285A0);
		}

		ClearCaches();
	}

	if (!previous)
		return 0;

	return previous(mod);
}

int sceKernelDeflateDecompress(u8 *dest, u32 destSize, const u8 *src, u32 unknown);

int scePopsManExitVSHKernelPatched(u32 destSize, const u8 *src, u8 *dest)
{
	int k1 = pspSdkSetK1(0);
	int res;

	if (destSize & 0x80000000)
	{
		RebootVSHWithError(destSize);
		pspSdkSetK1(k1);
		return 0;
	}

	if (destSize != 0x9300 && use_dracula)
	{
		// sceIoRead(pbp, icon_buf, sizeof(icon))
		memcpy((void *)src, icon, sizeof(icon));
		pspSdkSetK1(k1);
		return sizeof(icon);
	}

	res = sceKernelDeflateDecompress(dest, destSize, src, 0);	

	if (res == 0x9300)
	{
		res = 0x92FF;		
	}
	
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

int sceIoReadPatched(SceUID fd, u8 *buf, int size)
{
	int res = sceIoRead(fd, buf, size);
	
	if (res == size)
	{
		if (buf[0x41B] == 0x27)
			buf[0x41B] = 0x55;
	}

	return res;
}

int module_start(SceSize args, void *argp)
{
	u32 header[0x28/4];
	int pos, icon_pos, original = 0;

	char *filename = sceKernelInitFileName();
	
	SceUID fd = sceIoOpen(filename, PSP_O_RDONLY, 0);

	if (fd < 0)
		return 1;

	sceIoRead(fd, header, 0x28);
	
	pos = header[9];
	icon_pos = header[3];
	sceIoLseek(fd, pos, PSP_SEEK_SET);
	sceIoRead(fd, header, 0x28);

	if (memcmp(header, "PSTITLE", 7) == 0)
	{
		pos += 0x200;
		new_school = 1;
	}
	else
	{
		pos += 0x400;
	}

	sceIoLseek(fd, pos, PSP_SEEK_SET);
	sceIoRead(fd, header, 0x28);

	if (header[0] == 0x44475000)
		original = 1;
	else
	{
		sceIoLseek(fd, icon_pos, PSP_SEEK_SET);
		sceIoRead(fd, header, 0x28);

		if (header[0] == 0x474E5089 && header[1] == 0x0A1A0A0D
			&& header[3] == 0x52444849)
		{
			// ok, png verified
			if (header[4] == 0x50000000 && header[5] == 0x50000000)
			{
				// 80x80, don't use dracula
				use_dracula = 0;
			}
		}
	}

	sceIoClose(fd);
	
	SceModule2 *mod = sceKernelFindModuleByName("scePops_Manager");

	// Bypass npdrm check
	_sw(0, mod->text_addr+0x14C);

	if (original)
	{
		ClearCaches();
		return 1;
	}
	
	previous = sctrlHENSetStartModuleHandler(OnModuleStart);
		
	int (* SetCompiledSdkVersion)(int) = (void *)sctrlHENFindFunction("sceSystemMemoryManager", "SysMemUserForUser", 0x315AD3A0);
	if (!SetCompiledSdkVersion)
	{
		RebootVSHWithError(0x80000001);
	}

	SetCompiledSdkVersion(0x03090010);
	
	_sw(0, mod->text_addr+0x14E8);
			
	REDIRECT_FUNCTION(mod->text_addr+0x161C, scePopsManExitVSHKernelPatched);
	REDIRECT_FUNCTION(mod->text_addr+0x664, OpenEncryptedPatched);
	REDIRECT_FUNCTION(mod->text_addr+0xADC, OpenEncryptedPatched);
	
	_sw(0, mod->text_addr+0x388); // New patch beginning at 3.52 avoid amctrl calls
	_sw(0, mod->text_addr+0x10F0); // Patch pgd signature check

	// More amctrl shit
	_sw(0, mod->text_addr+0x126C);
	_sw(0, mod->text_addr+0x0C68);
	_sw(0, mod->text_addr+0x0D2C);
	_sw(0, mod->text_addr+0x0FD0);	

	// New patch since 3.90, to patch sound issue in Castlevania
	REDIRECT_FUNCTION(mod->text_addr+0x2810, sceIoReadPatched);

	ClearCaches();	

	return 0;
}

/*
	pos = base_pos + offset;
	pos = pos - (pos & 0x1FF); // 0x201 -> 0x200

	sceIoLseek(pos, SEEK_SET);

*/


