#include <pspsdk.h>
#include "pspbtcnf_recovery.h"
#include "seed.h"
#include "syscon.h"

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000

#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a); 
#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a); 

int (* Ipl_Payload)(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2) = (void *)0x88600000;
int (* sceBootLfatOpen)(char *file) = (void *)0x88608990;
int (* sceBootLfatRead)(void *buf, int size) = (void *)0x88608b04;
int (* sceBootLfatClose)(void) = (void *)0x88608aa8;
int (* DcacheClear)(void) = (void *)0x8860609c;
int (* IcacheClear)(void) = (void *)0x88605a68;

int recovery = 0;

#ifdef ALLOW_PLAIN_PRX
int (* sceKernelCheckExecFile)(void *buf, int *check);
#endif

int (* KDecrypt)(u32 *buf, int size, int *retSize);
int (* UnsignCheck)(void *buf, int size);

#define OE_TAG_REBOOT_371	0x52C5FCF0

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

void my_memset(char *m, char ch, int size)
{
	int i;

	for (i = 0; i < size; i++)
		m[i] = ch;
}

void ClearCaches()
{
	DcacheClear();
	IcacheClear();
}

char g_file[64];
int recovery_file = 0;

int sceBootLfatOpenPatched(char *file)
{	
	// Copy to other buffer to avoid changing the string
	my_memcpy(g_file, file, 64);
	
	if (my_memcmp(g_file+4, "pspbtcnf_02g.bin", 16) == 0)
	{
		if (recovery)
		{
			recovery_file = 1;
			return 0;
		}
		
		g_file[9] = 'j';
	}

	return sceBootLfatOpen(g_file);	
}

int sceBootLfatReadPatched(void *buf, int size)
{
	if (recovery_file)
	{
		my_memcpy(buf, (char *)pspbtcnf_recovery, sizeof(pspbtcnf_recovery));
		return sizeof(pspbtcnf_recovery);
	}

	return sceBootLfatRead(buf, size);
}

int sceBootLfatClosePatched()
{
	if (recovery_file)
	{
		recovery_file = 0;
		return 0;
	}

	return sceBootLfatClose();
}

#ifdef ALLOW_PLAIN_PRX

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

int entry(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2)
{	
	recovery = 0; 
	u32 ctrl = _lw(0x88fb0000);

	if ((ctrl & (SYSCON_CTRL_ALLOW_UP | SYSCON_CTRL_HOME | SYSCON_CTRL_TRIANGLE)) == 0)
	{
		goto EXECUTE_PAYLOAD;
	}

	if((ctrl & SYSCON_CTRL_RTRG) == 0)
	{
		recovery = 1;
	}
	
	// Patch sceBootLfatOpen, scebootLfatRead and sceBooLfatClose calls 
	MAKE_CALL(0x88602354, sceBootLfatOpenPatched);
	MAKE_CALL(0x886023bc, sceBootLfatReadPatched);
	MAKE_CALL(0x886023dc, sceBootLfatClosePatched);

	// Two patches during file read to avoid possible fake recovery file error
	_sw(0, 0x8860239c);
	_sw(0, 0x886023ac);

	// Patch sceKernelCheckPspConfig to enable plain config 
	// sw $a0, 0($sp) -> sw $a1, 0($sp) 
	// addiu v1, zero, $ffff -> addi	$v1, $a1, 0x0000
	// return -1 -> return a1 (size)
	_sw(0xafa50000, 0x88602610);	
	_sw(0x20a30000, 0x88602614);
	
	// patch point : removeByDebugSecion 
	// Dummy a function to make it return 1 
	_sw(0x03e00008, 0x88604ad4);
	_sw(0x24020001, 0x88604ad8);
	
	// patch init error
	_sw(0, 0x8860234c);

	// Patch the call to LoadCore module_start 
	// 886016fc: jr $t7 -> mov $a3, $t7 // a3 = LoadCore module_start 
	// 88601700: mov $sp, $s4 -> j PatchLoadCore
	// 88601704: nop -> mov $sp, $s4
	_sw(0x000f3821, 0x886016fc);
	MAKE_JUMP(0x88601700, PatchLoadCore);
	_sw(0x0280e821, 0x88601704);
	
	// Remove check of return code from the check of the hash in pspbtcnf.bin
	_sw(0, 0x88601c98);

	// Clear Systemctrl vars
	my_memset((void *)0x88fb0000, 0, 0x100);

	my_memcpy((void *)0xbfc00200, seed, 0x40);

	ClearCaches();		

EXECUTE_PAYLOAD:	

	return Ipl_Payload(a0, a1, a2, a3, t0, t1, t2);	
}

