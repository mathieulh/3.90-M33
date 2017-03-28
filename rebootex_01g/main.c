#include <pspsdk.h>
#include <psp_uart.h>

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000

#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a); 
#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a); 

//#define CHANGE_FUNC(a, f) _sw(J_OPCODE | (((u32)(f) & 0x3FFFFFFF) >> 2), a); _sw(0, a+4);

int (* Reboot)(void *a0, void *a1, void *a2, void *a3) = (void *)0x88600000;
int (* sceBootLfatOpen)(char *file) = (void *)0x88607dd0;
int (* DcacheClear)(void) = (void *)0x88602900;
int (* IcacheClear)(void) = (void *)0x886022cc;

#ifdef ALLOW_PLAIN_PRX
int (* sceKernelCheckExecFile)(void *buf, int *check);
#endif

int (* KDecrypt)(u32 *buf, int size, int *retSize);
int (* UnsignCheck)(void *buf, int size);

#define OE_TAG_REBOOT_371 0xB178738A

int my_memcmp(char *m1, char *m2, int size)
{
	int i;

	for (i = 0; i < size; i++)
	{
		if (m1[i] != m2[i])
			return m2[i] - m1[i];
	}

	return 0;
}

void my_memcpy(char *m1, char *m2, int size)
{
	int i;

	for (i = 0; i < size; i++)
	{
		m1[i] = m2[i];			
	}
}

void ClearCaches()
{
	DcacheClear();
	IcacheClear();
}

char g_file[64];

int sceBootLfatOpenPatched(char *file)
{	
	// Copy to other buffer to avoid changing the string
	my_memcpy(g_file, file, 64);
	
	if (my_memcmp(g_file+4, "pspbtcnf.bin", 12) == 0)
	{
		if (_lw(0x88fb00c0) == 1)
			g_file[9] = 'k';
		else if (_lw(0x88fb00c0) == 2)
			g_file[9] = 'l';
		else
			g_file[9] = 'j';
	}

	return sceBootLfatOpen(g_file);	
}

#ifdef ALLOW_PLAIN_PRX

int sceKernelCheckExecFilePatched(void *buf, int *check)
{
	int res;
	int isPlain;
	int index;	
	
	isPlain = (((u32 *)buf)[0] == 0x464C457F); // ELF 		
		
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

#endif

int KDecryptPatched(u32 *buf, int size, int *retSize)
{
	if (buf[0x130/4] == OE_TAG_REBOOT_371)
	{
		*retSize = buf[0xB0/4];				
		my_memcpy((char *)buf, ((char *)buf)+0x150, *retSize);			
			
		return 0;
	}

	return KDecrypt(buf, size, retSize);
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

int PatchLoadCore(void *a0, void *a1, void *a2, int (* module_start)(void *, void *, void *))
{
	u32 text_addr = ((u32)module_start) - 0x0B34;	

#ifdef ALLOW_PLAIN_PRX

	// Patch calls to sceKernelCheckExecFile */
	MAKE_CALL(text_addr+0x1560, sceKernelCheckExecFilePatched);
	MAKE_CALL(text_addr+0x15B0, sceKernelCheckExecFilePatched);
	MAKE_CALL(text_addr+0x49C8, sceKernelCheckExecFilePatched);
	
	sceKernelCheckExecFile = (void *)(text_addr+0x42F8);

#endif
	
	MAKE_CALL(text_addr+0x41BC, KDecryptPatched);
	MAKE_CALL(text_addr+0x68C0, KDecryptPatched);
	KDecrypt = (void *)(text_addr+0x83F0);

	MAKE_CALL(text_addr+0x68E4, UnsignCheckPatched);
	MAKE_CALL(text_addr+0x6914, UnsignCheckPatched);
	MAKE_CALL(text_addr+0x69AC, UnsignCheckPatched);
	UnsignCheck = (void *)(text_addr+0x8400);

	ClearCaches();	
	
	return module_start(a0, a1, a2);
}

int entry(void *a0, void *a1, void *a2, void *a3)
{
	/* Patch sceBootLfatOpen call */
	MAKE_CALL(0x886000a4, sceBootLfatOpenPatched);

	/* Patch sceKernelCheckPspConfig to enable plain config */
	// sw $a0, 0($sp) -> sw $a1, 0($sp) 
	// addiu v1, zero, $ffff -> addi	$v1, $a1, 0x0000
	// return -1 -> return a1 (size)
	_sw(0xafa50000, 0x886043b8);	
	_sw(0x20a30000, 0x886043bc);

	/* patch point : removeByDebugSecion */		
	_sw(0x03e00008, 0x886010d8);
	_sw(0x24020001, 0x886010dc);
		
	// patch init error
	_sw(0, 0x8860009c);

	/* Patch the call to LoadCore module_start */
	// 88604278: jr $s1 -> mov $a3, $s1 /* a3 = LoadCore module_start */
	// 8860427c: mov $sp, $s5 -> j PatchLoadCore352
	// 88604280: nop -> mov $sp, $s5
	_sw(0x00113821, 0x88604278);
	MAKE_JUMP(0x8860427c, PatchLoadCore);
	_sw(0x02a0e821, 0x88604280);		

	// Remove check of return code from the check of the hash in pspbtcnf.bin
	_sw(0, 0x886060e4);

	/* Patch nand decryption */
	_sh(0xac60, 0x8860f726); 

	ClearCaches();

	return Reboot(a0, a1, a2, a3);	
}


