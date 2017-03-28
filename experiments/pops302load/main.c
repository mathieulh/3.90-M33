#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <psputilsforkernel.h>
#include <systemctrl.h>

#include <stdio.h>
#include <string.h>


PSP_MODULE_INFO("daxPops302Loader", 0x1007, 1, 1);
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


int ModuleMgrForKernel_EF7A7F02(int size, u8 *modbuf, u32 unk, void *unk2, u32 unk3);
int sceKernelDeflateDecompress(u8 *dest, u32 destSize, const u8 *src, u32 unknown);

void ClearCaches()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}

int ReadFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0777);

	int read = sceIoRead(fd, buf, size);
	sceIoClose(fd);

	return read;
}

APRS_EVENT previous = NULL;
int plain_game = 0;

int IsPlainGame()
{
	u32 header[0x28/4];
	
	SceUID fd = sceIoOpen(sceKernelInitFileName(), PSP_O_RDONLY, 0);

	if (fd < 0)
	{
		Kprintf("Warning cannot open initfilename %08X !\n", fd);
		return 1;
	}

	sceIoRead(fd, header, 0x28);
	sceIoLseek(fd, header[9]+0x0400, PSP_SEEK_SET);
	sceIoRead(fd, header, 0x28);
	sceIoClose(fd);

	if (header[0] == 0x44475000)
		return 0;

	return 1;
}

int sceKernelLoadModulePatched(const char *path, int flags, SceKernelLMOption *option)
{
	Kprintf("Loading pops.prx...\n");
	Kprintf("find module: %08X\n", sceKernelFindModuleByName("daxPopcornManager"));
	return sceKernelLoadModule("ms0:/seplugins/pops302/pops.prx", flags, option);
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

int scePopsMan_6768B22F_Patched(u8 *buf)
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

	_sw(pgd_off, text_addr+0x12F0);
	_sw(0, text_addr+0x12F4);
	//_sw(pgd_off, text_addr+0x13A0); // 3.03
	//_sw(0, text_addr+0x13A4);

	sceIoClose(fd);

	pspSdkSetK1(k1);
	return 0;	
}

int scePopsMan_0090B2C8_Patched(u8 *dest, u32 destSize, const u8 *src, u32 unknown)
{
	int k1 = pspSdkSetK1(0);
	u32 code = (u32)dest;
	int res;

	if (code == 0x80000004)
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

	Kprintf("open encrypted %s\n", path);
		
	if (fd < 0)
		return fd;

	sceIoLseek(fd, offset, PSP_SEEK_SET);
	sceIoRead(fd, &signature, 4);

	if (signature != 0x44475000)
	{
		sceIoLseek(fd, offset, PSP_SEEK_SET);
		Kprintf("Returning %08X\n", fd);
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

int OnModuleRelocated(char *modname, u8 *modbuf)
{
	u32 text_addr;

	if (strcmp(modname, "scePops_Manager") == 0)
	{
		text_addr = (u32)(modbuf+0xA0);

		plain_game = IsPlainGame();
		MAKE_CALL(text_addr+0x98, sceKernelLoadModulePatched);	

		Kprintf("plain game %d\n", plain_game);
		
		if (plain_game)
		{
			_sw(0x00001021, text_addr+0x50);
			REDIRECT_FUNCTION(text_addr+0x4A8, scePopsMan_6768B22F_Patched);
			REDIRECT_FUNCTION(text_addr+0x144, scePopsMan_0090B2C8_Patched);
			REDIRECT_FUNCTION(text_addr+0xA44, OpenEncryptedPatched);
			/*_sw(0x00001021, text_addr+0x50); 3.03 
			REDIRECT_FUNCTION(text_addr+0x53C, scePopsMan_6768B22F_Patched);
			REDIRECT_FUNCTION(text_addr+0x144, scePopsMan_0090B2C8_Patched);
			REDIRECT_FUNCTION(text_addr+0xAD8, OpenEncryptedPatched); */
		}

		ClearCaches();
	}
	else if (strcmp(modname, "pops") == 0)
	{
		if (plain_game)
		{
			text_addr = (u32)(modbuf+0xC0);
		
			// Patch hash check
			_sw(0, text_addr+0x12358);

			// Use our decompression function
			u32 x = _lw(text_addr+0x5E8);
			_sw(x, text_addr+0x12314);
			ClearCaches();
		}
	}
	
	if (!previous)	
		return 0;

	return previous(modname, modbuf);
}

int ModuleMgrForKernel_EF7A7F02_Patched(int size, u8 *modbuf, u32 unk, void *unk2, u32 unk3)
{
	Elf32_Ehdr *header = (Elf32_Ehdr *)modbuf;
	Elf32_Phdr *phdr = NULL;
	char *modname;
	
	if (header->e_magic == 0x464C457F)
	{
		phdr = (Elf32_Phdr *)(header->e_phoff + ((u32)modbuf));
		u32 modinfo_off = phdr->p_paddr;

		modinfo_off &= 0x7FFFFFFF;
		char *modinfo = (char *)(modinfo_off + ((u32)modbuf)); 

		modname = modinfo+4;
	}
	else // ~PSP
	{
		modname = (char *)modbuf+10;
	}

	Kprintf("pspbtcnf* module loaded: %s\n", modname);

	if (strcmp(modname, "scePops_Manager") == 0)
	{
		memset(modbuf, 0, size);
		size = ReadFile("ms0:/seplugins/pops302/popsman.prx", modbuf, 30000);
		Kprintf("Read popsman: %d\n", size);
	}
	else if (strcmp(modname, "sceMeAudio") == 0)
	{
		memset(modbuf, 0, size);
		size = ReadFile("ms0:/seplugins/pops302/meaudio.prx", modbuf, 30000);
		Kprintf("Read meaudio: %d\n", size);
	}
	else if (strcmp(modname, "sceMesgLed") == 0)
	{
		memset(modbuf, 0, size);
		size = ReadFile("ms0:/seplugins/pops302/mesg_led.prx", modbuf, 100000);
		Kprintf("Read mesg_led: %d\n", size);
	}
	else if (strcmp(modname, "daxPopcornManager") == 0)
	{
		// Hack my own module :D
		// Make popcorn exit inmediately
		u32 entry = phdr->p_offset + header->e_entry + (u32)modbuf;

		Kprintf("Entry: 0x%08X\n", entry);
		_sw(0x03E00008, entry); // jr ra
		_sw(0x24020001, entry+4); // li v0, 1
		ClearCaches();
	}
	
	return ModuleMgrForKernel_EF7A7F02(size, modbuf, unk, unk2, unk3);
}

int module_start(SceSize args, void *argp)
{
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceInit");
	u32 text_addr = *(mod+27);

	MAKE_CALL(text_addr+0x938, ModuleMgrForKernel_EF7A7F02_Patched);
	ClearCaches();

	previous = sctrlHENSetOnApplyPspRelSectionEvent(OnModuleRelocated);
	
	return 0;
}

int module_stop(void)
{
	return 0;
}

