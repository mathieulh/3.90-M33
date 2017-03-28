#ifndef __BOOTINFO__
#define __BOOTINFO__

#define BOOT_PARAM_ID_FILENAME	0x0002
#define BOOT_PARAM_ID_VSHPARAM	0x0004
#define BOOT_PARAM_ID_GAMEINFO	0x0020
#define BOOT_PARAM_ID_DISCIMAGE	0x0040
#define BOOT_PARAM_ID_USERPARAM	0x0100
#define BOOT_PARAM_ID_UNKNOWN	0x0200

#define BOOT_MODULE_FLAGS_USER		1
#define BOOT_MODULE_FLAGS_EXECUTABLE	2
#define BOOT_MODULE_FLAGS_FILE	4

#define BOOT_PARAM_FLAGS_BLOCK	1

enum PspBootType
{
	PSP_BOOT_PLAIN = 0,
	PSP_BOOT_VSH = 1,
	PSP_BOOT_USERAPP = 2, /* includes game, pops and others */
};

enum PspKeyConfig
{
	PSP_KEY_CONFIG_GAME = 1,
	PSP_KEY_CONFIG_VSH = 2,
	PSP_KEY_CONFIG_UPDATER = 3,
	PSP_KEY_CONFIG_POPS = 4,
	PSP_KEY_CONFIG_APP = 5,
	PSP_KEY_CONFIG_UMDEMU = 7,
};

typedef struct BootModuleInfo // size 0x20
{
	void *unk1; // 0
	u32  addr; // 4
	SceSize  entrysize; // 8
	int  unk2; // C
	u8   flags; // 0x10
	u8   unkf; // 0x11
	u8   mpid; // 0x12
	u8   pad; // 0x13

	union
	{
		int    apitype; // 0x14
		SceUID blockuid;
	};

	u32    argsize; // 0x18
	SceUID argsuid; // 0x1C
} BootModuleInfo;

typedef struct BootParam // size 0x1C
{
	void *buf; // 0
	SceSize bufsize; // 4
	u16 id; // 8
	u16 flags; // A	 
	SceUID uid; // C
	int unk3[3]; // 0x10
} BootParam;

typedef struct BootInfo // size -> 0x80
{
	void *topaddr; // 0
	u32  memsize; // 4
	int  nextmodule; // 8
	int  nmodules; // C
	BootModuleInfo *bootmoduleinfo; // 0x10
	int unk14; // 0x14
	u8  boottype; // 0x18
	u8  pad[3]; // 0x19
	u32 nbootparams; // 0x1C
	BootParam *bootparams; // 0x20
	int filename_index; // 0x24
	int args_index; // 0x28
	int unk2C; // 0x2C
	u32 buildversion; // 0x30
	int unk34; // 0x34
	char *unkstr; // 0x38
	int keyconfig; //  0x3C
	u32 unk40[2]; // 0x40
	struct BootInfo *reboot_bootinfo; // Copy of boot info at a previous state 
	void *unk4C; // 0x4C
	u8 reserved[0x30]; // 0x50
} BootInfo;

#endif

