#include <pspsdk.h>
#include <pspkernel.h>
#include <pspinit.h>
#include <psputilsforkernel.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "sysmodpatches.h"
#include "malloc.h"
#include "umd9660_driver.h"
#include "systemctrl.h"
#include "systemctrl_se.h"

PSP_MODULE_INFO("SystemControl", 0x3007, 1, 4);
PSP_MAIN_THREAD_ATTR(0);


int (* ProbeExec1)(void *buf, u32 *check);
int (* PartitionCheck)(void *st0, void *check);
int (* ProbeExec2)(u8 *buf, u32 *check);
//int (* sceMesgLed_driver_DFF0F308)(u8 *buf, int size, int x) = (void *)0x8801c718;
//int (* sceMesgLed_driver_55E4F665)(u8 *buf, int size, int x) = (void *)0x8801c6f0;
int (* FlashfatIoOpen)(PspIoDrvFileArg *arg, char *file, int flags, SceMode mode); 
int (* FlashfatIoRemove)(PspIoDrvFileArg *arg, const char *name); 
int (* FlashfatIoMkdir)(PspIoDrvFileArg *arg, const char *name, SceMode mode); 
int (* FlashfatIoRmdir)(PspIoDrvFileArg *arg, const char *name);
int (* FlashfatIoDopen)(PspIoDrvFileArg *arg, const char *dirname);
int (* FlashfatIoGetstat)(PspIoDrvFileArg *arg, const char *file, SceIoStat *stat);
int (* FlashfatIoChstat)(PspIoDrvFileArg *arg, const char *file, SceIoStat *stat, int bits);
int (* FlashfatIoRename)(PspIoDrvFileArg *arg, const char *oldname, const char *newname); 
int (* FlashfatIoChdir)(PspIoDrvFileArg *arg, const char *dir); 

int sceKernelCheckExecFile(void *buf, int *check);
int sceKernelDcacheWBinvAll();
int sceKernelLoadExecutableObject(char *buf, void *check);
int sceKernelApplyPspRelSection(u32 *a0, void *a1, void *a2, void *a3, void *t0, void *t1);

int debug = 0;

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

void ClearCaches()
{
	sceKernelIcacheInvalidateAll();
	sceKernelDcacheWritebackInvalidateAll();	
}

u32 sctrlHENFindFunction(const char* szMod, const char* szLib, u32 nid)
{
	struct SceLibraryEntryTable *entry;
	SceModule *pMod;
	void *entTab;
	int entLen;

	pMod = sceKernelFindModuleByName(szMod);

	if (!pMod)
	{
		Kprintf("Cannot find module %s\n", szMod);
		return 0;
	}
	
	int i = 0;

	entTab = pMod->ent_top;
	entLen = pMod->ent_size;
	//***printf("entTab %p - entLen %d\n", entTab, entLen);
	while(i < entLen)
    {
		int count;
		int total;
		unsigned int *vars;

		entry = (struct SceLibraryEntryTable *) (entTab + i);

        if(entry->libname && !strcmp(entry->libname, szLib))
		{
			total = entry->stubcount + entry->vstubcount;
			vars = entry->entrytable;

			if(entry->stubcount > 0)
			{
				for(count = 0; count < entry->stubcount; count++)
				{
					if (vars[count] == nid)
						return vars[count+total];					
				}
			}
		}

		i += (entry->len * 4);
	}

	//***printf("Funtion not found.\n");
	return 0;
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
		/*if (check[0x1C/4] != 0)
		{
			if (IsAddress(check[0x1C/4]))			
				DoModulePatches((void *)check[0x1C/4]);
		}*/		

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

APRS_EVENT onpsprelsection = NULL;

APRS_EVENT sctrlHENSetOnApplyPspRelSectionEvent(APRS_EVENT func)
{
	APRS_EVENT res = onpsprelsection;

	Kprintf("%08X\n", (u32)func);
	
	onpsprelsection = (APRS_EVENT)((u32)func | 0x80000000);
	return res;
}

int sceKernelApplyPspRelSectionPatched(u32 *a0, void *a1, void *a2, void *a3, void *t0, void *t1)
{
	Elf32_Ehdr *buf = (Elf32_Ehdr *)a0[0];
	int res = sceKernelApplyPspRelSection(a0, a1, a2, a3, t0, t1);
	
	if (buf->e_magic == 0x464C457F) // ELF 
	{
		if (!IsStaticElf(buf))
		{
			Elf32_Shdr *shdr = (Elf32_Shdr *)(buf->e_phoff + ((u32)buf));
			u32 modinfo_off = shdr->sh_addr;
			char *modinfo = (char *)(modinfo_off + ((u32)buf));
			
			modinfo = (char *)((u32)modinfo | 0x80000000);

			if (IsAddress(modinfo+4))
			{
				if (strcmp(modinfo+4, "sceIsofs_driver") == 0)
				{
					if (sceKernelInitApitype() == 0x120)
					{
						PatchIsofsDriver((char *)buf);
					}
				}

				else if (strcmp(modinfo+4, "sceNAND_Driver") == 0)
				{
					PatchNandDriver((char *)buf);
				}				

				else if (strcmp(modinfo+4, "sceUmdMan_driver") == 0)
				{
					PatchUmdMan((char *)buf);
				}

				else if (strcmp(modinfo+4, "sceMediaSync") == 0)
				{
					PatchInitLoadExecAndMediaSync((char *)buf);
				}

				else if (strcmp(modinfo+4, "sceImpose_Driver") == 0)
				{
					OnImposeLoad();
				}
				else if (strcmp(modinfo+4, "sceWlan_Driver") == 0)
				{
					PatchWlan((char *)buf);
				}
				else if (strcmp(modinfo+4, "scePower_Service") == 0)
				{
					PatchPower((char *)buf);
				}

				if (onpsprelsection)
				{
					onpsprelsection(modinfo+4, (u8 *)buf);
				}
			}		
		}
	}

	return res;
}



char g_file[256];

char *GetNewPath(char *file)
{
	if (strcmp(file, "/kd") == 0 || strcmp(file, "/KD") == 0)
	{
		strcpy(g_file, file);
		g_file[2] = 'n';
		return g_file;
	}

	if (strcmp(file, "/vsh/module") == 0 || strcmp(file, "/VSH/MODULE") == 0)
	{
		strcpy(g_file, file);
		g_file[5] = 'n';
		return g_file;
	}

	if (strncmp(file, "/kd/", 4) == 0 || strncmp(file, "/KD/", 4) == 0)
	{
		strcpy(g_file, file);
		g_file[2] = 'n';
		return g_file;
	}

	if (strncmp(file, "/vsh/module/", 12) == 0 || strncmp(file, "/VSH/MODULE/", 12) == 0)
	{
		strcpy(g_file, file);
		g_file[5] = 'n';
		return g_file;
	}	

	return file;
}

int FlashfatIoOpen_Patched(PspIoDrvFileArg *arg, char *file, int flags, SceMode mode)
{
	if (arg->fs_num == 2)
		arg->fs_num = 1;
	
	if (file)
	{
		return FlashfatIoOpen(arg, GetNewPath(file), flags, mode);
	}

	return FlashfatIoOpen(arg, file, flags, mode);
}

int FlashfatIoRemove_Patched(PspIoDrvFileArg *arg, const char *name)
{
	if (arg->fs_num == 2)
		arg->fs_num = 1;
	
	if (name)
	{
		return FlashfatIoRemove(arg, GetNewPath((char *)name));
	}

	return FlashfatIoRemove(arg, name);
}

int FlashfatIoMkdir_Patched(PspIoDrvFileArg *arg, const char *name, SceMode mode)
{
	if (arg->fs_num == 2)
		arg->fs_num = 1;

	if (name)
	{
		return FlashfatIoMkdir(arg, GetNewPath((char *)name), mode);
	}

	return FlashfatIoMkdir(arg, name, mode);
}

int FlashfatIoRmdir_Patched(PspIoDrvFileArg *arg, const char *name)
{
	if (arg->fs_num == 2)
		arg->fs_num = 1;

	if (name)
	{
		return FlashfatIoRmdir(arg, GetNewPath((char *)name));
	}

	return FlashfatIoRmdir(arg, name);
}

int FlashfatIoDopen_Patched(PspIoDrvFileArg *arg, const char *dirname)
{
	if (arg->fs_num == 2)
		arg->fs_num = 1;

	if (dirname)
	{
		return FlashfatIoDopen(arg, GetNewPath((char *)dirname));
	}

	return FlashfatIoDopen(arg, dirname);
}

int FlashfatIoGetstat_Patched(PspIoDrvFileArg *arg, const char *file, SceIoStat *stat)
{
	if (arg->fs_num == 2)
		arg->fs_num = 1;
	
	if (file)
	{
		return FlashfatIoGetstat(arg, GetNewPath((char *)file), stat);
	}

	return FlashfatIoGetstat(arg, file, stat);
}

int FlashfatIoChstat_Patched(PspIoDrvFileArg *arg, const char *file, SceIoStat *stat, int bits)
{
	if (arg->fs_num == 2)
		arg->fs_num = 1;

	if (file)
	{
		return FlashfatIoChstat(arg, GetNewPath((char *)file), stat, bits);
	}

	return FlashfatIoChstat(arg, file, stat, bits);
}

int FlashfatIoRename_Patched(PspIoDrvFileArg *arg, const char *oldname, const char *newname)
{
	if (arg->fs_num == 2)
		arg->fs_num = 1;

	if (oldname)
	{
		if (newname)
		{
			return FlashfatIoRename(arg, GetNewPath((char *)oldname), GetNewPath((char *)newname));
		}

		return FlashfatIoRename(arg, GetNewPath((char *)oldname), newname);
	}

	if (newname)
		return FlashfatIoRename(arg, oldname, GetNewPath((char *)newname));

	return FlashfatIoRename(arg, oldname, newname);
}

int FlashfatIoChdir_Patched(PspIoDrvFileArg *arg, const char *dir)
{
	if (arg->fs_num == 2)
		arg->fs_num = 1;

	if (dir)
	{
		return FlashfatIoChdir(arg, GetNewPath((char *)dir));
	}

	return FlashfatIoChdir(arg, dir);
}

int sceIoAddDrvPatched(PspIoDrv *drv)
{
	if (drv->name)
	{
		if (strcmp(drv->name, "flashfat") == 0)
		{
			FlashfatIoOpen = drv->funcs->IoOpen;
			FlashfatIoRemove = drv->funcs->IoRemove;
			FlashfatIoMkdir = drv->funcs->IoMkdir;
			FlashfatIoRmdir = drv->funcs->IoRmdir;
			FlashfatIoDopen = drv->funcs->IoDopen;
			FlashfatIoGetstat = drv->funcs->IoGetstat;
			FlashfatIoChstat = drv->funcs->IoChstat;
			FlashfatIoRename = drv->funcs->IoRename;
			FlashfatIoChdir = drv->funcs->IoChdir;
			drv->funcs->IoOpen = FlashfatIoOpen_Patched;
			drv->funcs->IoRemove = FlashfatIoRemove_Patched;
			drv->funcs->IoMkdir = FlashfatIoMkdir_Patched;
			drv->funcs->IoRmdir = FlashfatIoRmdir_Patched;
			drv->funcs->IoDopen = FlashfatIoDopen_Patched;
			drv->funcs->IoGetstat = FlashfatIoGetstat_Patched;
			drv->funcs->IoChstat = FlashfatIoChstat_Patched;
			drv->funcs->IoRename = FlashfatIoRename_Patched;
			drv->funcs->IoChdir = FlashfatIoChdir_Patched;
		}		
	}
	
	return sceIoAddDrv(drv);
}

void PatchSyscall(u32 funcaddr, void *newfunc)
{
	u32 *vectors = (u32 *)0x8802b830;

	int i;

	for (i = 0; i < 0x1000; i++)
	{
		if (vectors[i] == funcaddr)
		{
			vectors[i] = (u32)newfunc;
		}
	}
} 

int WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_CREAT | PSP_O_TRUNC | PSP_O_WRONLY, 0777);

	if (fd >= 0)
	{
		sceIoWrite(fd, buf, size);
		sceIoClose(fd);
		return 1;
	}

	return -1;
}


int w;

void UndoSuperNoPlainModuleCheckPatch()
{
	/*_sw((u32)sceKernelCheckExecFile, 0x8801c9ac);
	MAKE_CALL(0x88017EC4, sceKernelCheckExecFile);
	MAKE_CALL(0x88017F14, sceKernelCheckExecFile);
	MAKE_CALL(0x88019CC0, sceKernelCheckExecFile);
	_sw(w, 0x8801D05C);

	MAKE_CALL(0x88019b50, ProbeExec1);
	MAKE_CALL(0x880199b0, ProbeExec2);
	_sw(0x11c0ffb2, 0x88019ac0);	

	MAKE_JUMP(0x8805FFB4, sceKernelCheckExecFile);
	MAKE_CALL(0x8805da24, PartitionCheck);

	ClearCaches();*/
}

void PatchLoadCore()
{
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceLoaderCore");
	u32 text_addr = *(mod+27);

	/* patch applypsprelsection to patch the rest of modules after relocation */
	MAKE_CALL(text_addr+0x4964, sceKernelApplyPspRelSectionPatched);
	MAKE_CALL(text_addr+0x4B98, sceKernelApplyPspRelSectionPatched); 
	
	/* Patch calls and references to sceKernelCheckExecFile */
	_sw((u32)sceKernelCheckExecFilePatched, text_addr+0x7610);
	MAKE_CALL(text_addr+0x1554, sceKernelCheckExecFilePatched);
	MAKE_CALL(text_addr+0x15A4, sceKernelCheckExecFilePatched);
	MAKE_CALL(text_addr+0x4268, sceKernelCheckExecFilePatched);
	w = _lw(text_addr+0x7D2C);
	_sw(_lw(text_addr+0x7D10), text_addr+0x7D2C);

	/* Patch 2 functions called by sceKernelProbeExecutableObject */
	ProbeExec1 = (void *)(text_addr+0x5530);
	ProbeExec2 = (void *)(text_addr+0x544C);
	MAKE_CALL(text_addr+0x3F70, ProbeExec1Patched);
	MAKE_CALL(text_addr+0x4108, ProbeExec2Patched);

	/* some sceKernelProbeExecutableObject patch (elf) 
	   Not necesary: apitype faked in ProbeExec2Patched */
	// (3.03)
	//_sw(0x1120fff3, text_addr+0x39A8);

	/* Allow kernel modules to have syscall exports */
	_sw(0x3c080000, text_addr+0x3A64);		
}

void PatchModuleMgr()
{
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceModuleManager");
	u32 text_addr = *(mod+27);
	
	/* Patch ModuleMgr sceKernelCheckExec calls */
	MAKE_JUMP(text_addr+0x6FB4, sceKernelCheckExecFilePatched);

	/* NoDeviceCehckPatch (this time  has to be done differently) */
	_sw(0, text_addr+0x219C); // sceKernelLoadModule (Kernel)
	_sw(0, text_addr+0x21F0); // sceKernelLoadModule (Kernel)
	_sw(0x10000009, text_addr+0x221C); // sceKernelLoadModule (Kernel)
	_sw(0, text_addr+0x07D4); // sceKernelLoadModule (User)
	_sw(0x24020000, text_addr+0x0830); // sceKernelLoadModule (User)
	_sw(0, text_addr+0x1F90); // sceKernelLoadModuleVSH
	_sw(0, text_addr+0x1FE8); // sceKernelLoadModuleVSH
	_sw(0x10000009, text_addr+0x2014); // sceKernelLoadModuleVSH

	PartitionCheck = (void *)(text_addr+0x6850);
	MAKE_CALL(text_addr+0x4984, PartitionCheckPatched);

	/* QueryModuleInfo Patch */
	_sw(0, text_addr+0x2F18);
	_sw(0, text_addr+0x2F4C);
	_sw(0, text_addr+0x2F64);	
}

void PatchIoFileMgr()
{
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceIOFileManager");
	u32 text_addr = *(mod+27);
	
	/* Patch IoFileMgr sceIoAddDrv nid */
	_sw((u32)sceIoAddDrvPatched, text_addr+0x4E4C);
}

void PatchInterruptMgr()
{
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceInterruptManager");
	u32 text_addr = *(mod+27);

	// Allow execution of syscalls in kmode
	_sw(0, text_addr+0x37E4);
}

int module_start(SceSize args, void *argp)
{	
	int intr = sceKernelCpuSuspendIntr();
	PatchLoadCore();
	PatchModuleMgr();
	PatchIoFileMgr();
	PatchInterruptMgr();	
	sceKernelCpuResumeIntr(intr);
	
	ClearCaches();
		
	SetUmdFile((char *)0x88fb0000);	
	SetConfig((SEConfig *)0x88fb0050);		

	ClearCaches();	

	return 0;
}

int module_stop(void)
{
	return 0;
}

