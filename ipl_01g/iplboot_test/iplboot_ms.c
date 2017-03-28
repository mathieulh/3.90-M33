#include <pspsdk.h>
#include "syscon.h"
#include "sysreg.h"

void (* Dcache)(void) = (void *)0x800102d8;
void (* Icache)(void) = (void *)0x800102a0;

int (* pspMsReadIplBlock)(int block, void *addr) = (void *)0x80010248;
int (* pspMsReadPage)(int page, void *addr) = (void *)0x80010418;

void entry(void)
{
	u32 page;
	u8  *output;
	int i;
	u32 ctrl;
	u32 x;

	// GPIO enable ?
	REG32(0xbc100058) |= 0x02;

	// GPIO enable
	REG32(0xbc10007c) |= 0xc8;

	pspSyscon_init();
	pspSyscon_driver_Unkonow_f775bc34(&x); 

	if (!(x & 0x80))
	{
		// Not sleep mode return
		ctrl = 0xffffffff;
		pspSysconGetCtrl1(&ctrl);
		if((ctrl & SYSCON_CTRL_RTRG) == 0) // MSBOOT
		{
			*(u32 *)0x040d0000 = 0x33333333;
		}
		else if ((ctrl & (SYSCON_CTRL_ALLOW_UP | SYSCON_CTRL_HOME | SYSCON_CTRL_TRIANGLE)) == 0)
		{
			goto RETURN_PRE_IPL;
		}

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
	}

RETURN_PRE_IPL:
	
	// Move ms ipl start 4 ipl blocks (0x20 pages) ahead
	*(u32 *)(0x80010264) = 0x24840030;
	// Nullify ms init
	*(u32 *)(0x80010240) = 0x03e00008; // jr ra

	Dcache();
	Icache();	

	// return to PRE-IPL
}
