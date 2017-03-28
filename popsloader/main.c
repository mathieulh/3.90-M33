#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <psputilsforkernel.h>
#include <pspctrl.h>
#include <pspmoduleexport.h>
#include <pspsysmem_kernel.h>
#include <pspchnnlsv_driver.h>
#include <pspmodulemgr_kernel.h>
#include <psprtc.h>
#include <systemctrl.h>
#include <systemctrl_internal.h>

#include <pspdecrypt.h>

#include <stdio.h>
#include <string.h>

#include "popslibraries.h"

PSP_MODULE_INFO("popsloader_trademark", 0x5007, 2, 1);
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

/* ELF file header */
typedef struct { 
	u32		e_magic;
	u8		e_class;
	u8		e_data;
	u8		e_idver;
	u8		e_pad[9];
	u16		e_type; 
	u16		e_machine; 
	u32		e_version; 
	u32		e_entry; 
	u32		e_phoff; 
	u32		e_shoff; 
	u32		e_flags; 
	u16		e_ehsize; 
	u16		e_phentsize; 
	u16		e_phnum; 
	u16		e_shentsize; 
	u16		e_shnum; 
	u16		e_shstrndx; 
} __attribute__((packed)) Elf32_Ehdr;

typedef struct 
{ 
	u32 p_type; 
	u32 p_offset; 
	u32 p_vaddr; 
	u32 p_paddr; 
	u32 p_filesz; 
	u32 p_memsz; 
	u32 p_flags; 
	u32 p_align; 
} Elf32_Phdr;

typedef struct 
{
	u32 offset;
	u32 length;
	u32 dummy[6];
} IsoIndex;

int sceKernelDeflateDecompress(u8 *dest, u32 destSize, const u8 *src, u32 unknown);

int (*InitStartModule)(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option);

int pops_firmware;
int devkit, new_school;
u32 iso_position, psar_pos;
char filename[128];
BootModuleInfo bootmoduleinfo;
STMOD_HANDLER previous;

void ClearCaches()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}

int OpenRetry(char *file, int flags, int mode)
{
	SceUID fd;
	int i;

	for (i = 0; i < 15; i++)
	{
		fd = sceIoOpen(file, flags, mode);
		if (fd >= 0)
			return fd;

		sceKernelDelayThread(20000);
	}

	return fd;
}

int ReadFile(char *file, int seek, void *buf, int size)
{
	SceUID fd = OpenRetry(file, PSP_O_RDONLY, 0);

	if (fd < 0)
		return fd;

	if (seek)
		sceIoLseek(fd, seek, PSP_SEEK_SET);

	int read = sceIoRead(fd, buf, size);
	sceIoClose(fd);

	return read;
}

int WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	
	if (fd < 0)
		return fd;

	int written = sceIoWrite(fd, buf, size);
	sceIoClose(fd);

	return written;
}

struct PspModuleImport
{
	const char *name;
	unsigned short version;
	unsigned short attribute;
	unsigned char entLen;
	unsigned char varCount;
	unsigned short funcCount;
	u32 *fnids;
	u32 *funcs;
	u32 *vnids;
	u32 *vars;
} __attribute__((packed));

u32 FindImport(char *prxname, char *importlib, u32 nid)
{
	SceModule2 *pMod;
	void *stubTab;
	int stubLen;

	pMod = sceKernelFindModuleByName(prxname);
	if (!pMod)
		return 0;

	pMod = sceKernelFindModuleByUID(pMod->modid);
	if(pMod != NULL)
	{
		int i = 0;

		stubTab = pMod->stub_top;
		stubLen = pMod->stub_size;
		//printf("stubTab %p - stubLen %d\n", stubTab, stubLen);
		while(i < stubLen)
		{
			int count;
			struct PspModuleImport *pImp = (struct PspModuleImport *) (stubTab + i);

			if(pImp->name)
			{
				//printf("Import Library %s, attr 0x%04X\n", pImp->name, pImp->attribute);
			}
			else
			{
				//printf("Import Library %s, attr 0x%04X\n", "Unknown", pImp->attribute);
			}

			if(pImp->funcCount > 0)
			{
				//printf("Function Imports:\n");
				for(count = 0; count < pImp->funcCount; count++)
				{
					//printf("Entry %-3d: UID 0x%08X, Function 0x%08X\n", count+1, pImp->fnids[count], (u32) &pImp->funcs[count*2]);

					if (pImp->name)
					{
						if (strcmp(pImp->name, importlib) == 0)
						{
							if (pImp->fnids[count] == nid)
							{
								return (u32)&pImp->funcs[count*2];
							}
						}						
					}

				}
			}

			i += (pImp->entLen * 4);
		}
	}
	
	return 0;
}

int sceKernelLoadModulePatched(const char *path, int flags, SceKernelLMOption *option)
{	
	switch (pops_firmware)
	{
		case 0x03000010:
			path = "ms0:/seplugins/popsloader/pops300.prx";
		break;

		case 0x03000110:
			path = "ms0:/seplugins/popsloader/pops301.prx";
		break;

		case 0x03000210:
			path = "ms0:/seplugins/popsloader/pops302.prx";
		break;
	
		case 0x03000310:
			path = "ms0:/seplugins/popsloader/pops303.prx";
		break;
	
		case 0x03010010:
			path = "ms0:/seplugins/popsloader/pops310.prx";
		break;
	
		case 0x03010110:
			path = "ms0:/seplugins/popsloader/pops311.prx";
		break;
	
		case 0x03030010:
			path = "ms0:/seplugins/popsloader/pops330.prx";
		break;
	
		case 0x03040010:
			path = "ms0:/seplugins/popsloader/pops340.prx";
		break;	
		
		case 0x03050110:
			path = "ms0:/seplugins/popsloader/pops351.prx";
		break;

		case 0x03050210:
			path = "ms0:/seplugins/popsloader/pops352.prx";
		break;

		case 0x03070110:
			path = "ms0:/seplugins/popsloader/pops371.prx";
		break;

		case 0x03070210:
			path = "ms0:/seplugins/popsloader/pops372.prx";
		break;
	}
	
	return sceKernelLoadModule(path, flags, option);
}

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

int scePopsManExitVSHKernelPatched(u8 *dest, u32 destSize, const u8 *src, u32 unknown)
{
	int k1 = pspSdkSetK1(0);
	u32 code = (u32)dest;
	int res;

	if (code & 0x80000000)
	{
		RebootVSHWithError(code);
		pspSdkSetK1(k1);
		return 0;
	}

	res = sceKernelDeflateDecompress(dest, destSize, src, 0);

	pspSdkSetK1(k1);
	return res;
}

int scePopsManExitVSHKernelPatched2(u32 destSize, const u8 *src, u8 *dest)
{
	int k1 = pspSdkSetK1(0);
	int res;

	if (destSize & 0x80000000)
	{
		RebootVSHWithError(destSize);
		pspSdkSetK1(k1);
		return 0;
	}

	res = sceKernelDeflateDecompress(dest, destSize, src, 0);

	pspSdkSetK1(k1);
	return res;
}

int OpenEncryptedPatched(char *path, void *keys, int flags, int filemode, int offset)
{
	SceUID fd = OpenRetry(path, flags, filemode);
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
	fd = OpenRetry(path, flags | 0x40000000, filemode);

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

int sceIoReadPatched(SceUID fd, u8 *buf, SceSize size)
{
	SceUID nfd = sceIoOpen(filename, PSP_O_RDONLY, 0);

	sceIoLseek(nfd, iso_position, PSP_SEEK_SET);
	int res = sceIoRead(nfd, buf, size);

	sceIoClose(nfd);
	return res;
}

SceUID document= -1;

SceUID sceIoOpenPatched(const char *file, int flags, SceMode mode)
{
	return sceIoOpen(file, PSP_O_RDONLY, mode);
}

SceUID sceIoOpenPatched2(const char *file, int flags, SceMode mode)
{
	u32 signature;

	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, mode);

	if (fd < 0)
		return fd;

	sceIoRead(fd, &signature, 4);

	if (signature == 0x44475000)
	{
		sceIoClose(fd);
		fd = sceIoOpen(file, flags, mode);
		document = fd;
	}
	
	return fd;
}

int sceIoIoctlPatched(SceUID fd, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{	
	int res;

	if (fd == document)	
		res = sceIoIoctl(fd, cmd, indata, inlen, outdata, outlen);
	else
		res = -1;
		
	if (res < 0)
	{
		res = 0;

		if (cmd == 0x04100002)
		{
			u32 offset = *(u32 *)indata;	
			sceIoLseek(fd, offset, PSP_SEEK_SET);
		}
	}

	return res;
}

int scePopsManIsoOpenPatched(u32 *pos)
{
	int k1 = pspSdkSetK1(0);
	u16 *block;

	SceUID fd = sceIoOpen(filename, PSP_O_RDONLY, 0);
	if (fd < 0)
	{
		pspSdkSetK1(k1);
		return fd;
	}

	if (sceKernelGetUIDcontrolBlock(fd, (void *)&block) == 0)
	{
		block[0x16/2] |= 0x0015;
	}

	*pos = psar_pos;

	pspSdkSetK1(k1);
	return fd;
}

int scePopsMan_6768B22F_Patched(u8 *buf)
{
	u32 header[0x28/4];	
	int k1 = pspSdkSetK1(0);	
	u32 *buf32 = (u32 *)buf;

	SceUID fd = OpenRetry(filename, PSP_O_RDONLY, 0);

	sceIoRead(fd, header, 0x28);
	sceIoLseek(fd, header[9]+0x0400, PSP_SEEK_SET);
	sceIoRead(fd, buf+0x400,  0xb3880);

	u32 pgd_off = buf32[0x1220/4] + header[9];

	u32 *mod = (u32 *)sceKernelFindModuleByName("scePops_Manager");
	u32 text_addr = *(mod+27);

	switch (pops_firmware)
	{
		case 0x03000010: case 0x03000110: case 0x03000210: // 3.00-3.02
			_sw(pgd_off, text_addr+0x12F0);
			_sw(0, text_addr+0x12F4);
		break;

		case 0x03000310: // 3.03
			_sw(pgd_off, text_addr+0x13A0); 
			_sw(0, text_addr+0x13A4);
		break;

		case 0x03010010: case 0x03010110: // 3.10-3.11
			_sw(pgd_off, text_addr+0x23C0);
			_sw(0, text_addr+0x23C4);
		break;
	}

	sceIoClose(fd);

	pspSdkSetK1(k1);
	return 0;	
}

void PatchPopsMan300(u32 text_addr)
{
	// Patch for 3.30- specific for popsloader
	REDIRECT_FUNCTION(text_addr+0x6D0, scePopsManIsoOpenPatched);
	
	MAKE_CALL(text_addr+0x98, sceKernelLoadModulePatched);	
	_sw(0x00001021, text_addr+0x50);
	REDIRECT_FUNCTION(text_addr+0x4A8, scePopsMan_6768B22F_Patched);
	REDIRECT_FUNCTION(text_addr+0x144, scePopsManExitVSHKernelPatched);
	REDIRECT_FUNCTION(text_addr+0xA44, OpenEncryptedPatched);
}

void PatchPopsMan303(u32 text_addr)
{
	// Patch for 3.30- specific for popsloader
	REDIRECT_FUNCTION(text_addr+0x764, scePopsManIsoOpenPatched);
	
	MAKE_CALL(text_addr+0x98, sceKernelLoadModulePatched);	
	_sw(0x00001021, text_addr+0x50);
	REDIRECT_FUNCTION(text_addr+0x53C, scePopsMan_6768B22F_Patched);
	REDIRECT_FUNCTION(text_addr+0x144, scePopsManExitVSHKernelPatched);
	REDIRECT_FUNCTION(text_addr+0xAD8, OpenEncryptedPatched);
}

void PatchPopsMan310(u32 text_addr)
{
	// Patch for 3.30- specific for popsloader
	REDIRECT_FUNCTION(text_addr+0x71C, scePopsManIsoOpenPatched);
	
	MAKE_CALL(text_addr+0x98, sceKernelLoadModulePatched);	
	_sw(0x00001021, text_addr+0x50);
	REDIRECT_FUNCTION(text_addr+0x554, scePopsMan_6768B22F_Patched);
	REDIRECT_FUNCTION(text_addr+0x144, scePopsManExitVSHKernelPatched);
	REDIRECT_FUNCTION(text_addr+0xAFC, OpenEncryptedPatched);
}

void PatchPopsMan330(u32 text_addr)
{
	// Patch for 3.30- specific for popsloader
	REDIRECT_FUNCTION(text_addr+0x8AC, scePopsManIsoOpenPatched);

	MAKE_CALL(text_addr+0x138, sceKernelLoadModulePatched);
	_sw(0x00001021, text_addr+0xF0);	
	_sw(0, text_addr+0x5A8); // Nop fd error
	MAKE_CALL(text_addr+0x5B8, sceIoReadPatched);
	REDIRECT_FUNCTION(text_addr+0x1E8, scePopsManExitVSHKernelPatched);
	REDIRECT_FUNCTION(text_addr+0xC90, OpenEncryptedPatched);	
}

void PatchPopsMan340(u32 text_addr)
{
	MAKE_CALL(text_addr+0x138, sceKernelLoadModulePatched);
	_sw(0x00001021, text_addr+0xF0);
	_sw(0, text_addr+0x504); // Nop fd error
	MAKE_CALL(text_addr+0x514, sceIoReadPatched);
	REDIRECT_FUNCTION(text_addr+0x1E8, scePopsManExitVSHKernelPatched);
	REDIRECT_FUNCTION(text_addr+0xE34, OpenEncryptedPatched);
}

void PatchPopsMan351(u32 text_addr)
{
	MAKE_CALL(text_addr+0x1C0, sceKernelLoadModulePatched);
	_sw(0x00001021, text_addr+0x178);
	_sw(0, text_addr+0x59C);
	MAKE_CALL(text_addr+0x5AC, sceIoReadPatched);
	REDIRECT_FUNCTION(text_addr+0x280, scePopsManExitVSHKernelPatched);
	REDIRECT_FUNCTION(text_addr+0xECC, OpenEncryptedPatched);
}

void PatchPopsMan352(u32 text_addr)
{
	MAKE_CALL(text_addr+0x37C, sceKernelLoadModulePatched);
	// Patch changed in 3.52, before patch nullified function, now nullify result check
	_sw(0, text_addr+0x2DC); 		
	REDIRECT_FUNCTION(text_addr+0x84, scePopsManExitVSHKernelPatched);
	MAKE_CALL(text_addr+0x9E8, sceIoOpenPatched);
	MAKE_CALL(text_addr+0xFE4, sceIoOpenPatched2);
	MAKE_CALL(text_addr+0x119C, sceIoOpenPatched2);
	MAKE_CALL(text_addr+0x7C8, sceIoIoctlPatched);
	MAKE_CALL(text_addr+0x7EC, sceIoIoctlPatched);
	MAKE_CALL(text_addr+0xA10, sceIoIoctlPatched);
	MAKE_CALL(text_addr+0xA38, sceIoIoctlPatched);
	MAKE_CALL(text_addr+0x100C, sceIoIoctlPatched);
	MAKE_CALL(text_addr+0x1034, sceIoIoctlPatched);
	MAKE_CALL(text_addr+0x11C4, sceIoIoctlPatched);
	MAKE_CALL(text_addr+0x11E8, sceIoIoctlPatched);
	MAKE_CALL(text_addr+0x1C04, sceIoIoctlPatched);	
	_sw(0, text_addr+0xA68); // New patch beginning at 3.52 avoid amctrl calls
	_sw(0x100000d0, text_addr+0x10C0);
}

void PatchPopsMan371(u32 text_addr)
{
	MAKE_CALL(text_addr+0x1B8, sceKernelLoadModulePatched);
	// Bypass npdrm check (new in 3.71)
	_sw(0, text_addr+0x5B0);
	
	REDIRECT_FUNCTION(text_addr+0x27C, scePopsManExitVSHKernelPatched);
	REDIRECT_FUNCTION(text_addr+0xBE8, OpenEncryptedPatched);
	REDIRECT_FUNCTION(text_addr+0x1064, OpenEncryptedPatched);
	_sw(0, text_addr+0x7FC); // New patch beginning at 3.52 avoid amctrl calls
	_sw(0, text_addr+0x1658); // Patch pgd signature check

	// More amctrl shit
	_sw(0, text_addr+0x11F0);
	_sw(0, text_addr+0x12B4);
	_sw(0, text_addr+0x1558);
	_sw(0, text_addr+0x17E4);	
}

void PatchPopsMan372(u32 text_addr)
{
	MAKE_CALL(text_addr+0x159C, sceKernelLoadModulePatched);
	// Bypass npdrm check (new in 3.71)
	_sw(0, text_addr+0x14C);
	
	REDIRECT_FUNCTION(text_addr+0x1660, scePopsManExitVSHKernelPatched2);
	REDIRECT_FUNCTION(text_addr+0x658, OpenEncryptedPatched);
	REDIRECT_FUNCTION(text_addr+0xAD4, OpenEncryptedPatched);
	_sw(0, text_addr+0x370); // New patch beginning at 3.52 avoid amctrl calls
	_sw(0, text_addr+0x10C8); // Patch pgd signature check

	// More amctrl shit
	_sw(0, text_addr+0x1254);
	_sw(0, text_addr+0x0C60);
	_sw(0, text_addr+0x0D24);
	_sw(0, text_addr+0x0FC8);		
}

void PatchPops300(u32 text_addr)
{
	u32 x;
	
	// Patch hash check
	_sw(0, text_addr+0x12358);

	// Use our decompression function
	x = _lw(text_addr+0x5E8);
	_sw(x, text_addr+0x12314);
}

void PatchPops303(u32 text_addr)
{
	u32 x;
	
	// Patch hash check
	_sw(0, text_addr+0x12A9C);

	// Use our decompression function
	x = _lw(text_addr+0x5D4);
	_sw(x, text_addr+0x12A60);							
}

void PatchPops310(u32 text_addr)
{
	u32 x;

	// Patch hash check
	_sw(0, text_addr+0x10628);

	// Use our decompression function
	x = _lw(text_addr+0x5D4);
	_sw(x, text_addr+0x105F0);

	// load whatever document.dat
	_sw(0, text_addr+0x1C104);
	_sw(0, text_addr+0x1C108);
}

void PatchPops311(u32 text_addr)
{
	u32 x;

	// Patch hash check
	_sw(0, text_addr+0x107CC);

	// Use our decompression function
	x = _lw(text_addr+0x5D4);
	_sw(x, text_addr+0x10794);

	// load whatever document.dat
	_sw(0, text_addr+0x1C0C8);
	_sw(0, text_addr+0x1C0CC);
}

void PatchPops330(u32 text_addr)
{
	u32 x;

	// Patch hash check
	_sw(0, text_addr+0x138E4);

	// Use our decompression function
	x = _lw(text_addr+0x10C);
	_sw(x, text_addr+0x13788);	

	// load whatever document.dat
	_sw(0, text_addr+0x1B474);
	_sw(0, text_addr+0x1B478);
}

void PatchPops340(u32 text_addr)
{
	u32 x;

	// Patch hash check
	_sw(0, text_addr+0x139AC);

	// Use our decompression function
	x = _lw(text_addr+0x10C);
	_sw(x, text_addr+0x13850);

	// load whatever document.dat
	_sw(0, text_addr+0x1B8C4);
	_sw(0, text_addr+0x1B8C8);
}

void PatchPops351(u32 text_addr)
{
	u32 x;
	
	// Patch hash check
	_sw(0, text_addr+0x13DB8);
		
	// Use our decompression function
	x = _lw(text_addr+0x15C);
	_sw(x, text_addr+0x13C64);	

	// load whatever document.dat
	_sw(0, text_addr+0x1C47C);
	_sw(0, text_addr+0x1C480);
}

void PatchPops352(u32 text_addr)
{
	u32 x;

	// Patch hash check
	_sw(0, text_addr+0x13FA4);
		
	// Use our decompression function
	x = _lw(text_addr+0x16C);
	_sw(x, text_addr+0x13E50);	

	// load whatever document.dat
	_sw(0, text_addr+0x1AF20); 
	_sw(0, text_addr+0x1AF24);
}

void PatchPops371(u32 text_addr)
{
	u32 x;

	// New patch hash check (3.71+)
	// Nullify comparison with 0 (0 = bad here)
	_sw(0, text_addr+0x1428C);
		
	// Use our decompression function
	x = _lw(text_addr+0x16C);
	_sw(x, text_addr+0x14100);	

	// Set a variable to 0x100000 (gap between psar and iso)
	// and patch stores to it (3.71+)
	if (!new_school)
	{
		_sw(0x100000, text_addr+0x11B2A8);
		_sw(0, text_addr+0x12F64);
		_sw(0, text_addr+0x1431C);
	}
		
	_sw(0, text_addr+0x1B04C); // load whatever document.dat
	_sw(0, text_addr+0x1B050);
}

void PatchPops372(u32 text_addr)
{
	u32 x;

	// New patch hash check (3.71+)
	// Nullify comparison with 0 (0 = bad here)
	_sw(0, text_addr+0x13DD4);
		
	// Use our decompression function
	x = _lw(text_addr+0x1B0);
	_sw(x, text_addr+0x13D6C);	// 0x9300
	
	// Set a variable to 0x100000 (gap between psar and iso)
	// and patch stores to it (3.71+)
	if (!new_school)
	{
		_sw(0x100000, text_addr+0xF7DEC);
		_sw(0, text_addr+0x13E74);
		_sw(0, text_addr+0x1307C);
	}

	// New patch in 3.72+ to fix sound issue in decompressed iso
	_sh(0x9300, text_addr+0x1D3D4);
		
	_sw(0, text_addr+0x1DA04); // load whatever document.dat
	_sw(0, text_addr+0x1DA08);	
}

int OnModuleStart(SceModule2 *mod)
{
	u32 text_addr = mod->text_addr;
	char *modname = mod->modname;
		
	if (strcmp(modname, "scePops_Manager") == 0)
	{
		switch (pops_firmware)
		{
			case 0x03000010: case 0x03000110: case 0x03000210:// 3.00-3.02
				PatchPopsMan300(text_addr);
			break;
		
			case 0x03000310: // 3.03
				PatchPopsMan303(text_addr);
			break;
		
			case 0x03010010: case 0x03010110: // 3.10-3.11
				PatchPopsMan310(text_addr);
			break;
		
			case 0x03030010:
				PatchPopsMan330(text_addr);
			break;
		
			case 0x03040010:
				PatchPopsMan340(text_addr);				
			break;

			case 0x03050110:
				PatchPopsMan351(text_addr);
			break;

			case 0x03050210:
				PatchPopsMan352(text_addr);
			break;

			case 0x03070110:
				PatchPopsMan371(text_addr);
			break;

			case 0x03070210:
				PatchPopsMan372(text_addr);
			break;
		}
		
		ClearCaches();
	}
	else if (strcmp(modname, "pops") == 0)
	{
		switch (pops_firmware)
		{				
			case 0x03000010: case 0x03000110: case 0x03000210: // 3.00-3.02
				PatchPops300(text_addr);
			break;
		
			case 0x03000310: // 3.03
				PatchPops303(text_addr);
			break;
		
			case 0x03010010: // 3.10
				PatchPops310(text_addr);
			break;
		
			case 0x03010110: // 3.11
				PatchPops311(text_addr);
			break;
		
			case 0x03030010:
				PatchPops330(text_addr);
			break;
		
			case 0x03040010: 
				PatchPops340(text_addr);
			break;

			case 0x03050110:
				PatchPops351(text_addr);
			break;

			case 0x03050210:
				PatchPops352(text_addr);
			break;

			case 0x03070110:
				PatchPops371(text_addr);
			break;

			case 0x03070210:
				PatchPops372(text_addr);
			break;
		}

		ClearCaches();		
	}
	
	if (!previous)	
		return 0;

	return previous(mod);
}

LibraryData *FindLib(const char *lib)
{
	int i;

	if (!lib)
		return NULL;

	for (i = 0; i < N_LIBRARIES_370; i++)
	{
		if (strcmp(lib, libraries370[i].name) == 0)
			return &libraries370[i];
	}

	return NULL;
}

u32 FindNewNid(LibraryData *lib, u32 nid)
{
	int i;

	for (i = 0; i < lib->nnids; i++)
	{
		if (lib->nids[i].input_nid == nid)
			return lib->nids[i].output_nid;
	}

	return 0;
}

int NidsHandler(SceLibraryStubTable *import)
{
	if ((pops_firmware & 0xFFFF0000) == 0x03070000)
	{
		LibraryData *data = FindLib(import->libname);

		if (data)
		{		
			int i;
			
			for (i = 0; i < import->stubcount; i++)
			{
				u32 nnid = FindNewNid(data, import->nidtable[i]);
				if (nnid)
				{
					import->nidtable[i] = nnid;
				}
			}
		}
	}

	return 0;
}

//////// Menu and confgiuration stuff ////////

#define LINE_CHARS 68
#define N_LINES	34
#define HIGHLIGHT_COLOR 0x00FAEA1F
#define NITEMS	13
#define CONFIG_FILE	"ms0:/seplugins/popsloader/config.bin"

typedef struct
{
	char gamecode[9];
	u8	 pad[3];
	int	 firmware;	
} PopsLoaderConfig;


char *strings[NITEMS] =
{
	"3.00 pops",
	"3.01 pops",
	"3.02 pops",
	"3.03 pops",
	"3.10 pops",
	"3.11 pops",
	"3.30 pops",
	"3.40 pops",
	"3.51 pops",
	"3.52 pops",
	"3.71 pops",
	"3.72 pops",
	"Original from flash"
};

int firmwares[NITEMS] =
{
	0x03000010,
	0x03000110,
	0x03000210,
	0x03000310,
	0x03010010,
	0x03010110,
	0x03030010,
	0x03040010,
	0x03050110,
	0x03050210,
	0x03070110,
	0x03070210,
	0
};

char *items[NITEMS];
int  values[NITEMS];

PopsLoaderConfig g_config;
SceCtrlData pad;

int GetConfiguration(PopsLoaderConfig *config)
{
	PopsLoaderConfig config_buf[32];
	u32 *header = (u32 *)config_buf;
	char gamecode[9]; 
	SceUID fd;	
	
	memset(config, 0, sizeof(PopsLoaderConfig));

	fd = OpenRetry(filename, PSP_O_RDONLY, 0);
	if (fd < 0)
	{
		return 0x80020001;
	}

	sceIoRead(fd, header, 0x28);
	if (header[0] != 0x50425000) // PBP
	{
		sceIoClose(fd);
		return 0x80020098;
	}

	if (!new_school)
		sceIoLseek(fd, header[0x24/4]+0x0400, PSP_SEEK_SET);
	else
		sceIoLseek(fd, header[0x24/4]+0x0200, PSP_SEEK_SET);

	sceIoRead(fd, header, 0x20);
	sceIoClose(fd);

	if (header[0] == 0x44475000) // PGD
	{
		return 0x80020098;
	}

	memcpy(gamecode, ((u8 *)header)+1, 4);
	memcpy(gamecode+4, ((u8 *)header)+6, 5);

	fd = OpenRetry(CONFIG_FILE, PSP_O_RDONLY, 0);
	if (fd < 0)
	{
		memcpy(config->gamecode, gamecode, 9);
		return 0x80010002;
	}

	int read;
	int totalread = 0;

	do
	{
		read = sceIoRead(fd, config_buf, sizeof(config_buf));
		
		if (read > 0)
		{
			int n = read / sizeof(PopsLoaderConfig);
			int i;

			totalread += read;

			for (i = 0; i < n; i++)
			{
				if (memcmp(config_buf[i].gamecode, gamecode, 9) == 0)
				{
					sceIoClose(fd);
					memcpy(config, config_buf+i, sizeof(PopsLoaderConfig));
					return (totalread - read) + (i*sizeof(PopsLoaderConfig)); // position in file
				}
			}
		}
	} while (read == sizeof(config_buf));

	sceIoClose(fd);
	memset(config, 0, sizeof(PopsLoaderConfig));
	memcpy(config->gamecode, gamecode, 9);
	return -1;
}

void SaveConfiguration(int createfile, int pos, PopsLoaderConfig *config)
{
	int flags = PSP_O_WRONLY;

	if (createfile)
		flags |= PSP_O_CREAT | PSP_O_TRUNC;
	else if (pos < 0)
		flags |= PSP_O_APPEND;
	else
		flags |= PSP_O_RDONLY;

	SceUID fd = OpenRetry(CONFIG_FILE, flags, 0777);
	if (fd < 0)
		return;

	if (pos > 0)
	{
		sceIoLseek(fd, pos, PSP_SEEK_SET);
	}	

	sceIoWrite(fd, config, sizeof(PopsLoaderConfig));	
	sceIoClose(fd);
}

void PrintItem(int i, int n, char *s, int highlight)
{
	pspDebugScreenSetXY((LINE_CHARS/2) - (strlen(s)/2) - 2, ((N_LINES/2) - (n/2)) + i - 5);
	pspDebugScreenSetTextColor((highlight) ? HIGHLIGHT_COLOR : 0x00FFFFFF);
	pspDebugScreenPuts(s);
}

int DoMenu()
{
	int nitems = NITEMS-1;
	int i, j;
	int createfile = 0, position = 0;
		
	switch ((position = GetConfiguration(&g_config)))
	{
		case 0x80010002:
			createfile = 1;
		break;	

		case 0x80020001: case 0x80020098:
			return -1;
		break;		
	}

	if (g_config.firmware < 0 || g_config.firmware > 0x03070210)
		g_config.firmware = 0;

	if (devkit > 0x03070210)
		nitems++;

	sceKernelDelayThread(5600000); // Wait for screen to be cleared

	pspDebugScreenInit();
	pspDebugScreenClear();	

	for (i = 0, j = 0; i < NITEMS; i++)
	{
		if (devkit == firmwares[i])
			continue;

		items[j] = strings[i];
		values[j] = firmwares[i];
		j++;
	}

	for (i = 0; i < nitems; i++)
	{
		PrintItem(i, nitems, items[i], (g_config.firmware == values[i]));
		
		if (g_config.firmware == values[i])
		{
			j = i;
		}
	}

	while (1)
	{
		sceCtrlReadBufferPositive(&pad, 1);

		int prevj = j;
		
		if (pad.Buttons & PSP_CTRL_DOWN)
		{
			j = (j+1) % nitems;			
		}
		else if (pad.Buttons & PSP_CTRL_UP)
		{
			if (j == 0)
			{
				j = (nitems-1);
			}
			else
			{
				j--;
			}
		}
		else if (pad.Buttons & PSP_CTRL_CROSS)
		{
			g_config.firmware = values[j];
			SaveConfiguration(createfile, position, &g_config);
			return values[j];
		}

		if (prevj != j)
		{
			PrintItem(prevj, nitems, items[prevj], 0);
			PrintItem(j, nitems, items[j], 1);
			sceKernelDelayThread(200000);
		}
		else
		{
			sceKernelDelayThread(50000);
		}		
	}

	return -1;
}


//////// End of menu and configuration stuff ////////

//////// Savedata stuff /////////

u8 psx_cipher_key[0x10] = 
{
	0xFE, 0xA1, 0x0C, 0x2D, 0xC6, 0x67, 0x89, 0xD1, 
	0x24, 0x68, 0xA1, 0x49, 0xA8, 0x78, 0x55, 0x82
};

int EncryptSD(u8 *buf, int len, u8 *key, u8 *outhash)
{
	pspChnnlsvContext1 ctx1;
	pspChnnlsvContext2 ctx2;
	
	memset(&ctx1, 0, sizeof(ctx1));
	memset(&ctx2, 0, sizeof(ctx2));
	memset(buf, 0, 0x10);
	memset(outhash, 0, 0x10);

	if (sceChnnlsv_driver_ABFDFC8B(&ctx2, 5, 1, buf, key) < 0)
		return -1;
        
	if (sceChnnlsv_driver_E7833020(&ctx1, 5) < 0)
		return -2;
        
	if (sceChnnlsv_driver_F21A1FCA(&ctx1, buf, 0x10) < 0)
		return -3;

	if (sceChnnlsv_driver_850A7FA1(&ctx2, buf+0x10, len) < 0)
		return -4;
	
	if (sceChnnlsv_driver_F21A1FCA(&ctx1, buf+0x10, len) < 0)
                return -5;	
	
	if (sceChnnlsv_driver_21BE78B4(&ctx2) < 0)
		return -6;

	if (sceChnnlsv_driver_C4C494F8(&ctx1, outhash, key) < 0)
		return -7;

	return 0;
}

int HashSFO(u8 *data, int len, int mode, void *outhash)
{
	pspChnnlsvContext1 ctx1;

	memset(&ctx1, 0, sizeof(ctx1));
	memset(outhash, 0, 0x10);

	if (sceChnnlsv_driver_E7833020(&ctx1, mode) < 0)
		return -1;
	
	if (sceChnnlsv_driver_F21A1FCA(&ctx1, data, len) < 0)
		return -2;
	
	if (sceChnnlsv_driver_C4C494F8(&ctx1, outhash, NULL) < 0)
		return -3;

	return 0;
}

char mc_file_old[44], mc_file_new[44], mc_file_sfo[44];
u8 *sd_buf;

int UpdateSfo(char *gamecode, int u, u8 *hash)
{
	int offs;
	u8 hash2[0x10];
	
	sprintf(mc_file_sfo, "ms0:/PSP/SAVEDATA/%s/PARAM.SFO", gamecode);

	if (ReadFile(mc_file_sfo, 0, sd_buf, 4912) != 4912)
		return -1;

	offs = (u == 0) ? 0x570 : 0x590;	
	sprintf((char *)sd_buf+offs, "MEMCARD%d.DAT", u+1);	

	memcpy(sd_buf+offs+0x0D, hash, 0x10);

	memset(sd_buf+0x11D0, 0, 0x10);
	memset(sd_buf+0x1220, 0, 0x10);
	memset(sd_buf+0x11C0, 0, 0x10);

	sd_buf[0x11B0] = 0;

	if (HashSFO(sd_buf, 4912, 6, hash2) < 0)
		return -1;

	memcpy(sd_buf+0x11D0, hash2, 0x10);
	sd_buf[0x11B0] = 0x41;	

	if (HashSFO(sd_buf, 4912, 5, hash2) < 0)
		return -1;

	memcpy(sd_buf+0x1220, hash2, 0x10);

	if (HashSFO(sd_buf, 4912, 1, hash2) < 0)
		return -1;

	memcpy(sd_buf+0x11C0, hash2, 0x10);

	if (WriteFile(mc_file_sfo, sd_buf, 4912) != 4912)
		return -1;

	return 0;
}

void VerifyConvertSaveDataX(char *gamecode, int u)
{
	SceIoStat stat_old, stat_new;
	u64 tick_old, tick_new;
	int convert = 1, remove = 1;
	u8 hash[0x10];
	
	sprintf(mc_file_new, "ms0:/PSP/SAVEDATA/%s/SCEVMC%d.VMP", gamecode, u);

	if (sceIoGetstat(mc_file_new, &stat_new) < 0)
		return;

	sprintf(mc_file_old, "ms0:/PSP/SAVEDATA/%s/MEMCARD%d.DAT", gamecode, u+1);

	if (sceIoGetstat(mc_file_old, &stat_old) >= 0)
	{
		sceRtcGetTick((void *)&stat_old.st_mtime, &tick_old);
		sceRtcGetTick((void *)&stat_new.st_mtime, &tick_new);

		if (sceRtcCompareTick(&tick_new, &tick_old) < 0)
		{
			convert = 0;
		}
	}

	if (convert)
	{
		if (ReadFile(mc_file_new, 0x80, sd_buf+0x30, 131072) != 131072)
		{
			remove = 0;
		}
		else
		{
			memset(sd_buf+0x10, 0, 0x20);
			strcpy((char *)sd_buf+0x10, "POPS.MEM");
			sd_buf[0x18] = 1;
			
			int res = EncryptSD(sd_buf, 131072+0x20, psx_cipher_key, hash);
			if (res < 0)
			{
				remove = 0;
			}
			else
			{
				if (WriteFile(mc_file_old, sd_buf, 131072+0x30) != (131072+0x30))
				{
					remove = 0;
				}
				else
				{
					if (UpdateSfo(gamecode, u, hash) < 0)
					{
						remove = 0;
						sceIoRemove(mc_file_old);
					}
				}
			}
		}
	}

	if (remove)
	{
		sceIoRemove(mc_file_new);
	}
}

void VerifyConvertSaveData(char *gamecode)
{
	SceUID fpl = sceKernelCreateFpl("MC", PSP_MEMORY_PARTITION_KERNEL, 0, 131072+0x30, 1, NULL);

	if (fpl < 0)
		return;

	if (sceKernelAllocateFpl(fpl, (void *)&sd_buf, NULL) < 0)
		goto DELETE_FPL;

	VerifyConvertSaveDataX(gamecode, 0);
	VerifyConvertSaveDataX(gamecode, 1);

	sceKernelFreeFpl(fpl, (void *)sd_buf);

DELETE_FPL:

	sceKernelDeleteFpl(fpl);

}

//////// End of savedata stuff /////////

void PopsLoaderBegin()
{
	SceUID fd;
	u32 header[0x28/4];
	int i, domenu, res;
	
	devkit = sceKernelDevkitVersion();
	strncpy(filename, sceKernelInitFileName(), 128);
	
	fd = sceIoOpen(filename, PSP_O_RDONLY, 0);

	if (fd < 0)
	{
		RebootVSHWithError(fd);
	}
	
	sceIoRead(fd, header, 0x28);
	psar_pos = header[9];

	sceIoLseek(fd, psar_pos, PSP_SEEK_SET);
	sceIoRead(fd, header, 0x28);

	if (memcmp(header, "PSTITLE", 7) == 0)
	{
		iso_position = psar_pos + 0x200;
		new_school = 1;
	}
	else
	{
		iso_position = psar_pos + 0x400;
	}

	sceIoLseek(fd, iso_position, PSP_SEEK_SET);
	sceIoRead(fd, header, 0x28);

	if (header[0] == 0x44475000)
		return;

	pops_firmware = -1;

	if (sctrlSEGetVersion() < 0x1022) // M33-3
	{
		RebootVSHWithError(0x800200e5);
	}

	for (i = 0, domenu = 0; i < 10; i++)
	{
		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_RTRIGGER)
		{
			domenu = 1;
			break;
		}
	}
	
	if (domenu)
	{
		pops_firmware = DoMenu();
	}
	else
	{
		res = GetConfiguration(&g_config);
		if (res == -1 || res == 0x80010002)
		{
			pops_firmware = DoMenu();
		}
		else if (res >= 0)
		{
			pops_firmware = g_config.firmware;
		}
	}

	if (pops_firmware == 0)
		return;
	else if (pops_firmware < 0)
		RebootVSHWithError(pops_firmware);

	if (pops_firmware <= 0x03030010)
	{
		VerifyConvertSaveData(g_config.gamecode);
	}

	sctrlHENRegisterLLEHandler(NidsHandler);

	// 3.70+ have their different own different functions for 0307XXXX and 0308XXXX compiled sdk version. (avoid umd games loading in previous firmwares?)
	int (* SetCompiledSdkVersion)(int) = (void *)sctrlHENFindFunction("sceSystemMemoryManager", "SysMemUserForUser", 0x315AD3A0);
	if (!SetCompiledSdkVersion)
	{
		RebootVSHWithError(0x80000001);
	}

	SetCompiledSdkVersion(0x03080010);
	previous = sctrlHENSetStartModuleHandler(OnModuleStart);	
}

int LoadPopsModule(char *modname)
{
	SceUID moduid;
	char s[64];

	if (pops_firmware <= 0)
	{
		return -1;
	}
	
	if (strcmp(modname, "scePops_Manager") == 0)
	{
		switch (pops_firmware)
		{
			case 0x03000010:
				strcpy(s, "ms0:/seplugins/popsloader/popsman300.prx");
			break;

			case 0x03000110:
				strcpy(s, "ms0:/seplugins/popsloader/popsman301.prx");
			break;

			case 0x03000210:
				strcpy(s, "ms0:/seplugins/popsloader/popsman302.prx");
			break;
		
			case 0x03000310:
				strcpy(s, "ms0:/seplugins/popsloader/popsman303.prx");
			break;
		
			case 0x03010010:
				strcpy(s, "ms0:/seplugins/popsloader/popsman310.prx");
			break;
		
			case 0x03010110:
				strcpy(s, "ms0:/seplugins/popsloader/popsman311.prx");
			break;
		
			case 0x03030010:
				strcpy(s, "ms0:/seplugins/popsloader/popsman330.prx");
			break;
		
			case 0x03040010:
				strcpy(s, "ms0:/seplugins/popsloader/popsman340.prx");
			break;

			case 0x03050110:
				strcpy(s, "ms0:/seplugins/popsloader/popsman351.prx");
			break;

			case 0x03050210:
				strcpy(s, "ms0:/seplugins/popsloader/popsman352.prx");
			break;
			
			case 0x03070110:
				strcpy(s, "ms0:/seplugins/popsloader/popsman371.prx");
			break;

			case 0x03070210:
				strcpy(s, "ms0:/seplugins/popsloader/popsman372.prx");
			break;
		}	

		return sceKernelLoadModule(s, 0, NULL);	
	}
	
	else if (strcmp(modname, "M33PopcornManager") == 0)
	{
		if (pops_firmware > 0x03000310)
			return 0; // Return pseudo success (we'll take care of pops hack)

		// 3.00-3.03 Load idcanager now
		return sceKernelLoadModule("ms0:/seplugins/popsloader/idcanager_old.prx", 0, NULL);
	}

	else if (strcmp(modname, "sceImpose_Driver") == 0)
	{
		if (pops_firmware <= 0x03000310)
		{
			switch (pops_firmware)
			{
				case 0x03000010:
					strcpy(s, "ms0:/seplugins/popsloader/meaudio300.prx");
				break;

				case 0x03000110:
					strcpy(s, "ms0:/seplugins/popsloader/meaudio301.prx");
				break;

				case 0x03000210:
					strcpy(s, "ms0:/seplugins/popsloader/meaudio302.prx");
				break;
			
				case 0x03000310:
					strcpy(s, "ms0:/seplugins/popsloader/meaudio303.prx");
				break;
			}

			moduid = sceKernelLoadModule(s, 0, NULL);
			if (moduid >= 0)
			{
				sceKernelStartModule(moduid, 0, NULL, NULL, NULL);				
			}
		}
	}

	else if (strcmp(modname, "pspvmc_Library") == 0)
	{
		switch (pops_firmware)
		{
			case 0x03040010:
				strcpy(s, "ms0:/seplugins/popsloader/libpspvmc340.prx");
			break;

			case 0x03050110:
				strcpy(s, "ms0:/seplugins/popsloader/libpspvmc351.prx");
			break;

			case 0x03050210:
				strcpy(s, "ms0:/seplugins/popsloader/libpspvmc352.prx");
			break;

			case 0x03070110:
				strcpy(s, "ms0:/seplugins/popsloader/libpspvmc371.prx");
			break;

			case 0x03070210:
				strcpy(s, "ms0:/seplugins/popsloader/libpspvmc372.prx");
			break;

			default: // 3.00-3.30 no libpspvmc
				return 0; 
		}	

		return sceKernelLoadModule(s, 0, NULL);
	}

	else if (strcmp(modname, "scePaf_Module") == 0)
	{
		switch (pops_firmware)
		{
			case 0x03000010: // 3.00
				strcpy(s, "ms0:/seplugins/popsloader/pafmini300.prx");
			break;
		
			case 0x03000110: // 3.01
				strcpy(s, "ms0:/seplugins/popsloader/pafmini301.prx");
			break;
		
			case 0x03000210: // 3.02
				strcpy(s, "ms0:/seplugins/popsloader/pafmini302.prx");
			break;
		
			case 0x03000310: // 3.03
				strcpy(s, "ms0:/seplugins/popsloader/pafmini303.prx");
			break;
		
			case 0x03010010: // 3.10
				strcpy(s, "ms0:/seplugins/popsloader/pafmini310.prx");
			break;
		
			case 0x03010110: // 3.11
				strcpy(s, "ms0:/seplugins/popsloader/pafmini311.prx");
			break;
		
			case 0x03030010: // 3.30
				strcpy(s, "ms0:/seplugins/popsloader/pafmini330.prx");
			break;
		
			case 0x03040010: // 3.40
				strcpy(s, "ms0:/seplugins/popsloader/pafmini340.prx");
			break;

			case 0x03050110: // 3.51
				strcpy(s, "ms0:/seplugins/popsloader/pafmini351.prx");
			break;

			case 0x03050210: // 3.52
				strcpy(s, "ms0:/seplugins/popsloader/pafmini352.prx");
			break;

			case 0x03070110: // 3.71
				strcpy(s, "ms0:/seplugins/popsloader/pafmini371.prx");
			break;

			case 0x03070210: // 3.72
				strcpy(s, "ms0:/seplugins/popsloader/pafmini372.prx");
			break;
		}	

		return sceKernelLoadModule(s, 0, NULL);
	}

	return -1;
}

static void GetElfName(u8 *buf, char **pname)
{
	Elf32_Ehdr *header;
	Elf32_Phdr *phdr;
	
	header = (Elf32_Ehdr *)buf;
	phdr = NULL;
	
	phdr = (Elf32_Phdr *)(header->e_phoff + ((u32)buf));
	u32 modinfo_off = phdr->p_paddr;

	modinfo_off &= 0x7FFFFFFF;
	char *modinfo = (char *)(modinfo_off + ((u32)buf)); 

	*pname = modinfo+4;	
}

static char *GetModuleName(u8 *buf, SceSize insize, char *name)
{
	char *modname = NULL;
	
	if (*(u32 *)buf == 0x464C457F) // ELF)
	{
		GetElfName(buf, &modname);
		strncpy(name, modname, 28);	
	}
	else // ~PSP
	{
		modname = (char *)buf+10;

		if (strlen(modname) <= 1)
		{
			SceSize outsize;
			SceUID pid, uid, uid2 = -1; 
			u8 *out, *out2;
			int compressed = 0, error = 0;

			memlmd_323366CA(buf, insize);

			if (sceKernelGetModel() == PSP_MODEL_STANDARD)
			{
				pid = PSP_MEMORY_PARTITION_USER;
			}
			else
			{
				pid = PSP_MEMORY_PARTITION_UMDCACHE;
			}

			outsize = *(u32 *)&buf[0xB0];
			uid = sceKernelAllocPartitionMemory(pid, "", PSP_SMEM_Low, outsize, NULL);
			if (uid < 0)
			{
				return NULL;
			}

			out = sceKernelGetBlockHeadAddr(uid);	
			memcpy(out, buf, insize);			

			if (*(u32 *)&buf[0x7C] == 2)
			{
				memlmd_1570BAB4(0, 0xbfc00200);				
				error = memlmd_7CF1CD3E(out, insize, &outsize);				
			}
			else
			{			
				error = sceMesgLed_driver_DFF0F308(out, insize, &outsize);	
			}

			if (error)
			{
				RebootVSHWithError(error);
			}

			if (outsize >= 0)
			{
				if (pspIsCompressed(out))
				{
					compressed = 1;
					outsize = *(u32 *)&buf[0x28];

					uid2 = sceKernelAllocPartitionMemory(pid, "", PSP_SMEM_Low, outsize, NULL);
					if (uid2 < 0)
					{
						compressed = 0;
						error = 1;
					}
					else
					{
						out2 = sceKernelGetBlockHeadAddr(uid2);

						outsize = pspDecompress(out, out2, outsize);
						if (outsize <= 0)
						{
							error = 1;
						}
						else
						{
							out = out2;
						}						
					}
				}

				if (!error)
				{
					if (*(u32 *)out == 0x464C457F)
					{
						GetElfName(out, &modname);
						strncpy(name, modname, 28);	
					}
				}

				if (compressed)
				{
					sceKernelFreePartitionMemory(uid2);
				}
			}

			sceKernelFreePartitionMemory(uid);			
		}
		else
		{
			strncpy(name, modname, 28);
		}
	}

	if (modname)
	{
		return name;
	}

	return NULL;
}

int ClearFreeBlock()
{
	int res;

	sceKernelFillFreeBlock(PSP_MEMORY_PARTITION_KERNEL, 0);
	sceKernelFillFreeBlock(PSP_MEMORY_PARTITION_VOLATILE, 0);
	sceKernelVolatileMemUnlock(NULL);
	
	res = sceKernelFillFreeBlock(PSP_MEMORY_PARTITION_USER, 0);

	sceKernelMemoryExtendSize(); // slim only memory begin
	sceKernelFillFreeBlock(PSP_MEMORY_PARTITION_UMDCACHE, 0);
	sceKernelFillFreeBlock(PSP_MEMORY_PARTITION_ME, 0);
	sceKernelMemoryShrinkSize(); // slim only memory end

	memset((void *)0x80010000, 0, 0x4000); // scratchpad
	return res;
}

int SystemBootEnd(InitControl *ctrl)
{
	ctrl->ProcessCallbacks(3);

	sceKernelSetDNAS(NULL);
	sceKernelSetSystemStatus(0x00020000);

	//WriteFile("ms0:/powerlock_count.bin", ctrl->powerlock_count, 4);

	if (*ctrl->powerlock_count != 0)
	{
		if (sceKernelPowerUnlock(0) >= 0)
		{
			*ctrl->powerlock_count = *ctrl->powerlock_count - 1;
		}
	}

	return 0;
}

int init_control(InitControl *ctrl)
{
	BootInfo *bi = ctrl->bootinfo;
	SceKernelLMOption option;
	SceUID mod;
	SceSize size;
	void *buf;
	
	//Kprintf("Init Control\n");

	PopsLoaderBegin();	

	for (; bi->nextmodule < bi->nmodules; bi->nextmodule++)
	{
		SceUID uid;
		BootModuleInfo *bm = &bi->bootmoduleinfo[bi->nextmodule];
		u32 flags = bm->flags;

		if (bm->blockuid != 0)
		{
			sceKernelFreePartitionMemory(bm->blockuid);
		}

		if (flags & BOOT_MODULE_FLAGS_EXECUTABLE)
		{
			memmove(&bootmoduleinfo, bm, sizeof(BootModuleInfo));
			ctrl->FreeUnkParamAndModuleInfo(bi);

			bm = &bootmoduleinfo;

			uid = sceKernelGetChunk(PSP_INIT_CHUNK_VSHPARAM);
			if (uid >= 0)
			{
				SceInitVSHParam *vshparam = sceKernelGetBlockHeadAddr(uid);

				if (vshparam->exitcheck != 0)
				{
					RebootVSHWithError(vshparam->exitcheck);
				}
			}			
			if (bm->flags & BOOT_MODULE_FLAGS_FILE)
			{
				//Kprintf("Loading executable...\n");
				mod = sceKernelLoadModuleForLoadExecVSHMs4((char *)bm->addr, 0, NULL);
			}
			else
			{
				//Kprintf("Cannot handle.\n");
				mod = 0x80020001;
			}

			if (mod < 0)
			{
				RebootVSHWithError(mod);
			}
		}
		else
		{
			option.size = sizeof(SceKernelLMOption);
			option.mpidtext = option.mpiddata = bm->mpid;
			option.position = 0;
			option.access = 1;

			if (flags & BOOT_MODULE_FLAGS_FILE)
			{
				mod = ModuleMgrForKernel_25E1F458((char *)bm->addr, 0, &option);
			}
			else
			{
				char modname[28];
				
				buf = (void *)bm->addr;
				size = bm->entrysize;

				mod = -1;

				if (GetModuleName(buf, size, modname))
				{
					mod = LoadPopsModule(modname);
					//Kprintf("LoadPopsModule %s = 0x%08X\n", modname, mod);
				}

				if (mod == -1)
				{
					mod = ModuleMgrForKernel_EF7A7F02(size, buf, 0, &option);
				}
				
				if (mod >= 0)
				{
					sceKernelMemset32(buf, size, 0);
				}
			}
		}

		if (flags & BOOT_MODULE_FLAGS_EXECUTABLE)
		{
			ClearFreeBlock();
			SystemBootEnd(ctrl);				
		}

		if (mod < 0)
		{
			//Kprintf("LoadModule failed 0x%08X\n", mod);
			sceKernelSleepThread();
		}

		void *argp = NULL;
		SceSize args = 0;
		int status;

		uid = bm->argsuid;
		if (uid != 0 && bm->argsize > 0)
		{
			argp = sceKernelGetBlockHeadAddr(uid);
			args = bm->argsize;
		}

		if (mod != 0)
		{
			mod = sceKernelStartModule(mod, args, argp, &status, NULL);
			if (mod < 0)
			{
				if (flags & BOOT_MODULE_FLAGS_EXECUTABLE)
				{
					RebootVSHWithError(mod);
				}
				else
				{
					//Kprintf("StartModule failed 0x%08X\n", mod);
				}
			}
		}

		if (uid != 0)
		{
			sceKernelFreePartitionMemory(uid);
		}

		if (flags & BOOT_MODULE_FLAGS_EXECUTABLE)
			break;		
	}	

	ctrl->FreeUnkParamAndModuleInfo(bi);
	ctrl->FreeParamsAndBootInfo(bi);	

	ctrl->ProcessCallbacks(4);
	
	return 0;
}

int module_start(SceSize args, void *argp)
{
	if (sceKernelInitMode() == PSP_INIT_MODE_POPS)	
		sctrlHENTakeInitControl(init_control);
	
	return 0;
}



