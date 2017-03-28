#include <pspsdk.h>
#include "syscon.h"
#include "sysreg.h"

void (* Dcache)(void) = (void *)0x800102d8;
void (* Icache)(void) = (void *)0x800102a0;

int (* pspNandReadPage)(u32 ppn, void *buf, void *extra) = (void *)0x80010334;
int (* pspMsReadPage)(int page, void *addr) = (void *)0x80010418;


void entry(void)
{
	u32 page;
	u8  *output;
	int i;
	u32 ctrl;
	u32 x;
	u16 *fat_ptr = (u16*)0x8001081c;

	page = 0x10+8; // First page of the rest of our code
	output = (u8 *)0x040e0000;

	for (i = 0; i < 24; i++)
	{
		pspMsReadPage(page, output);
		output += 0x200;
		page++;
	}

	
	// Hook ipl start
	*(u32 *)0x800100fc = 0x3c19040e; // lui t9, 0x040e
	*(u32 *)0x80010104 = 0; 

	*(u32 *)(0x80010264) = 0x24840030;
	// Nullify ms init
	*(u32 *)(0x80010240) = 0x03e00008; // jr ra


	Dcache();
	Icache();	

	// return to PRE-IPL
}
