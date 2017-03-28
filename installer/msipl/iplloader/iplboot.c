#include <pspsdk.h>


void (* Dcache)(void) = (void *)0x800102d8;
void (* Icache)(void) = (void *)0x800102a0;
int (* pspMsReadSector)(int sector, void *buf) = (void *)0x80010418;

void entry(void)
{	
	u8 *output;
	int i, sector;
	void (* patch)() = (void *)0x04000000;

	sector = 0x10+8; // First sector of the rest of our code
	output = (u8 *)0x04000000;

	for (i = 0; i < 24; i++)
	{
		pspMsReadSector(sector, output);
		output += 0x200;
		sector++;
	}
	
	// Patch checksum check
	*(u32 *)(0x800100D4) =  0; 
	// Nullify ms init
	*(u32 *)(0x80010240) = 0x03e00008; // jr ra	

	Dcache();
	Icache();

	patch();

	Dcache();
	Icache();	

	// return to PRE-IPL
}
