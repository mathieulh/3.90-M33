#include <pspsdk.h>
#include <pspkernel.h>
#include <pspinit.h>
#include <psputilsforkernel.h>
#include <psploadcore.h>
#include <stdio.h>
#include <string.h>

#include <oe_malloc.h>

#include "main.h"
#include "sysmodpatches.h"
#include "malloc.h"
#include "umd9660_driver.h"
#include "systemctrl.h"
#include "systemctrl_se.h"
#include "systemctrl_internal.h"
#include "libraries.h"

PSP_MODULE_INFO("SystemControl", 0x3007, 1, 7);
PSP_MODULE_SDK_VERSION(0x03090010);

#define OE_TAG_KERNEL_371	0xFD8DD20E
#define OE_TAG_USER_371		0x28796DAA

#define OE_TAG_GENERIC_KERNEL	0x55668D96
#define OE_TAG_MS_GAME			0x7316308C
#define OE_TAG_MS_UPDATER		0x3EAD0AEE

static SceUID start_thread;
static SceModule2 *last_module;
static STMOD_HANDLER stmod_handler = NULL;
static KDEC_HANDLER kdec_handler = NULL;
static MDEC_HANDLER mdec_handler = NULL;
static LLE_HANDLER lle_handler = NULL;

static int idle = 0;

static int (* ProbeExec1)(void *buf, u32 *check);
static int (* PartitionCheck)(void *st0, void *check);
static int (* ProbeExec2)(u8 *buf, u32 *check);
static int (* UnsignCheck)(void *buf, int size, int m);
static int (* KDecrypt)(void *buf, int size, int *retSize, int m);
static int (* SetKeys)(int unk, void *buf);
static int (* MDecrypt)(u32 *tag, u8 *keys, u32 code, void *buf, int size, int *retSize, int m, void *unk0, int unk1, int unk2, int unk3, int unk4);
static int (* aLinkLibEntries)(SceLibraryStubTable *imports, SceSize size, int user, int cs);


int sceKernelCheckExecFile(void *buf, int *check);
int sceKernelDcacheWBinvAll();
int sceKernelLoadExecutableObject(char *buf, void *check);
int sceKernelApplyPspRelSection(u32 *a0, void *a1, void *a2, void *a3, void *t0, void *t1);

//static int debug = 0;

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

void ClearCaches()
{
	sceKernelIcacheInvalidateAll();
	sceKernelDcacheWritebackInvalidateAll();	
}

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

int IsStaticElf(void *buf)
{
	Elf32_Ehdr *header = (Elf32_Ehdr *)buf;

	if (header->e_magic == 0x464C457F && header->e_type == 2)
	{
		return 1;
	}

	return 0;
}

int PatchExec2(void *buf, int *check)
{
	int index = check[0x4C/4];

	if (index < 0)
	{
		index += 3;
	}

	u32 addr = (u32)(buf + index);

	if (addr >= 0x88400000 && addr <= 0x88800000)
	{
		return 0;
	}

	check[0x58/4] = ((u32 *)buf)[index / 4] & 0xFFFF;
	return ((u32 *)buf)[index / 4];
}

int PatchExec3(void *buf, int *check, int isPlain, int res)
{
	if (!isPlain)
	{
		return res;
	}

	if ((u32)check[8/4] >= 0x52)
	{
		if (check[0x20/4] == -1);
		{
			if (IsStaticElf(buf))
			{		
				check[0x20/4] = 3;
			}			
		}	
				
		return res;
	}

	if (!(PatchExec2(buf, check) & 0x0000FF00))
	{		
		return res;
	}

	check[0x44/4] = 1;	
	return 0;
}

int PatchExec1(void *buf, int *check)
{
	if (((u32 *)buf)[0] != 0x464C457F) /* ELF */
	{
		return -1;
	}

	if (check[8/4] >= 0x120)
	{
		if (check[8/4] != 0x120 && check[8/4] != 0x141
			&& check[8/4] != 0x142 && check[8/4] != 0x143
			&& check[8/4] != 0x140)
		{
			return -1;
		}
		
		if (check[0x10/4] == 0)
		{
			if (check[0x44/4] != 0) 
			{ 
				check[0x48/4] = 1; 
				return 0; 
			} 
			
			return -1;
		}

		check[0x48/4] = 1;
		check[0x44/4] = 1;
		PatchExec2(buf, check);

		return 0;
	}
	else if ((u32)check[8/4] >= 0x52)
	{
		return -1;
	}	

	if (check[0x44/4] != 0) 
	{ 
		check[0x48/4] = 1; 
		return 0; 
	} 
	
	return -2;
}

int sceKernelCheckExecFilePatched(int *buf, int *check)
{
	int res = PatchExec1(buf, check);

	if (res == 0)
	{		
		return res;
	}

	int isPlain = (((u32 *)buf)[0] == 0x464C457F); /* ELF */

	res = sceKernelCheckExecFile(buf, check);

	return PatchExec3(buf, check, isPlain, res);	
}

int ProbeExec1Patched(void *buf, u32 *check)
{
	int res; 
	u16 attr;
	u16 *modinfo;
	u16 realattr;

	res = ProbeExec1(buf, check);

	if (((u32 *)buf)[0] != 0x464C457F)
		return res;

	modinfo = ((u16 *)buf) + (check[0x4C/4] / 2);

	realattr = *modinfo;
	attr = realattr & 0x1E00;
	
	if (attr != 0)
	{
		u16 attr2 = ((u16 *)check)[0x58/2];
		attr2 &= 0x1e00;

		
		if (attr2 != attr)
		{
			((u16 *)check)[0x58/2] = realattr;
		}
	}

	if (check[0x48/4] == 0)
		check[0x48/4] = 1;	

	return res;
}

u32 buf[256/4];

int PartitionCheckPatched(u32 *st0, u32 *check)
{
	SceUID fd = (SceUID)st0[0x34/4];
	u32 pos;
	u16 attributes;	

	if (fd < 0)
		return PartitionCheck(st0, check);

	pos = sceIoLseek(fd, 0, PSP_SEEK_CUR);

	if (pos < 0)
		return PartitionCheck(st0, check);

	/* rewind to beginning */
	sceIoLseek(fd, 0, PSP_SEEK_SET);
	if (sceIoRead(fd, buf, 256) < 256)
	{
		sceIoLseek(fd, pos, PSP_SEEK_SET);
		return PartitionCheck(st0, check);
	}

	/* go to module info offset */
	if (buf[0] == 0x50425000) /* PBP */
	{
		sceIoLseek(fd, buf[0x20/4], PSP_SEEK_SET);
		sceIoRead(fd, buf, 0x14);

		if (buf[0] != 0x464C457F) /* ELF */
		{
			/* Encrypted module */
			sceIoLseek(fd, pos, PSP_SEEK_SET);
			return PartitionCheck(st0, check);
		}

		sceIoLseek(fd, buf[0x20/4]+check[0x4C/4], PSP_SEEK_SET);
		
		if (!IsStaticElf(buf))
		{
			check[0x10/4] = buf[0x24/4]-buf[0x20/4]; // Allow psar's in decrypted pbp's
		}
	}
	else if (buf[0] == 0x464C457F) /* ELF */
	{
		sceIoLseek(fd, check[0x4C/4], PSP_SEEK_SET);
	}
	else /* encrypted module */
	{
		sceIoLseek(fd, pos, PSP_SEEK_SET);
		return PartitionCheck(st0, check);
	}

	sceIoRead(fd, &attributes, 2);

	if (IsStaticElf(buf))
	{
		check[0x44/4] = 0;		
	}
	else
	{
		if (attributes & 0x1000)
		{
			check[0x44/4] = 1;
		}
		else
		{
			check[0x44/4] = 0;
		}
	}

	sceIoLseek(fd, pos, PSP_SEEK_SET);
	return PartitionCheck(st0, check);	
}

char *GetStrTab(u8 *buf)
{
	Elf32_Ehdr *header = (Elf32_Ehdr *)buf;
	int i;

	if (header->e_magic != 0x464C457F)
		return NULL;

	u8 *pData = buf+header->e_shoff;

	for (i = 0; i < header->e_shnum; i++)
	{
		if (header->e_shstrndx == i)
		{			
			Elf32_Shdr *section = (Elf32_Shdr *)pData;

			if (section->sh_type == 3)
				return (char *)buf+section->sh_offset;
		}

		pData += header->e_shentsize;

	}

	return NULL;
}
int ProbeExec2Patched(u8 *buf, u32 *check)
{
	int res;

	res = ProbeExec2(buf, check);

	if (((u32 *)buf)[0] != 0x464C457F) /* ELF */
		return res;

	if (IsStaticElf(buf))
	{
		// Fake apitype to avoid reject
		if (check[8/4] >= 0x140 && check[8/4] <= 0x144)
			check[8/4] = 0x120;
	}

	if (check[0x4C/4] == 0)
	{
		if (IsStaticElf(buf))
		{
			char *strtab = GetStrTab(buf);
		
			if (strtab)
			{
				Elf32_Ehdr *header = (Elf32_Ehdr *)buf;
				int i;				

				u8 *pData = buf+header->e_shoff;

				for (i = 0; i < header->e_shnum; i++)
				{
					Elf32_Shdr *section = (Elf32_Shdr *)pData;

					if (strcmp(strtab+section->sh_name, ".rodata.sceModuleInfo") == 0)
					{
						check[0x4C/4] = section->sh_offset;
						check[0x58/4] = 0;
					}
				
					pData += header->e_shentsize;
				}
			}
		}
	}	

	return res;
}

LibraryData *FindLib(const char *lib)
{
	int i;

	if (!lib)
		return NULL;

	for (i = 0; i < N_LIBRARIES; i++)
	{
		if (strcmp(lib, libraries[i].name) == 0)
			return &libraries[i];
	}

	return NULL;
}

u32 FindNewNid(LibraryData *lib, u32 nid)
{
	int i;

	for (i = 0; i < lib->nnids; i++)
	{
		if (lib->nids[i].oldnid == nid)
			return lib->nids[i].newnid;
	}

	return 0;
}

u32 sctrlHENFindFunction(const char* szMod, const char* szLib, u32 nid)
{
	struct SceLibraryEntryTable *entry;
	SceModule2 *pMod;
	void *entTab;
	int entLen;

	pMod = sceKernelFindModuleByName(szMod);

	if (!pMod)
	{
		pMod = sceKernelFindModuleByAddress((u32)szMod);
		
		if (!pMod)
			return 0;
	}

	LibraryData *data = FindLib(szLib);
	if (data)
	{
		u32 nnid = FindNewNid(data, nid);
		
		if (nnid)
			nid = nnid;
	}
	
	int i = 0;

	entTab = pMod->ent_top;
	entLen = pMod->ent_size;
	while(i < entLen)
    {
		int count;
		int total;
		unsigned int *vars;

		entry = (struct SceLibraryEntryTable *) (entTab + i);

        if(!szLib || (entry->libname && !strcmp(entry->libname, szLib)))
		{
			total = entry->stubcount + entry->vstubcount;
			vars = entry->entrytable;

			if(entry->stubcount > 0)
			{
				for(count = 0; count < (entry->stubcount+entry->vstubcount); count++)
				{
					if (vars[count] == nid)
						return vars[count+total];					
				}
			}
		}

		i += (entry->len * 4);
	}

	return 0;
}

int aLinkLibEntriesPatched(SceLibraryStubTable *imports, SceSize size)
{
	int i = 0;
	int resolve = 1;
	int *module_sdk_version;
	SceLibraryStubTable *power_driver = NULL;

	module_sdk_version = (int *)sctrlHENFindFunction((void *)imports, NULL, 0x11B97506);
	if (module_sdk_version)
	{
		int high = (*module_sdk_version >> 16);
		if (high == 0x0308 || high == 0x0309)
		{
			resolve = 0;			
		}
	}

	if (resolve)
	{
		int j;
		u32 stubTab = (u32)imports;
	
		while (i < size)
		{
			SceLibraryStubTable *import = (SceLibraryStubTable *)(stubTab + i);
			LibraryData *data = FindLib(import->libname);

			if (strcmp(import->libname, "scePower_driver") == 0)
			{
				power_driver = import;
			}

			if (lle_handler)
			{
				lle_handler(import);
			}

			if (data)
			{		
				for (j = 0; j < import->stubcount; j++)
				{
					u32 nnid = FindNewNid(data, import->nidtable[j]);
					if (nnid)
					{
						import->nidtable[j] = nnid;
					}
				}
			}

			i += (import->len*4);
		}

		ClearCaches();
	}
	
	int res = aLinkLibEntries(imports, size, 0, 0);

	if (power_driver)
	{
		for (i = 0; i < power_driver->stubcount; i++)
		{
			if (power_driver->nidtable[i] == 0x737486F2)
			{
				u32 func = FindPowerFunction(0x737486F2);

				if (func)
				{
					 REDIRECT_FUNCTION((u32)&power_driver->stubtable[i*2], func);
					ClearCaches();
				}
			}
		}
	}

	return res;
}

void sctrlHENRegisterLLEHandler(LLE_HANDLER handler)
{
	lle_handler = handler;
}

SceUID sceKernelCreateThreadPatched(const char *name, SceKernelThreadEntry entry, int initPriority,
                             int stackSize, SceUInt attr, SceKernelThreadOptParam *option)
{
	SceUID thid = sceKernelCreateThread(name, entry, initPriority, stackSize, attr, option);

	if (thid >= 0 && strcmp(name, "SceModmgrStart") == 0)
	{
		start_thread = thid;
		last_module = sceKernelFindModuleByAddress((u32)entry);		
	}

	return thid;
}

int sceKernelStartThreadPatched(SceUID thid, SceSize arglen, void *argp)
{
	if (thid == start_thread)
	{
		start_thread = -1;

		if (last_module && stmod_handler)
		{
			stmod_handler(last_module);
		}
	}

	return sceKernelStartThread(thid, arglen, argp);
}

STMOD_HANDLER sctrlHENSetStartModuleHandler(STMOD_HANDLER handler)
{
	STMOD_HANDLER res = stmod_handler;

	stmod_handler = (STMOD_HANDLER)(0x80000000 | (u32)handler);
	return res;
}

int OnModuleStart(SceModule2 *mod)
{
	char *modname = mod->modname;
	u32 text_addr = mod->text_addr;
	
	if (strcmp(modname, "sceIsofs_driver") == 0)
	{
		if (sceKernelInitApitype() == 0x120)
		{
			PatchIsofsDriver(text_addr);
		}
	}

	else if (strcmp(modname, "sceLowIO_Driver") == 0)
	{

#ifndef PSP_SLIM
		PatchNandDriver(text_addr); 
#endif
		
		if (mallocinit() < 0)
		{
			while (1) _sw(0, 0);
		}
	}

#ifdef PSP_SLIM

	else if (strcmp(modname, "sceUmdCache_driver") == 0)
	{
		PatchUmdCache(text_addr);
	}

#endif

	else if (strcmp(modname, "sceUmdMan_driver") == 0)
	{
		PatchUmdMan(text_addr);
	}

	else if (strcmp(modname, "sceMediaSync") == 0)
	{
		PatchInitLoadExecAndMediaSync(text_addr);
	}

	else if (strcmp(modname, "sceImpose_Driver") == 0)
	{
		OnImposeLoad(text_addr);
	}

	else if (strcmp(modname, "sceWlan_Driver") == 0)
	{
		PatchWlan(text_addr);
	}

	else if (strcmp(modname, "scePower_Service") == 0)
	{
		PatchPower(text_addr);
	}

	if (!idle)
	{
		if (sceKernelGetSystemStatus() == 0x20000)
		{
			idle = 1;
			OnSystemStatusIdle();
		}
	}
	
	return 0;
}

int UnsignCheckPatched(u8 *buf, int size, int m)
{
	int unsigncheck = 0, i;

	if (((u32 *)buf)[0] == 0x5053507E) // ~PSP
	{
		for (i = 0; i < 0x58; i++)
		{
			if (buf[0xD4+i] != 0)
			{
				unsigncheck = 1;
				break;
			}
		}
	}

	if (unsigncheck)
	{
		u32 *buf32 = (u32 *)buf;

		if (buf32[0xB8/4] != 0 || buf32[0xBC/4] != 0)
		{
			return UnsignCheck(buf, size, m);
		}
	}

	return 0;
}


int KDecryptPatched(u32 *buf, int size, int *retSize, int m)
{
	u8 *buf8 = (u8 *)buf;
	
	if (kdec_handler)
	{
		if (kdec_handler(buf, size, retSize, m) >= 0)
			return 0;
	}

	if (buf != NULL && retSize != NULL)
	{	
		u32 oe_tag = buf[0x130/4];
		
		if (oe_tag == OE_TAG_KERNEL_371 || oe_tag == OE_TAG_GENERIC_KERNEL)
		{
			if (buf8[0x150] == 0x1F && buf8[0x151] == 0x8B)
			{			
				*retSize = buf[0xB0/4];				
				memmove(buf, ((u8 *)buf)+0x150, *retSize);				
			
				return 0;
			}
		}
	}

	int res = KDecrypt(buf, size, retSize, m);
	if (res < 0)
	{
		// Enable user modules to load kernel signchecked modules
		if (UnsignCheckPatched(buf8, size, m) >= 0)
		{
			SetKeys(0, (void *)0xbfc00200);
			res = KDecrypt(buf, size, retSize, m);
		}
	}	

	return res;
}

int MDecryptPatched(u32 *tag, u8 *keys, u32 code, u32 *buf, int size, int *retSize, int m, void *unk0, int unk1, int unk2, int unk3, int unk4)
{
	u8 *buf8 = (u8 *)buf;
	
	if (mdec_handler)
	{
		if (mdec_handler(tag, keys, code, buf, size, retSize, m, unk0, unk1, unk2, unk3, unk4) >= 0)
			return 0;
	}	
	
	if (buf != NULL && tag != NULL && retSize != NULL)
	{	
		u32 oe_tag = buf[0x130/4];
		
		if (oe_tag == OE_TAG_USER_371 || oe_tag == OE_TAG_MS_GAME || oe_tag == OE_TAG_MS_UPDATER)
		{
			if (buf8[0x150] == 0x1F && buf8[0x151] == 0x8B)
			{			
				*retSize = buf[0xB0/4];				
				memmove(buf, ((u8 *)buf)+0x150, *retSize);					
			
				return 0;
			}
		}		
	}

	int res = MDecrypt(tag, keys, code, buf, size, retSize, m, unk0, unk1, unk2, unk3, unk4);
	if (res < 0)
	{
		// Enable user modules to load user signchecked modules
		if (UnsignCheckPatched(buf8, size, m) >= 0)
		{
			res = MDecrypt(tag, keys, code, buf, size, retSize, m, unk0, unk1, unk2, unk3, unk4);
		}
	}

	return res;
}

KDEC_HANDLER sctrlHENRegisterKDecryptHandler(KDEC_HANDLER handler)
{
	KDEC_HANDLER res = kdec_handler;	
	kdec_handler = handler;
	return res;
}

MDEC_HANDLER sctrlHENRegisterMDecryptHandler(MDEC_HANDLER handler)
{
	MDEC_HANDLER res = mdec_handler;	
	mdec_handler = handler;
	return res;
}

void PatchSyscall(u32 funcaddr, void *newfunc)
{
	u32 *vectors = (u32 *)0x8802be30;

	int i;

	for (i = 0; i < 0x1000; i++)
	{
		if (vectors[i] == funcaddr)
		{
			vectors[i] = (u32)newfunc;			
		}
	}
} 

/*int WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_CREAT | PSP_O_TRUNC | PSP_O_WRONLY, 0777);

	if (fd >= 0)
	{
		sceIoWrite(fd, buf, size);
		sceIoClose(fd);
		return 1;
	}

	return -1;
}*/

void UndoSuperNoPlainModuleCheckPatch()
{
	
}

void PatchLoadCore()
{
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceLoaderCore");
	u32 text_addr = *(mod+27);
	//int w;

	// ApplyPspRelSection is not called that often anymore in 3.70+
	// The start module thread stuff replaces that now.

	/* patch applypsprelsection to patch the rest of modules after relocation 
	MAKE_CALL(text_addr+0x4EE0, sceKernelApplyPspRelSectionPatched);
	MAKE_CALL(text_addr+0x4FA4, sceKernelApplyPspRelSectionPatched); */
	
	/* Patch calls and references to sceKernelCheckExecFile */
	_sw((u32)sceKernelCheckExecFilePatched, text_addr+0x86F4);
	MAKE_CALL(text_addr+0x1560, sceKernelCheckExecFilePatched);
	MAKE_CALL(text_addr+0x15B0, sceKernelCheckExecFilePatched);
	MAKE_CALL(text_addr+0x49C8, sceKernelCheckExecFilePatched);

	/* Change switch table */
	//w = _lw(text_addr+0x8A28);
	_sw(_lw(text_addr+0x8A0C), text_addr+0x8A28);

	/* Patch 2 functions called by sceKernelProbeExecutableObject */
	ProbeExec1 = (void *)(text_addr+0x61C0);
	ProbeExec2 = (void *)(text_addr+0x60DC);
	MAKE_CALL(text_addr+0x46BC, ProbeExec1Patched);
	MAKE_CALL(text_addr+0x485C, ProbeExec2Patched);

	/* Allow kernel modules to have syscall exports */
	_sw(0x3c080000, text_addr+0x40C8);	
	_sw(0x3c080000, text_addr+0x40B0);	
	
	/* Allow higher firmware modules to load */
	// movz a0, zero, a1 -> mov a0, zero
	_sw(0x00002021, text_addr+0x7CA0);

	// Allow ModuleMgrForKernel to load sdk kernel modules
	_sw(0, text_addr+0x6850);
	_sw(0, text_addr+0x6854);

	// Allow ModuleMgrForKernel to load sdk user modules
	_sw(0, text_addr+0x6960);
	_sw(0, text_addr+0x6964);

	// Nids resolver patch
	MAKE_CALL(text_addr+0x1158, aLinkLibEntriesPatched);
	aLinkLibEntries = (void *)(text_addr+0x33D8);

	/* Patch init start */
	MAKE_CALL(text_addr+0x1D94, PatchInit);
	// li a0, 4 -> mov a0, s7
	_sw(0x02e02021, text_addr+0x1D98);

	/* Restore UnsignCheck && encryption */
	char *m = "sceMemlmd";
	char *l = "memlmd";

	u32 addr;

	addr = FindProc(m, l, 0xC94891BB); // 0x323366CA
	MAKE_CALL(text_addr+0x68E4, addr);
	MAKE_CALL(text_addr+0x6914, addr);
	MAKE_CALL(text_addr+0x69AC, addr);

	addr = FindProc(m, l, 0xA43B8A6C); // 0x7CF1CD3E
	MAKE_CALL(text_addr+0x41BC, addr);
	MAKE_CALL(text_addr+0x68C0, addr);
}

void PatchModuleMgr()
{
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceModuleManager");
	u32 text_addr = *(mod+27);
	
	/* Patch ModuleMgr sceKernelCheckExec calls */
	MAKE_JUMP(text_addr+0x802C, sceKernelCheckExecFilePatched);

	/* NoDeviceCheckPatch  */
	_sw(0, text_addr+0x0760); // sceKernelLoadModule (User)
	_sw(0x24020000, text_addr+0x07BC); // sceKernelLoadModule (User)

	_sw(0, text_addr+0x2BE4); // sceKernelLoadModuleVSH
	_sw(0, text_addr+0x2C3C); // sceKernelLoadModuleVSH
	_sw(0x10000009, text_addr+0x2C68); // sceKernelLoadModuleVSH
	
	_sw(0, text_addr+0x2F90); // sceKernelLoadModule (Kernel)
	_sw(0, text_addr+0x2FE4); // sceKernelLoadModule (Kernel)
	_sw(0x10000010, text_addr+0x3010); // sceKernelLoadModule (Kernel)

	PartitionCheck = (void *)(text_addr+0x790C);
	MAKE_CALL(text_addr+0x5F30, PartitionCheckPatched);
	MAKE_CALL(text_addr+0x62AC, PartitionCheckPatched);

	// Add in some patches from TyRaNiD to allow modules before init
	// to be queried with sceKernelQueryModuleInfo 
	_sw(0, text_addr+0x3F6C); 
	_sw(0, text_addr+0x3FB4);
	_sw(0, text_addr+0x3FCC);

	// New Patches in 3.70+ to replace the ApplyPspRelSection stuff
	MAKE_JUMP(text_addr+0x8194, sceKernelCreateThreadPatched);
	MAKE_JUMP(text_addr+0x81DC, sceKernelStartThreadPatched);	
}

void PatchMemlmd()
{
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceMemlmd");
	u32 text_addr = *(mod+27);

	MAKE_CALL(text_addr+0x1170, UnsignCheckPatched);
	MAKE_CALL(text_addr+0x11c4, UnsignCheckPatched);
	UnsignCheck = (void *)(text_addr+0x0FA8);

	MAKE_CALL(text_addr+0x0EA8, KDecryptPatched);
	MAKE_CALL(text_addr+0x0F0C, KDecryptPatched);
	KDecrypt = (void *)(text_addr+0x0134);

	SetKeys = (void *)(text_addr+0x11F0);
}

void PatchInterruptMgr()
{
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceInterruptManager");
	u32 text_addr = *(mod+27);

	// Allow execution of syscalls in kmode
	_sw(0, text_addr+0x3664);

	// Stop kmem protection regeneration
	_sw(0, text_addr+0x3714);
	_sw(0, text_addr+0x3718);
}

void PatchMesgLed()
{
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceMesgLed");
	u32 text_addr = *(mod+27);

	MAKE_CALL(text_addr+0x1900, MDecryptPatched); // vsh m 0 sceMesgLed_driver_55E4F665
	MAKE_CALL(text_addr+0x1EC4, MDecryptPatched); // user m 0 sceMesgLed_driver_DFF0F308
	MAKE_CALL(text_addr+0x3920, MDecryptPatched); // updater pbp m 0 sceMesgLed_driver_AA59DE09
	MAKE_CALL(text_addr+0x3D6C, MDecryptPatched);// game pbp m 0 sceMesgLed_driver_5FDB29F3 (world tour soccer ADF305F0)
	MDecrypt = (void *)(text_addr+0x00E0);
}

extern int booted150;
extern int ram2, ram8;

int module_start(SceSize args, void *argp)
{	
	PatchLoadCore();
	PatchModuleMgr();

#ifndef IPL_VERSION
	PatchIoFileMgr();
#endif

	PatchMemlmd();
	PatchInterruptMgr();	

	ClearCaches();

	// Register primary stmod_handler
	sctrlHENSetStartModuleHandler(OnModuleStart);
		
	SetUmdFile((char *)0x88fb0000);	
	SetConfig((SEConfig *)0x88fb0050);	
	sctrlSESetBootConfFileIndex(*(int *)0x88fb00c0);	
	
	ram2 = *(int *)0x88fb00c4;
	ram8 = *(int *)0x88fb00c8;
	booted150 = *(int *)0x88fb00cc;

	ClearCaches();	

	return 0;
}

int module_stop(void)
{
	return 0;
}

