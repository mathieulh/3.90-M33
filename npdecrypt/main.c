#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <psploadexec_kernel.h>
#include <pspthreadman_kernel.h>

#include <systemctrl.h>

#include <stdio.h>
#include <string.h>

#include "lzrc.h"

PSP_MODULE_INFO("NPNPNPNPNP", 0x5006, 1, 0);

/*u8 keys0[0x28] = 
{
	0x01, 0x21, 0xEA, 0x6E, 0xCD, 0xB2, 0x3A, 0x3E, 
	0x23, 0x75, 0x67, 0x1C, 0x53, 0x62, 0xE8, 0xE2, 
	0x8B, 0x1E, 0x78, 0x3B, 0x1A, 0x27, 0x32, 0x15, 
	0x8B, 0x8C, 0xED, 0x98, 0x46, 0x6C, 0x18, 0xA3, 
	0xAC, 0x3B, 0x11, 0x06, 0xAF, 0xB4, 0xEC, 0x3B
};*/
typedef struct
{
	u32 shit[4];
	u32 pos;
	u32 size;
	u32 dummy[2];
} Position;

u8 *act;
SceUID fd;
int sectors_per_block;
u32 start_pos;
u8 gamekeys[0x10], sec_keys[0x10];
Position *pos_table;

void WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	sceIoWrite(fd, buf, size);
	sceIoClose(fd);
}

static int GetLicenseKeys(u8 *gamekeys, char *license, u32 mode)
{
	u8 buf[0x98];
	char path[0x40]; // sp+0xA0
	
	memset(path, 0, 0x40);

	strcpy(path, "ms0:/PSP/LICENSE/");
	strcat(path+0x11, license);
	strcat(path, ".rif");

	SceUID fd = sceIoOpen(path, 0x04000001, 0);

	int read = sceIoRead(fd, buf, 0x98);
	sceIoClose(fd);

	if (read != 0x98)
		return -8;

	if (__builtin_allegrex_wsbw(*(u32 *)&buf[4]) == 3)
	{
		return scePspNpDrm_driver_0F9547E6(gamekeys, 0, buf, mode);
	}
	
	fd = sceIoOpen("flash2:/act.dat", 0x04000001, 0);
	if (fd < 0)
		return -9;
	
	read = sceIoRead(fd, act, 0x1038);
	sceIoClose(fd);
	
	if (read != 0x1038)
		return -10;

	return scePspNpDrm_driver_0F9547E6(gamekeys, act, buf, mode);
}

static int NpegOpen_(u32 *args)
{
	u32 pbp_header[0x28/4]; // sp+0x70
	//u8 keys[0x28]; // sp+0x200
	u8 npdrm[0x10]; // sp+0x50
	//u8 gamekeys[0x10]; // sp+0x60
	u8 mac[0x30]; // sp
	u8 cipher[0x20]; // sp+0x30
	int postable_size, mod, i;
	int res;
	u32 *p;
	u8 *buf;

	char *file;
	u8 *header_mem, *act_mem, *table_mem;
	int *table_size;

	file = (char *)args[0];
	header_mem = (u8 *)args[1];
	act_mem = (u8 *)args[2];
	table_mem = (u8 *)args[3];
	table_size = (int *)args[4];
	
	buf = header_mem;
	act = act_mem;
	pos_table = (Position *)table_mem;

	if (scePspNpDrm_driver_04618D16(npdrm) < 0)
	{
		return -1;
	}
	
	fd = sceIoOpen(file, 0x04000001, 0);
	if (fd < 0)
	{
		return -2;
	}
	
	if (sceIoRead(fd, pbp_header, sizeof(pbp_header)) < sizeof(pbp_header))
	{
		return -3;
	}
	
	if (pbp_header[0] != 0x50425000)
	{
		return -4;
	}

	if (sceIoLseek(fd, pbp_header[0x24/4], PSP_SEEK_SET) < 0) // psar pos
	{
		return -5;
	}
	
	if (sceIoRead(fd, buf, 0x100) < 0x100)
	{
		return -6;
	}
	
	if (*(u32 *)&buf[0] != 0x4d55504e || *(u32 *)&buf[4] != 0x474d4944) // NPUMDIMG
	{
		return -7;
	}

	res = GetLicenseKeys(gamekeys, (char *)buf+0x10, *(u32 *)&buf[8]);
	if (res < 0)
	{
		return res;
	}

	memcpy(sec_keys, buf+0xA0, 0x10);

	//WriteFile("keys.bin", gamekeys, 0x10);
	//WriteFile("keys_sec.bin", sec_keys, 0x10);
	//WriteFile("buf_before.bin", buf, 0xD0);

	if (sceDrmBBMacInit(mac, 3) < 0)
	{
		return -11;
	}

	if (sceDrmBBMacUpdate(mac, buf, 0xC0) < 0)
	{
		return -12;
	}

	if (sceDrmBBMacFinal2(mac, buf+0xC0, gamekeys) < 0)
	{
		return -13;
	}

	//WriteFile("buf_after.bin", buf, 0xD0);

	//WriteFile("buf40_bef.bin", buf+0x40, 0x60);

	if (sceDrmBBCipherInit(cipher, 1, 2, sec_keys, gamekeys, 0) < 0)
	{
		return -14;
	}
	
	if (sceDrmBBCipherUpdate(cipher, buf+0x40, 0x60) < 0)
	{
		return -15;
	}

	if (sceDrmBBCipherFinal(cipher) < 0)
	{
		return -16;
	}

	//WriteFile("buf40_af.bin", buf+0x40, 0x60);

	if (sceDrmBBCipherInit(cipher, 1, 1, sec_keys, gamekeys, 0) < 0)
	{
		return -40;
	}
	
	if (sceDrmBBCipherUpdate(cipher, buf+0x40, 0x60) < 0)
	{
		return -41;
	}

	if (sceDrmBBCipherFinal(cipher) < 0)
	{
		return -42;
	}

	//WriteFile("buf40_af2.bin", buf+0x40, 0x60);

	
	if (sceDrmBBMacInit(mac, 3) < 0)
	{
		return -30;
	}

	if (sceDrmBBMacUpdate(mac, buf, 0xC0) < 0)
	{
		return -31;
	}

	int g = sceDrmBBMacFinal2(mac, buf+0xC0, gamekeys);
	if (g < 0)
	{
		return g;
	}

	//WriteFile("buf_after2.bin", buf, 0xD0);

	if (sceDrmBBMacInit(mac, 3) < 0)
	{
		return -60;
	}

	if (sceDrmBBMacUpdate(mac, buf, 0xC0) < 0)
	{
		return -61;
	}

	if (sceDrmBBMacFinal2(mac, buf+0xC0, gamekeys) < 0)
	{
		return -62;
	}

	// sp+238 = (buf[0x64/4]-buf[0x54/4])+1 -> 22F3F-0+1 = 22F40; (umd size in sectors)
	// sp+23C = (sp+238) /  buf[0x0C/4]; if (sp+238 % buf[0x0C/4) != 0) sp+238++;
	// -> sp+23C = 0x22F4
	// sp+240 = sp+23C*32 = 0x45E80
	// sp+250 = sfosize+0x30
	// sp+248 = psp offset
	// sp+24C = 0
	// seek sfo
	// Read sfo into fp
	// Copy buf+0x10 (npeg string + title) into fp+sfo_size
	// seek psp offset
	// Read 0x28 bytes
	// Do some ddrb verify
	// Seek psp offset +0x560
	// Read 0x34 bytes into fp (npeg string shit)
	// Comparison of fp with buf+0x10. If doesn't match, error

	
	sectors_per_block = *(u32 *)&buf[0x0C];

	postable_size = *(u32 *)&buf[0x64] - *(u32 *)&buf[0x54] + 1;
	mod = postable_size % sectors_per_block;
	postable_size = postable_size / sectors_per_block;	

	if (mod)
		postable_size++;

	postable_size *= 32;	

	/*if (sceDrmBBMacInit(mac, 3) < 0)
		return -17;*/

	sceIoLseek(fd, pbp_header[0x24/4] + *(u32 *)&buf[0x6C], PSP_SEEK_SET);
	if (sceIoRead(fd, pos_table, postable_size) < postable_size)
		return -18;
	
	p = (u32 *)pos_table;
	for (i = 0; i < postable_size; i += 32, p += 8)
	{
		p[4] ^= (p[2] ^ p[3]);
		p[5] ^= (p[1] ^ p[2]);
		p[6] ^= (p[0] ^ p[3]);
		p[7] ^= (p[0] ^ p[1]);
	}
	
	/*for (i = 0; i < postable_size; i += 0x8000)
	{
		int s, j;
		int remaining = postable_size-i;

		if (remaining > 0x8000)
			s = 0x8000;
		else
			s = remaining;

		if (sceIoRead(fd, bigbuf, s) != s)
			return -18;

		if (sceDrmBBMacUpdate(mac, bigbuf, s) < 0)
			return -19;

		if (!done)
		{
			u32 *p = (u32 *)bigbuf;
			
			for (j = 0; j < s; j += 32, p += 8)
			{
				p[4] ^= (p[2] ^ p[3]);
				p[5] ^= (p[1] ^ p[2]);
				p[6] ^= (p[0] ^ p[3]);
				p[7] ^= (p[0] ^ p[1]);
			}

			done = 1;
			WriteFile("ms0:/table.bin", bigbuf, s);
		}	
	}

	if (sceDrmBBMacFinal2(mac, buf+0xB0, gamekeys) < 0)
		return -20;*/

	// sp+1a0 = fd
	// sp+1a4 = sp+238
	// sp+1a8 = sp+23C
	// sp+1ac = buf[0xC/4]
	// memcpy(sp+1b0, buf+0x40, 0x28)
	// sp+1D8 = buf[0x84/4]; // 0
	// psar_pos = sp+230
	// g_47C = buf[0x6C/4] // 0x100
	// g_480 = buf[0x98/4]; // 0x1003FFE
	// g_484 = buf[0x80/4]; // 0
	// memcpy(g_b418, buf+0x10, 0x30);
	// g_b448 = buf[70/4]; 4745504E (NPEG)
	// g_b44C = buf[74] 3030302D -000
	// g_b450 = buf[78]; 0
	// g_b454 = buf[7C]
	// memcpy(g_gamekeys (b458 or b448+0x10), gamekeys, 0x10
	// memcpy(g_buf+b468; buf+0xA0, 0x10)// random shit (extended keys?)
	// SetUmdData & the other with game keys


	//WriteFile("ms0:/drm_buffer2.bin", buf, 0x100);

	*table_size = postable_size;
	start_pos = pbp_header[0x24/4];	
	
	return 0;
}

int NpegOpen(char *file, u8 *header_mem, u8 *act_mem, u8 *table_mem, int *table_size)
{
	u32 args[5];
	int k1 = pspSdkSetK1(0);

	args[0] = (u32)file;
	args[1] = (u32)header_mem;
	args[2] = (u32)act_mem;
	args[3] = (u32)table_mem;
	args[4] = (u32)table_size;

	int res = sceKernelExtendKernelStack(0x2000, (void *)NpegOpen_, args);

	pspSdkSetK1(k1);
	return res;
}

static int NpegReadBlock_(u32 *args)
{
	void *buf1, *buf2;
	int n, s;
	u8 mac[0x30];
	u8 cipher[0x20];
	int (* decompress)(void *, u32, void *, u32) = (void *)lzrc;


	buf1 = (void *)args[0];
	buf2 = (void *)args[1];
	n = (int)args[2];	

	s = (pos_table[n].size+ 0xF) & 0xFFFFFFF0;

	if (sceIoLseek(fd, start_pos+pos_table[n].pos, PSP_SEEK_SET) < 0)
		return -1;
	
	if (sceIoRead(fd, buf1, s) != s)
		return -2;
	
	if (!(pos_table[n].dummy[0] & 1))
	{
		if (sceDrmBBMacInit(mac, 3) < 0)
			return -3;

		if (sceDrmBBMacUpdate(mac, buf1, s) < 0)
			return -4;

		if (sceDrmBBMacFinal2(mac, &pos_table[n], gamekeys) < 0)
			return -5;
	}

	if (sceDrmBBCipherInit(cipher, 1, 2, sec_keys, gamekeys, pos_table[n].pos / 16) < 0)
		return -6;

	if (sceDrmBBCipherUpdate(cipher, buf1, s) < 0)
		return -7;

	if (sceDrmBBCipherFinal(cipher) < 0)
		return -8;

	if (pos_table[n].size >= (sectors_per_block * 0x800))
	{
		memcpy(buf2, buf1, pos_table[n].size);
	}
	else
	{
		return decompress(buf2, 0x100000, buf1, 0);
	}
	
	return 0x8000;
}

int NpegReadBlock(void *buf1, void *buf2, int n)
{
	u32 args[3];
	int k1 = pspSdkSetK1(0);

	args[0] = (u32)buf1;
	args[1] = (u32)buf2;
	args[2] = (u32)n;

	int res = sceKernelExtendKernelStack(0x2000, (void *)NpegReadBlock_, args);
	
	pspSdkSetK1(k1);
	return res;
}

int NpegClose()
{
	int k1 = pspSdkSetK1(0);
	
	sceIoClose(fd);
	fd = -1;

	pspSdkSetK1(k1);
	return 0;
}

int module_start(SceSize args, void *argp)
{
	return 0;
}

int module_stop()
{
	return 0;
}




