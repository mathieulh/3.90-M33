#include <pspsdk.h>
#include <psp_uart.h>

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000

#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a); 
#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a); 

//#define CHANGE_FUNC(a, f) _sw(J_OPCODE | (((u32)(f) & 0x3FFFFFFF) >> 2), a); _sw(0, a+4);

int (* Reboot)(void *a0, void *a1, void *a2, void *a3) = (void *)0x88600000;
int (* Kprintf)(const char *format, ...) = (void *)0x88600d4c;
int (* sceBootLfatOpen)(char *file) = (void *)0x8860740c;
int (* UnsignCheck)(void *buf, int size) = (void *)0x8860386c;
int (* DcacheClear351)(void) = (void *)0x886027C4;
int (* IcacheClear351)(void) = (void *)0x88602190;
int (* DcacheClear150)(void) = (void *)0x88c02c64;
int (* IcacheClear150)(void) = (void *)0x88c02c90;
int (* sceKernelCheckExecFile)(void *buf, int *check);
int (* KDecrypt)(u32 *buf, int size, int *retSize);

#define XD 0xF1

static const u8 key_data_351[0x10] = 
{ 
	0x3B^XD, 0x9B^XD, 0x1A^XD, 0x56^XD, 0x21^XD, 0x80^XD, 0x14^XD, 0xED^XD,
	0x8E^XD, 0x8B^XD, 0x08^XD, 0x42^XD, 0xFA^XD, 0x2C^XD, 0xDC^XD, 0x3A^XD
};

static void buld_key_param(const u8 *key_real)
{
	int i;
	u8 *dst  = (u8 *)(0xbfc00200 - 0x22844F0D); // KEY param
	u8 *src  = (u8 *)(0x8860fed0 - 0x22844F0D); // KEY seed
	// key seed
	for(i=0;i<0x10;i++)
	{
		dst[0x22844F0D+i] = src[0x22844F0D+i] ^ key_real[i] ^ XD;		
	}
}

int memcmp(char *m1, char *m2, int size)
{
	int i;

	for (i = 0; i < size; i++)
	{
		if (m1[i] != m2[i])
			return m2[i] - m1[i];
	}

	return 0;
}

void memcpy(char *m1, char *m2, int size)
{
	int i;

	for (i = 0; i < size; i++)
	{
		m1[i] = m2[i];			
	}
}

void ClearCaches351()
{
	DcacheClear351();
	IcacheClear351();
}

void ClearCaches150()
{
	DcacheClear150();
	IcacheClear150();
}

char g_file[64];

int sceBootLfatOpenPatched(char *file)
{	
	// Copy to other buffer to avid changing the string
	memcpy(g_file, file, 64);
	
	if (memcmp(g_file, "/kd", 3) == 0)
	{
		g_file[2] = 'n';
	}

	else if (memcmp(g_file, "/vsh/module", 11) == 0)
	{
		g_file[5] = 'n';
	}

	return sceBootLfatOpen(g_file);	
}

int UnsignCheckPatched(u8 *buf, int size)
{
	int unsigncheck = 0, i;

	for (i = 0; i < 0x58; i++)
	{
		if (buf[i+0xD4] != 0)
		{
			unsigncheck = 1;
			break;
		}
	}

	if (unsigncheck)
		return UnsignCheck(buf, size);

	return 0;
}

int sceKernelCheckExecFilePatched(void *buf, int *check)
{
	int res;
	int isPlain;
	int index;	
	
	isPlain = (((u32 *)buf)[0] == 0x464C457F); /* ELF */		
		
	if (isPlain) 
	{
		if (check[0x44/4] != 0)
		{
			check[0x48/4] = 1;
			return 0;
		}
	}	

	res = sceKernelCheckExecFile(buf, check);

	if (isPlain)
	{
		index = check[0x4C/4];

		if (check[0x4C/4] < 0)
		{
			index += 3;
		}			

		if ((check[8/4] == 0x20) || 
			((check[8/4] > 0x20) && (check[8/4] < 0x52)))		
		{
			if ((((u32 *)buf)[index / 4] & 0x0000FF00))
			{			
				check[0x44/4] = 1;
				check[0x58/4] = ((u32 *)buf)[index / 4] & 0xFFFF;
				return 0;
			}			
		}		
	}
	
	return res;	
}

int KDecryptPatched(u32 *buf, int size, int *retSize)
{
	if (buf[0x130/4] == 0xD30DBE7A)
	{
		*retSize = buf[0xB0/4];				
		memcpy(buf, ((u8 *)buf)+0x150, *retSize);			
			
		return 0;
	}

	return KDecrypt(buf, size, retSize);
}

int PatchLoadCore351(void *a0, void *a1, void *a2, int (* module_start)(void *, void *, void *))
{
	u32 text_addr = ((u32)module_start) - 0x0B34;	

	/* Patch calls to sceKernelCheckExecFile */
	MAKE_CALL(text_addr+0x1528, sceKernelCheckExecFilePatched);
	MAKE_CALL(text_addr+0x1578, sceKernelCheckExecFilePatched);
	MAKE_CALL(text_addr+0x4518, sceKernelCheckExecFilePatched);

	/* UnsignCheck patch stuff for modules between memlmd and systemctrl */
	_sw(0x00001021, text_addr+0x5EEC);
	_sw(0x00001021, text_addr+0x5F1C);
	_sw(0x00001021, text_addr+0x5F80);

	MAKE_CALL(text_addr+0x3E10, KDecryptPatched);
	MAKE_CALL(text_addr+0x5EC8, KDecryptPatched);
	KDecrypt = (void *)(text_addr+0x7804);

	ClearCaches351();

	sceKernelCheckExecFile = (void *)(text_addr+0x3E48);	
	
	return module_start(a0, a1, a2);
}

int PatchLoadCore150(void *a0, void *a1, int (* module_start)(void *, void *))
{
	/* No Plain Module Check Patch */
	_sw(0x340D0001, 0x880152e0);
	ClearCaches150();

	return module_start(a0, a1);
}

int entry(void *a0, void *a1, void *a2, void *a3)
{	
	if (memcmp((void *)0x88c16CB2, "19196", 5) != 0)
	{
		/* Patch sceBootLfatOpen call */
		MAKE_CALL(0x88600094, sceBootLfatOpenPatched);

		/* Patch DecryptPSP to enable plain text config */
		// sw $a2, 0($sp) -> sw $a1, 0($sp) 
		// addiu v1, zero, $ffff -> addi	$v1, $a1, 0x0000
		// return -1 -> return a1 (size)
		_sw(0xafa50000, 0x886044c0);	
		_sw(0x20a30000, 0x886044c4);

		/* patch point : removeByDebugSecion */		
		_sw(0x03e00008, 0x88600F9C);
		_sw(0x24020001, 0x88600FA0);
		
		// patch init error
		_sw(0, 0x8860008c);

		/* Patch the call to LoadCore module_start */
		// 8860425c: jr $s5 -> mov $a3, $s5 /* a3 = LoadCore module_start */
		// 88604260: mov $sp, $t0 -> j PatchLoadCore351
		// 88604264: nop -> mov $sp, $t0
		_sw(0x02a03821, 0x8860425c);
		MAKE_JUMP(0x88604260, PatchLoadCore351);
		_sw(0x0100e821, 0x88604264);		

		// Remove check of return code from the check of the hash in pspbtcnf.bin
		_sw(0, 0x886060ac);

		/* Patch nand decryption */
		/* sw $a0, 0x88618f60 -> sw $zero, 0x88618f60 */
		_sh(0xac60, 0x8860e756);

		/* Patch UnsignCheck */
		u16 low, high;

		low = ((u32)UnsignCheckPatched) & 0xFFFF;
		high = ((u32)UnsignCheckPatched) >> 16;

		_sh(high, 0x88604298);
		_sw(0x37390000 | low, 0x8860429C);
		_sh(high, 0x88603e28);
		_sw(0x34840000 | low, 0x88603e30);

		/* key generation */
		buld_key_param(key_data_351);
		ClearCaches351();
	}

	else
	{
		/* Patch the call to LoadCore module_start */
		// 88c00fec: mov $v0, $s2 -> mov $a2, $s2 (module_start)
		// 88c00ff0: mov $a0, $s5
		// 88c00ff4: mov $a1, $sp
		// 88c00ff8: jr  $v0 -> jal PatchLoadCore150
		// 88c00ffc: mov $sp, $s6
		_sw(0x02403021, 0x88c00fec);
		MAKE_CALL(0x88c00ff8, PatchLoadCore150);
		Reboot = (void *)0x88c00000;

		ClearCaches150();
	}	

	return Reboot(a0, a1, a2, a3);	
}

int main() { return 0; }
