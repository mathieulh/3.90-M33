#include <pspsdk.h>
#include <pspkernel.h>
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PSP_MODULE_INFO("VshControl", 0x1007, 1, 0);

#define EBOOT_BIN "disc0:/PSP_GAME/SYSDIR/EBOOT.BIN"
#define BOOT_BIN  "disc0:/PSP_GAME/SYSDIR/BOOT.BIN"


void ClearCaches()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}


////////////////////////////////////////////////////////////////

/* Note: the compiler has to be configured in makefile to make 
   sizeof(wchar_t) = 2. */
wchar_t verinfo[] = L"3.03 DH";

void PatchSysconfPlugin(u8 *buf)
{
	u32 addrlow;
	int intr = sceKernelCpuSuspendIntr();
	
	u32 text_addr = (u32)(buf+0xA0);

	memcpy((void *)(text_addr+0x19818), verinfo, sizeof(verinfo));
	addrlow = _lh(text_addr+0xCBD0) & 0xFFFF;
	addrlow -= 0x3808; /* address for ver info */				
				
	// addiu a1, s4, addrlow
	_sw(0x26850000 | addrlow, text_addr+0xCD84);

	sceKernelCpuResumeIntr(intr);	
	ClearCaches();
}

void PatchMsVideoMainPlugin(u8 *buf)
{
	int intr = sceKernelCpuSuspendIntr();
	u32 text_addr = (u32)(buf+0xA0);

	_sh(0xfe00, text_addr+0x33A1C); // Allow play avc 480x272 
	//_sh(0xfe00, text_addr+0x33A84);
	//_sh(0xfe00, text_addr+0x35AA8); // show standard mp4 480x272
	//_sh(0xfe00, text_addr+0x35B1C);
	_sh(0xfe00, text_addr+0x35BD0); // show avc 480x272
	//_sh(0xfe00, text_addr+0x3C57C);
	//_sh(0xfe00, text_addr+0x3C5E0);
	//_sh(0xfe00, text_addr+0x3C648);
	//_sh(0xfe00, text_addr+0x48E6C);
	//_sh(0xfe00, text_addr+0x48FB0);
	//_sh(0xfe00, text_addr+0x4BCC8);

	sceKernelCpuResumeIntr(intr);
	ClearCaches();
}

void PatchGamePlugin(u8 *buf)
{
	u32 text_addr = (u32)(buf+0xA0);
	_sw(0x1000fff9, text_addr+0xB264);					
	ClearCaches();			
}

int OnModuleRelocated(char *modname, u8 *modbuf)
{
	Kprintf("OnPspRelSectionApplied: %s\n", modname);
	
	if (strcmp(modname, "sysconf_plugin_module") == 0)
	{
		PatchSysconfPlugin(modbuf);
	}
						
	else if (strcmp(modname, "msvideo_main_plugin_module") == 0)
	{
		PatchMsVideoMainPlugin(modbuf);
	}

	else if (strcmp(modname, "game_plugin_module") == 0)
	{
		PatchGamePlugin(modbuf);					
	}
	
	return 0;
}

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

/* ELF section header */
typedef struct { 
	u32		sh_name; 
	u32		sh_type; 
	u32		sh_flags; 
	u32		sh_addr; 
	u32		sh_offset; 
	u32		sh_size; 
	u32		sh_link; 
	u32		sh_info; 
	u32		sh_addralign; 
	u32		sh_entsize; 
} __attribute__((packed)) Elf32_Shdr;

int IsAddress(void *addr)
{
	u32 u = (u32)addr;

	if (u >= 0x88000000 && u <= 0x883f0000)
		return 1;

	if (u >= 0x08840000 && u <= 0x09FFFFFFF)
		return 1;

	if (u >= 0x08800000 && u <= 0x0883FFFF)
		return 1;

	if (u >= 0x88800000 && u <= 0x8883FFFF)
		return 1;
	
	return 0;
}

int sceKernelApplyPspRelSection(u32 *a0, void *a1, void *a2, void *a3, void *t0, void *t1);

int sceKernelApplyPspRelSectionPatched(u32 *a0, void *a1, void *a2, void *a3, void *t0, void *t1)
{
	Elf32_Ehdr *buf = (Elf32_Ehdr *)a0[0];
	int res = sceKernelApplyPspRelSection(a0, a1, a2, a3, t0, t1);
	
	if (buf->e_magic == 0x464C457F) // ELF 
	{
		Elf32_Shdr *shdr = (Elf32_Shdr *)(buf->e_phoff + ((u32)buf));
		u32 modinfo_off = shdr->sh_addr;
		char *modinfo = (char *)(modinfo_off + ((u32)buf));
			
		modinfo = (char *)((u32)modinfo | 0x80000000);

		if (IsAddress(modinfo+4))
		{
			OnModuleRelocated(modinfo+4, (u8 *)buf);				
		}		
	}

	return res;
}

void PatchLoadCore()
{
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceLoaderCore");
	u32 text_addr = *(mod+27);

	/* patch applypsprelsection to patch the rest of modules after relocation */
	MAKE_CALL(text_addr+0x427C, sceKernelApplyPspRelSectionPatched);
	MAKE_CALL(text_addr+0x44B0, sceKernelApplyPspRelSectionPatched); 	
}


int module_start(SceSize args, void *argp)
{
	PatchLoadCore();
	ClearCaches();
	
	return 0;
}
