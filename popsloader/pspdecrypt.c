#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspcrypt.h>
#include <psputilsforkernel.h>
#include <pspthreadman_kernel.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <pspdecrypt.h>

/*static int Scramble(u32 *buf, u32 size, u32 code)
{
	buf[0] = 5;
	buf[1] = buf[2] = 0;
	buf[3] = code;
	buf[4] = size;

	if (sceUtilsBufferCopyWithRange(buf, size+0x14, buf, size+0x14, 7) < 0)
	{
		return -1;
	}

	return 0;
}

////////// Decryption 2 //////////

// kernel modules 3.00 
u8 keys300_0[0x10] =
{
	0x9F, 0x67, 0x1A, 0x7A, 0x22, 0xF3, 0x59, 0x0B,
    0xAA, 0x6D, 0xA4, 0xC6, 0x8B, 0xD0, 0x03, 0x77

};

// user modules 3.00 
u8 keys300_1[0x10] =
{
	0x15, 0x07, 0x63, 0x26, 0xDB, 0xE2, 0x69, 0x34,
    0x56, 0x08, 0x2A, 0x93, 0x4E, 0x4B, 0x8A, 0xB2

};

// kernel modules 3.03 
u8 keys303_0[0x10] =
{
	0x7b, 0xa1, 0xe2, 0x5a, 0x91, 0xb9, 0xd3, 0x13,
	0x77, 0x65, 0x4a, 0xb7, 0xc2, 0x8a, 0x10, 0xaf
};

// kernel modules 3.10 
u8 keys310_0[0x10] =
{
	0xa2, 0x41, 0xe8, 0x39, 0x66, 0x5b, 0xfa, 0xbb,
	0x1b, 0x2d, 0x6e, 0x0e, 0x33, 0xe5, 0xd7, 0x3f
};

// user modules 3.10 
u8 keys310_1[0x10] =
{
	0xA4, 0x60, 0x8F, 0xAB, 0xAB, 0xDE, 0xA5, 0x65,
	0x5D, 0x43, 0x3A, 0xD1, 0x5E, 0xC3, 0xFF, 0xEA
};

// reboot.bin 3.10 
u8 keys310_3[0x10] =
{
    0x2E, 0x00, 0xF6, 0xF7, 0x52, 0xCF, 0x95, 0x5A,
    0xA1, 0x26, 0xB4, 0x84, 0x9B, 0x58, 0x76, 0x2F
};

// kernel modules 3.30  
u8 keys330_0[0x10] = 
{ 
	0x3B, 0x9B, 0x1A, 0x56, 0x21, 0x80, 0x14, 0xED,
	0x8E, 0x8B, 0x08, 0x42, 0xFA, 0x2C, 0xDC, 0x3A
};

// user modules 3.30  
u8 keys330_1[0x10] = 
{ 
    0xE8, 0xBE, 0x2F, 0x06, 0xB1, 0x05, 0x2A, 0xB9, 
    0x18, 0x18, 0x03, 0xE3, 0xEB, 0x64, 0x7D, 0x26 
}; 

// 3.70 common and fat kernel modules 
u8 keys370_0[0x10] = 
{
	0x26, 0x38, 0x0A, 0xAC, 0xA5, 0xD8, 0x74, 0xD1, 
	0x32, 0xB7, 0x2A, 0xBF, 0x79, 0x9E, 0x6D, 0xDB
};

typedef struct
{
    u32 tag; // 4 byte value at offset 0xD0 in the PRX file
    u8  *key; // 16 bytes keys
    u8 code; // code for scramble
} TAG_INFO2;

static TAG_INFO2 g_tagInfo2[] =
{
	{ 0x4C9412F0, keys370_0, 0x43 },
		
	{ 0x4C940BF0, keys330_0, 0x43 }, 
	{ 0x457B0AF0, keys330_1, 0x5B }, 
	
	{ 0xcfef09f0, keys310_0, 0x62 },
	{ 0x457b08f0, keys310_1, 0x5B },
	{ 0xcfef08f0, keys310_3, 0x62 },

	{ 0xCFEF07F0, keys303_0, 0x62 },
	{ 0xCFEF06F0, keys300_0, 0x62 },
	{ 0x457B06F0, keys300_1, 0x5B },
};


static TAG_INFO2 *GetTagInfo2(u32 tagFind)
{
    int iTag;

    for (iTag = 0; iTag < sizeof(g_tagInfo2) / sizeof(TAG_INFO2); iTag++)
	{
        if (g_tagInfo2[iTag].tag == tagFind)
		{
            return &g_tagInfo2[iTag];
		}
	}

	return NULL; // not found
}

static int DecryptPRX2(const u8 *inbuf, u8 *outbuf, u32 size, u32 tag)
{
	TAG_INFO2 * pti = GetTagInfo2(tag);

	if (!pti)
	{
		Kprintf("Unknown tag 0x%08X.\n", tag);
		return -1;
	}	

	int retsize = *(int *)&inbuf[0xB0];
	u8	tmp1[0x150], tmp2[0x90+0x14], tmp3[0x60+0x14];

	memset(tmp1, 0, 0x150);
	memset(tmp2, 0, 0x90+0x14);
	memset(tmp3, 0, 0x60+0x14);

	if (inbuf != outbuf)
		memcpy(outbuf, inbuf, size);

	if (size < 0x160)
	{
		Kprintf("Buffer not big enough.\n");
		return -2;
	}

	if (((u32)outbuf & 0x3F))
	{
		Kprintf("Buffer not aligned to 64 bytes.\n");
		return -3;
	}

	if ((size - 0x150) < retsize)
	{
		Kprintf("No enough data.\n");
		return -4;
	}	

	memcpy(tmp1, outbuf, 0x150);

	int i, j;
	u8 *p = tmp2+0x14;

	for (i = 0; i < 9; i++)
	{
		for (j = 0; j < 0x10; j++)
		{
			p[(i << 4) + j] = pti->key[j]; 			
		}

		p[(i << 4)] = i;
	}	

	if (Scramble((u32 *)tmp2, 0x90, pti->code) < 0)
	{
		Kprintf("Error in Scramble #1.\n");
		return -5;
	}

	memcpy(outbuf, tmp1+0xD0, 0x5C);
	memcpy(outbuf+0x5C, tmp1+0x140, 0x10);
	memcpy(outbuf+0x6C, tmp1+0x12C, 0x14);
	memcpy(outbuf+0x80, tmp1+0x080, 0x30);
	memcpy(outbuf+0xB0, tmp1+0x0C0, 0x10);
	memcpy(outbuf+0xC0, tmp1+0x0B0, 0x10);
	memcpy(outbuf+0xD0, tmp1+0x000, 0x80);

	memcpy(tmp3+0x14, outbuf+0x5C, 0x60);	

	if (Scramble((u32 *)tmp3, 0x60, pti->code) < 0)
	{
		Kprintf("Error in Scramble #2.\n");
		return -6;
	}

	memcpy(outbuf+0x5C, tmp3, 0x60);
	memcpy(tmp3, outbuf+0x6C, 0x14);
	memcpy(outbuf+0x70, outbuf+0x5C, 0x10);
	memset(outbuf+0x18, 0, 0x58);
	memcpy(outbuf+0x04, outbuf, 0x04);

	*((u32 *)outbuf) = 0x014C;
	memcpy(outbuf+0x08, tmp2, 0x10);	

	// sha-1 
	if (sceUtilsBufferCopyWithRange(outbuf, 3000000, outbuf, 3000000, 0x0B) != 0)
	{
		Kprintf("Error in sceUtilsBufferCopyWithRange 0xB.\n");
		return -7;
	}	

	if (memcmp(outbuf, tmp3, 0x14) != 0)
	{
		Kprintf("SHA-1 is incorrect.\n");
        return -8;
	}

	int iXOR;

	for (iXOR = 0; iXOR < 0x40; iXOR++)
	{
		tmp3[iXOR+0x14] = outbuf[iXOR+0x80] ^ tmp2[iXOR+0x10];
	}

	if (Scramble((u32 *)tmp3, 0x40, pti->code) != 0)
	{
		Kprintf("Error in Scramble #2.\n");
		return -9;
	}
	
	for (iXOR = 0x3F; iXOR >= 0; iXOR--)
	{
		outbuf[iXOR+0x40] = tmp3[iXOR] ^ tmp2[iXOR+0x50]; // uns 8
	}

	memset(outbuf+0x80, 0, 0x30);
	*(u32 *)&outbuf[0xA0] = 1;

	memcpy(outbuf+0xB0, outbuf+0xC0, 0x10);
	memset(outbuf+0xC0, 0, 0x10);

	// The real decryption
	if (sceUtilsBufferCopyWithRange(outbuf, size, outbuf+0x40, size-0x40, 0x1) != 0)
	{
		Kprintf("Error in sceUtilsBufferCopyWithRange 0x1.\n");
		return -1;
	}

	if (retsize < 0x150)
	{
		// Fill with 0
		memset(outbuf+retsize, 0, 0x150-retsize);		
	}

	return retsize;
}

static int _pspDecryptPRX(u32 *arg)
{
	u8 *inbuf = (u8 *)arg[0];
	u8 *outbuf = (u8 *)arg[1];
	u32 size = arg[2];

	return DecryptPRX2(inbuf, outbuf, size, *(u32 *)&inbuf[0xD0]);
}

int pspDecryptPRX(u8 *inbuf, u8 *outbuf, u32 size)
{
	int k1 = pspSdkSetK1(0);
	u32 arg[3];

	arg[0] = (u32)inbuf;
	arg[1] = (u32)outbuf;
	arg[2] = size;
	
	int res = sceKernelExtendKernelStack(0x2000, (void *)_pspDecryptPRX, arg);

	pspSdkSetK1(k1);
	return res;
}

////////// SignCheck //////////

u8 check_keys0[0x10] =
{
	0x71, 0xF6, 0xA8, 0x31, 0x1E, 0xE0, 0xFF, 0x1E,
	0x50, 0xBA, 0x6C, 0xD2, 0x98, 0x2D, 0xD6, 0x2D
};

u8 check_keys1[0x10] =
{
	0xAA, 0x85, 0x4D, 0xB0, 0xFF, 0xCA, 0x47, 0xEB,
	0x38, 0x7F, 0xD7, 0xE4, 0x3D, 0x62, 0xB0, 0x10
};

int pspIsSignChecked(u8 *buf)
{
	int k1 = pspSdkSetK1(0);
	int i, res = 0;

	for (i = 0; i < 0x58; i++)
	{
		if (buf[0xD4+i] != 0)
		{
			res = 1;
			break;
		}
	}

	pspSdkSetK1(k1);
	return res;
}

////////// UnsignCheck //////////

static int Decrypt(u32 *buf, int size)
{
	buf[0] = 5;
	buf[1] = buf[2] = 0;
	buf[3] = 0x100;
	buf[4] = size;

	if (sceUtilsBufferCopyWithRange(buf, size+0x14, buf, size+0x14, 8) != 0)
		return -1;
	
	return 0;
}

int pspUnsignCheck(u8 *buf)
{
	u8 enc[0xD0+0x14];
	int iXOR, res;
	int k1 = pspSdkSetK1(0);

	memcpy(enc+0x14, buf+0x80, 0xD0);

	for (iXOR = 0; iXOR < 0xD0; iXOR++)
	{
		enc[iXOR+0x14] ^= check_keys1[iXOR&0xF]; 
	}

	if ((res = Decrypt((u32 *)enc, 0xD0)) < 0)
	{
		Kprintf("Decrypt failed.\n");
		pspSdkSetK1(k1);
		return res;
	}

	for (iXOR = 0; iXOR < 0xD0; iXOR++)
	{
		enc[iXOR] ^= check_keys0[iXOR&0xF];
	}

	memcpy(buf+0x80, enc+0x40, 0x90);
	memcpy(buf+0x110, enc, 0x40);

	pspSdkSetK1(k1);
	return 0;
}*/

////////// Decompression //////////

int pspIsCompressed(u8 *buf)
{
	int k1 = pspSdkSetK1(0);
	int res = 0;

	if (buf[0] == 0x1F && buf[1] == 0x8B)
		res = 1;
	else if (memcmp(buf, "2RLZ", 4) == 0)
		res = 1;

	pspSdkSetK1(k1);
	return res;
}

static int _pspDecompress(u32 *arg)
{
	int retsize;
	u8 *inbuf = (u8 *)arg[0];
	u8 *outbuf = (u8 *)arg[1];
	u32 outcapacity = arg[2];
	
	if (inbuf[0] == 0x1F && inbuf[1] == 0x8B)
	{
		retsize = sceKernelGzipDecompress(outbuf, outcapacity, inbuf, NULL);
	}
	else if (memcmp(inbuf, "2RLZ", 4) == 0) // 2RLZ
	{
		retsize = sceKernelLzrcDecode(outbuf, outcapacity, inbuf, NULL);
	}
	else
	{
		retsize = -1;
	}

	return retsize;
}

int pspDecompress(const u8 *inbuf, u8 *outbuf, u32 outcapacity)
{
	int k1 = pspSdkSetK1(0);
	u32 arg[3];

	arg[0] = (u32)inbuf;
	arg[1] = (u32)outbuf;
	arg[2] = outcapacity;

	int res = _pspDecompress(arg);
	
	//int res = sceKernelExtendKernelStack(0x2000, (void *)_pspDecompress, arg);
	
	pspSdkSetK1(k1);
	return res;
}



