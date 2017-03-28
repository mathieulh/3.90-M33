#ifndef __POPSLIBRARIES_H__
#define __POPSLIBRARIES_H__

typedef struct
{
	u32 input_nid;
	u32 output_nid;
} Nids;

typedef struct
{
	const char *name;
	Nids *nids;
	int  nnids;
} LibraryData;

#define N_LIBRARIES_370	4

Nids sceSysreg_driver370[14] =
{
	{ 0x6C305CB3, 0x0A83FC7B },
	{ 0x07881A0B, 0x158AD4FC },
	{ 0x76220E94, 0x2DB0EB28 },
	{ 0x3199CF1C, 0x44F6CDA7 },
	{ 0xB2C9B019, 0x8BE2D520 },
	{ 0x0C7E4512, 0x9100B4E5 },
	{ 0xE5B3D348, 0x9BB70D34 },
	{ 0xA57CBE53, 0x9FC87ED4 },
	{ 0x27C0A714, 0xA9CD1C1F },
	{ 0x595C27FB, 0xAA63C8BD }, // 10
	{ 0x38CD3AB5, 0xBB26CF1F },
	{ 0x1CD747DC, 0xC1DA05D2 },
	{ 0xA9997109, 0xDE59DACB },
	{ 0x545CE0A8, 0xF844DDF3 }
};

Nids sceSyscon_driver370[1] =
{
	{ 0x4E5C5F26, 0xE5E35721 }
};

Nids sceClockgen_driver370[1] =
{
	{ 0xD4F6990D, 0x4EB657D5 }
};

Nids sceGe_driver370[2] =
{
	{ 0xBA035FC8, 0x1F6752AD },
	{ 0x58C59880, 0x5BAA5439 }
};

LibraryData libraries370[N_LIBRARIES_370] =
{
	{
		"sceSysreg_driver",
		sceSysreg_driver370,
		14
	},

	{
		"sceSyscon_driver",
		sceSyscon_driver370,
		1
	},

	{
		"sceClockgen_driver",
		sceClockgen_driver370,
		1
	},

	{
		"sceGe_driver370",
		sceGe_driver370,
		2
	}
};


#endif

