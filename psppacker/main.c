#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <zlib.h>

#define u8	unsigned char
#define u16 unsigned short
#define u32	unsigned int

#define OE_TAG_GENERIC_KERNEL 0x55668D96

#define OE_TAG_KERNEL_351	0xF5D02F46
#define OE_TAG_USER_351		0x2DA8CEA7
#define OE_TAG_REBOOT_351	0xD30DBE7A

#define OE_TAG_KERNEL_352	0x713AD633
#define OE_TAG_USER_352		0x69ED7AC3	
#define OE_TAG_REBOOT_352	0x126DBEBB

#define OE_TAG_KERNEL_352_IPL	0x3FA947BD
#define OE_TAG_USER_352_IPL		0xB920885E
#define OE_TAG_REBOOT_352_IPL	0x8F113918

#define OE_TAG_KERNEL_360	0x6670939B
#define OE_TAG_USER_360		0x36532303
#define OE_TAG_REBOOT_360	0x75710884

#define OE_TAG_KERNEL_371		0xFD8DD20E
#define OE_TAG_USER_371			0x28796DAA
#define OE_TAG_REBOOT_371		0xB178738A
#define OE_TAG_REBOOT_SLIM_371	0x52C5FCF0

#define OE_TAG_MS_GAME			0x7316308C
#define OE_TAG_MS_UPDATER		0x3EAD0AEE

#define IPL_FLAG		1
#define SLIM_FLAG		2
#define FIRMWARE_FLAG	4
#define UPDATER_FLAG	8
#define SDK_FLAG		16
#define SCE_HEADER_FLAG	32
#define PLUGIN_FLAG		64
#define PBP_GAME_FLAG	128

u8 sce_header[64] = 
{
	0x7E, 0x53, 0x43, 0x45, 0x40, 0x00, 0x00, 0x00, 0x5C, 0x79, 0x72, 0x3D, 0x6B, 0x68, 0x5A, 0x30, 
	0x5C, 0x7D, 0x34, 0x67, 0x57, 0x59, 0x34, 0x78, 0x79, 0x8A, 0x4E, 0x3D, 0x47, 0x4B, 0x44, 0x44, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

typedef struct
{
	u32		signature;  // 0
	u16		attribute; // 4  modinfo
	u16		comp_attribute; // 6
	u8		module_ver_lo;	// 8
	u8		module_ver_hi;	// 9
	char	modname[28]; // 0A
	u8		version; // 26
	u8		nsegments; // 27
	int		elf_size; // 28
	int		psp_size; // 2C
	u32		entry;	// 30
	u32		modinfo_offset; // 34
	int		bss_size; // 38
	u16		seg_align[4]; // 3C
	u32		seg_address[4]; // 44
	int		seg_size[4]; // 54
	u32		reserved[5]; // 64
	u32		devkitversion; // 78
	u32		decrypt_mode; // 7C 
	u8		key_data0[0x30]; // 80
	int		comp_size; // B0
	int		_80;	// B4
	int		reserved2[2];	// B8
	u8		key_data1[0x10]; // C0
	u32		tag; // D0
	u8		scheck[0x58]; // D4
	u32		key_data2; // 12C
	u32		oe_tag; // 130
	u8		key_data3[0x1C]; // 134
} __attribute__((packed)) PSP_Header;

typedef struct 
{ 
	u32 e_magic;
	u8	e_class;
	u8	e_data;
	u8	e_idver;
	u8	e_pad[9];
	u16 e_type; 
    u16 e_machine; 
    u32 e_version; 
    u32 e_entry; 
    u32 e_phoff; 
    u32 e_shoff; 
    u32 e_flags; 
    u16 e_ehsize; 
    u16 e_phentsize; 
    u16 e_phnum; 
    u16 e_shentsize; 
    u16 e_shnum; 
    u16 e_shstrndx; 
} __attribute__((packed)) Elf32_Ehdr;

typedef struct 
{ 
	u32 p_type; 
	u32 p_offset; 
	u32	p_vaddr; 
	u32	p_paddr; 
    u32	p_filesz; 
    u32	p_memsz; 
    u32	p_flags; 
    u32 p_align; 
} __attribute__((packed)) Elf32_Phdr;

typedef struct 
{ 
	u32 sh_name; 
	u32 sh_type; 
	u32 sh_flags; 
	u32 sh_addr; 
	u32 sh_offset; 
	u32 sh_size; 
	u32 sh_link; 
	u32 sh_info; 
	u32 sh_addralign; 
	u32 sh_entsize; 
} __attribute__((packed)) Elf32_Shdr;

typedef struct 
{
	u16		attribute;
	u8		module_ver_lo;	
	u8		module_ver_hi;
	char	modname[28];
} __attribute__((packed)) PspModuleInfo;

int ReadFile(char *file, void *buf, int size)
{
	int fd = open(file, O_RDONLY);
	
	if (fd < 0)
		return fd;

	int rd = read(fd, buf, size);
	close(fd);

	return rd;
}

int WriteFile(char *file, void *buf, int size)
{
	int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC);
	
	if (fd < 0)
		return fd;	

	int written = write(fd, buf, size);
	close(fd);
	chmod(file, 0755);

	return written;
}

int PspPack(u8 *in, int size, u8 *out, int mode, int firmware)
{
	PSP_Header header;
	Elf32_Ehdr *elf_header;
	Elf32_Phdr *segments;
	Elf32_Shdr *sections;
	char *strtab;
	PspModuleInfo *modinfo;
	int i, oe_tag_k = 0, oe_tag_u = 0, oe_tag_r = 0;
	int kernel_tag = 0, user_tag = 0, vsh_tag = 0;

	if (mode & FIRMWARE_FLAG)
	{
		if (!(mode & IPL_FLAG))
		{
			switch(firmware)
			{
				case 100:
					oe_tag_k = OE_TAG_GENERIC_KERNEL;
					kernel_tag = 0;
					user_tag = 0x02000000;
					vsh_tag = 0x03000000;
					firmware = 0x01000300;
				break;

				case 200:
					oe_tag_k = OE_TAG_GENERIC_KERNEL;
					kernel_tag = 0x4467415D;
					firmware = 0x02000010;
				break;
			
				case 351:
					oe_tag_k = OE_TAG_KERNEL_351;
					oe_tag_u = OE_TAG_USER_351;
					oe_tag_r = OE_TAG_REBOOT_351;
					kernel_tag = 0x4C940BF0;
					vsh_tag = 0x38020AF0;
					user_tag = 0x457B0AF0;
					firmware = 0x03050110;				
				break;

				case 352:
					oe_tag_k = OE_TAG_KERNEL_352;
					oe_tag_u = OE_TAG_USER_352;
					oe_tag_r = OE_TAG_REBOOT_352;
					kernel_tag = 0x4C940BF0;
					vsh_tag = 0x38020AF0;
					user_tag = 0x457B0AF0;
					firmware = 0x03050210;
				break;	
				
				case 360:
					oe_tag_k = OE_TAG_KERNEL_360;
					oe_tag_u = OE_TAG_USER_360;
					oe_tag_r = OE_TAG_REBOOT_360;
					kernel_tag = 0x4C940DF0;
					vsh_tag = 0x38020AF0;
					user_tag = 0x457B0AF0;
					firmware = 0x03060010;
				break;

				case 371: case 380: case 390:
					switch (firmware)
					{
						case 371:
							firmware = 0x03070110;
						break;

						case 380:
							firmware = 0x03080010;
						break;

						case 390:
							firmware = 0x03090010;
						break;
					}
				break;

				default:
					printf("Invalid firmware.\n");
					return -1;
			}
		}
		else
		{
			switch (firmware)
			{
				case 352:
					oe_tag_k = OE_TAG_KERNEL_352_IPL;
					oe_tag_u = OE_TAG_USER_352_IPL;
					oe_tag_r = OE_TAG_REBOOT_352_IPL;
					kernel_tag = 0x4C940BF0;
					vsh_tag = 0x38020AF0;
					user_tag = 0x457B0AF0;
					firmware = 0x03050210;
				break;

				case 360:
					oe_tag_k = OE_TAG_KERNEL_360;
					oe_tag_u = OE_TAG_USER_360;
					oe_tag_r = OE_TAG_REBOOT_360;
					kernel_tag = 0x4C940DF0;
					vsh_tag = 0x38020AF0;
					user_tag = 0x457B0AF0;
					firmware = 0x03060010;
				break;

				case 371: case 380: case 390:
					oe_tag_k = OE_TAG_KERNEL_371;
					oe_tag_u = OE_TAG_USER_371;
					oe_tag_r = (!(mode & SLIM_FLAG)) ? OE_TAG_REBOOT_371 : OE_TAG_REBOOT_SLIM_371;
					kernel_tag = (!(mode & SLIM_FLAG)) ? 0x4C9412F0 : 0x4C9413F0;
					vsh_tag = 0x38020AF0;
					user_tag = 0x457B0AF0;

					switch (firmware)
					{
						case 371:
							firmware = 0x03070110;
						break;

						case 380:
							firmware = 0x03080010;
						break;

						case 390:
							firmware = 0x03090010;
						break;
					}

				break;

				default:
					printf("Invalid firmware.\n");
					return -1;
			}
		}

		if (mode & PLUGIN_FLAG)
		{
			oe_tag_k = OE_TAG_GENERIC_KERNEL;
			kernel_tag = 0xDADADAF0;			
		}
	}

	memset(&header, 0, sizeof(header));
	
	// Fill simple fields
	header.signature = 0x5053507E;
	header.comp_attribute = 1;
	header.version = 1;
	header.elf_size = size;

	if (mode & FIRMWARE_FLAG && !(mode & SDK_FLAG))
		header.devkitversion = firmware;
	else
		header.devkitversion = 0;

	header._80 = 0x80;

	elf_header = (Elf32_Ehdr *)in;
	if (elf_header->e_magic != 0x464C457F)
	{
		if (elf_header->e_magic == 0x5053507E || elf_header->e_magic == 0x4543537E)
		{
			printf("Already packed.\n");
			return 0;
		}
		
		printf("Invalid ELF signature.\n");
		return -1;
	}
	
	// Fill fields from elf header
	header.entry = elf_header->e_entry;
	header.nsegments = (elf_header->e_phnum > 2) ? 2 : elf_header->e_phnum;

	if (header.nsegments == 0)
	{
		printf("There are no segments.\n");
		return -1;
	}

	// Fill segements
	segments = (Elf32_Phdr *)&in[elf_header->e_phoff];

	for (i = 0; i < header.nsegments; i++)
	{
		header.seg_align[i] = segments[i].p_align;
		header.seg_address[i] = segments[i].p_vaddr;
		header.seg_size[i] = segments[i].p_memsz;
	}

	// Fill module info fields
	header.modinfo_offset = segments[0].p_paddr;
	modinfo = (PspModuleInfo *)&in[header.modinfo_offset&0x7FFFFFFF];
	header.attribute = modinfo->attribute;
	header.module_ver_lo = modinfo->module_ver_lo;
	header.module_ver_hi = modinfo->module_ver_hi;
	strncpy(header.modname, modinfo->modname, 28);

	sections = (Elf32_Shdr *)&in[elf_header->e_shoff];
	strtab = (char *)(sections[elf_header->e_shstrndx].sh_offset + in);

	header.bss_size = segments[0].p_memsz - segments[0].p_filesz;
	
	for (i = 0; i < elf_header->e_shnum; i++)
	{
		if (strcmp(strtab+sections[i].sh_name, ".bss") == 0)
		{
			printf("X = 0x%08X.\n", sections[i].sh_size);
			printf("Y = 0x%08X.\n", header.bss_size);

			if (strcmp(header.modname, "$ystemControl") == 0)
				sections[i].sh_size = header.bss_size;
			else
				header.bss_size = sections[i].sh_size;

			printf("Applied: 0x%08X.\n", header.bss_size);
			
			break;
		}
	}

	if (i == elf_header->e_shnum)
	{
		printf("Error: .bss section not found.\n");
		return -1;
	}	

	// Fill mode fields

	if (header.attribute & 0x1000)
	{
		// Kernel

		if (mode & FIRMWARE_FLAG)
		{
			header.tag = kernel_tag;

			if (strcmp(header.modname, "SystemControl") == 0)
			{
				header.oe_tag = oe_tag_r;
				printf("HEN/OE/M33 core file.\n");
			}
			else
			{
				header.oe_tag = oe_tag_k;
			}

			if (firmware < 0x02060010)
				header.decrypt_mode = 1;
			else
				header.decrypt_mode = 2;
		}

		else if (mode & UPDATER_FLAG)
		{
			printf("Updater module can only be vsh mode.\n");
			return -1;
		}
		
		else if (mode & PBP_GAME_FLAG)
		{
			printf("PBP cannot be kernel mode.\n");
			return -1;
		} 
	}
	else
	{
		if (mode & FIRMWARE_FLAG)
		{		
			if (firmware <= 0x02060010)
			{
				printf("User/Vsh for this firmware not implemented yet.\n");
				return -1;
			}
			
			header.oe_tag = oe_tag_u;

			if (header.attribute & 0x800)
			{
				// vsh
				header.tag = vsh_tag;
				header.decrypt_mode = 3;
			}
			else
			{
				// user
				header.tag = user_tag;
				header.decrypt_mode = 4;
			}
		}
		else if (mode & UPDATER_FLAG)
		{
			if (!(header.attribute & 0x800))
			{
				printf("Updater module can only be vsh mode.\n");
				return -1;
			}

			header.oe_tag = OE_TAG_MS_UPDATER;
			header.tag = 0x0B000000;
			header.decrypt_mode = 0x0C;
		}

		else if (mode & PBP_GAME_FLAG)
		{
			if (!(header.attribute & 0x200))
			{
				printf("PBP game has to have flag 0x0200.\n");
				return -1;
			}
			
			header.oe_tag = OE_TAG_MS_GAME;
			header.tag = 0xADF305F0;
			header.decrypt_mode = 0x0D;
		}
	}

	// Fill key data with random bytes
	ReadFile("/dev/urandom", header.key_data0, 0x30);
	ReadFile("/dev/urandom", header.key_data1, 0x10);
	ReadFile("/dev/urandom", &header.key_data2, 4);
	ReadFile("/dev/urandom", header.key_data3, 0x1C);

	if (!(mode & FIRMWARE_FLAG) || firmware == 0x01000300)
	{
		if (!(mode & PBP_GAME_FLAG))
			ReadFile("/dev/urandom", header.scheck, 0x58);
	}

	gzFile comp = gzopen("temp.bin", "wb");
	if (!comp)
	{
		printf("Cannot create temp file.\n");
		return -1;
	}

	if (gzwrite(comp, in, size) != size)
	{
		printf("Error in compression.\n");
		return -1;
	}

	gzclose(comp);

	if (mode & SCE_HEADER_FLAG)
	{
		memcpy(out, sce_header, 0x40);
		out += 0x40;
	}

	header.comp_size = ReadFile("temp.bin", out+0x150, 6*1024*1024);
	remove("temp.bin");

	header.psp_size = header.comp_size+0x150;
	memcpy(out, &header, 0x150);

	if (mode & SCE_HEADER_FLAG)
		header.psp_size += 0x40;

	return header.psp_size;
}

u8 input[6*1024*1024], output[6*1024*1024];

int main(int argc, char *argv[])
{
	char *out;
	int firmware = 0;
	int mode = 0;
	
	if (argc < 3)
	{
		printf("Invalid usage.\n");
		return -1;
	}
	
	if (argc == 3)
		out = argv[2];
	else
		out = argv[3];

	if (argv[1][0] == 'S')
	{
		mode |= SCE_HEADER_FLAG;
		memmove(argv[1], argv[1]+1, strlen(argv[1]));
	}

	if (strlen(argv[1]) >= 5 && argv[1][4] == 'S')
	{
		printf("Slim version.\n");
		mode |= SLIM_FLAG;
	}

	if (strlen(argv[1]) >= 4 && argv[1][3] == 'I')
	{
		printf("Ipl version.\n");
		mode |= IPL_FLAG;
		argv[1][3] = 0;
	}

	else if (strlen(argv[1]) >= 4 && argv[1][3] == 'P')
	{
		printf("Plugin.\n");
		mode |= PLUGIN_FLAG;
		argv[1][3] = 0;
	}

	if (strcmp(argv[1], "UPD") == 0)
	{
		printf("Updater main module\n");
		mode |= UPDATER_FLAG;
	}
	else if (strcmp(argv[1], "20G") == 0)
	{
		printf("2.00 SDK module\n");
		firmware = 200;
		mode |= SDK_FLAG | FIRMWARE_FLAG;
	}
	else if (strcmp(argv[1], "10G") == 0)
	{
		printf("1.00 SDK module\n");
		firmware = 100;
		mode |= SDK_FLAG | FIRMWARE_FLAG;
	}
	if (strcmp(argv[1], "PBG") == 0)
	{
		printf("PBP game\n");
		mode |= PBP_GAME_FLAG;
	}
	else if (strcmp(argv[1], "UPD") != 0 && strcmp(argv[1], "10G") != 0)
	{
		mode |= FIRMWARE_FLAG;
		sscanf(argv[1], "%d", &firmware);
		printf("Firmware %d.\n", firmware);
	}

	int size = ReadFile(argv[2], input, sizeof(input));
	if (size <= 0)
	{
		printf("Cannot read file %s.\n", argv[2]);
		return -1;
	}

	size = PspPack(input, size, output, mode, firmware);
	if (size < 0)
	{
		printf("Error packing module.\n");
		return -1;
	}
	else if (size == 0)
		return 0;

	size = WriteFile(out, output, size);
	if (size <= 0)
	{
		printf("Error writing file %s.\n", out);
		return -1;
	}


	return 0;
}

