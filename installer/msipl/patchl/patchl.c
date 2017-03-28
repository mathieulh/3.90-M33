#include <pspsdk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fat.h"

int (* pspMsReadSector)(int sector, void *buf) = (void *)0x80010418;

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000
#define SC_OPCODE	0x0000000C
#define JR_RA		0x03e00008

#define NOP	0x00000000

#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a); 
#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a); 
#define MAKE_SYSCALL(a, n) _sw(SC_OPCODE | (n << 6), a);
#define JUMP_TARGET(x) (0x80000000 | ((x & 0x03FFFFFF) << 2))

#define REDIRECT_FUNCTION(a, f) _sw(J_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a);  _sw(NOP, a+4);
#define MAKE_DUMMY_FUNCTION0(a) _sw(0x03e00008, a); _sw(0x00001021, a+4);
#define MAKE_DUMMY_FUNCTION1(a) _sw(0x03e00008, a); _sw(0x24020001, a+4);

int pspMsReadSectorPatched(int sector, void *buf)
{
	memcpy(buf, (void *)(0x04180000+((sector-0x10)*0x200)), 0x200);
	
	return 0;
}

void entry(void)
{	
	u32 addr;
	int read;
	u8 *output = (u8 *)0x04180000;
	
	MsFatMount();
	MsFatOpen("/msipl.bin");

	while ((read = MsFatRead(output, 0x8000)) > 0)
	{
		output += read;
	}

	MsFatClose();	
	// Redirect pspMsReadSector
	addr = (u32)pspMsReadSectorPatched;
	*(u32 *)0x80010418 = 0x3c020000 | (addr >> 16);
	*(u32 *)0x8001041C = 0x34420000 | (addr & 0xFFFF);
	*(u32 *)0x80010420 = 0x00400008;
	*(u32 *)0x80010424 = 0;	
}
