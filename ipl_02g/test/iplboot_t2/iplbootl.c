#include <pspsdk.h>
#include "syscon.h"
#include "sysreg.h"

void (* Dcache)(void) = (void *)0x800102d8;
void (* Icache)(void) = (void *)0x800102a0;

int (* pspNandReadPage)(u32 ppn, void *buf, void *extra) = (void *)0x80010334;
//int (* pspMsReadSector)(int page, void *addr) = (void *)0x80010418;
//int (* pspMsInit)(void) = (void *)0x80010240;

void entry(void)
{
	u32 page;
	u8  *output;
	int i;
	u32 ctrl;
	u32 x;
	u16 *fat_ptr = (u16*)0x8001081c;

	page = (fat_ptr[0]*0x20)+8; // First page of the rest of our code
	output = (u8 *)0x040e0000;

	REG32(0xbc100058) |= 0x02;

	// GPIO enable
	REG32(0xbc10007c) |= 0xc8;

	pspSyscon_init();
	pspSysconCrlMsPower(1);
	pspMsWriteSector(1, output);
	
	//pspSyscon_end();

	while (1);

	for (i = 0; i < 24; i++)
	{
		pspNandReadPage(page, output, (void *)0x80010810);
		output += 0x200;
		page++;
	}

		// GPIO enable ?
	

	// Hook ipl start
	*(u32 *)0x800100fc = 0x3c19040e; // lui t9, 0x040e
	*(u32 *)0x80010104 = 0; 

	// Bypass FAT Read
	*(u32*)(0x80010154) = 0x00001021; // mov v0, zero
	
	// Move the blocks one position left for new pre-ipl execution	
	do
	{
		fat_ptr[0] = fat_ptr[1];
		fat_ptr++;
	} while(((u32)fat_ptr) < 0x8001091a);

	Dcache();
	Icache();	

	// return to PRE-IPL
}
