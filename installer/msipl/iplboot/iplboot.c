#include <pspsdk.h>
#include "syscon.h"
#include "sysreg.h"

void (* Dcache)(void) = (void *)0x800102d8;
void (* Icache)(void) = (void *)0x800102a0;

int (* pspMsReadSector)(int page, void *addr) = (void *)0x80010418;

void entry(void)
{
	u32 sector;
	u8  *output;
	int i;

	sector = 0x10+8; // First sector of the rest of our code
	output = (u8 *)0x040e0000;

	for (i = 0; i < 24; i++)
	{
		pspMsReadSector(sector, output);
		output += 0x200;
		sector++;
	}
	
	// Nullify decryption
	*(u32 *)(0x800100BC) = 0x00001021; // mov v0, zero
	// Patch checksum check
	*(u32 *)(0x800100D4) =  0; 
	// Move ms ipl start 4 ipl blocks (0x20 pages) ahead
	*(u32 *)(0x80010264) = 0x24840030;
	// Nullify ms init
	*(u32 *)(0x80010240) = 0x03e00008; // jr ra

	// GPIO enable ?
	REG32(0xbc100058) |= 0x02;

	// GPIO enable
	REG32(0xbc10007c) |= 0xc8;

	// WLAN LED ON but Can't lamp LED ...
	REG32(0xbe240000) |= 0xc8;
	REG32(0xbe240008)  = 0xc0;
	asm("sync"::);
	
	pspSyscon_init();
	pspSysconCtrlLED(0,1);
	pspSysconCtrlLED(1,1);
	pspSysconCrlMsPower(1);
	pspSysconCrlHpPower(1);	

	Dcache();
	Icache();	

	// return to PRE-IPL
}
