#include <pspsdk.h>
#include <string.h>

#include "psp_uart.h"
#include "fat.h"
#include "ms.h"
#include "syscon.h"

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000

#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a); 
#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a); 

int (* Reboot)(void *, void *, void*, u32) = (void *)0x88c00000;
int (* Kprintf)(const char *format, ...) = (void *)0x88C01BB0;
int (* Dcache)(void) = (void *)0x88C03F50;
int (* Icache)(void) = (void *)0x88C03B80;

void ClearCaches()
{
	Dcache();
	Icache();
}

u32 buf[512/4];

int check_point()
{
	buf[0] = 0x88888888;

	pspMsWriteSector(1, buf);
	return 0;
}

// bc000000
// 0000: CCCCCCCC
// 0004: CCCCCCCC
// 0008: FFFFFFFF
// 000C: FFFFFFFF
// 0038: 400
// 003C: 4
// 0044: 01FF009F/ 0FFF079F
// 0048: 0/1
// 0050: 1020244

int SysMemStart(SceSize args, void *argp, int (* module_start)(SceSize, void *))
{
	u32 text_addr = ((u32)module_start) - 0xB6A8;
	
	Kprintf("Starting sysmem...\n");

	//MAKE_CALL(text_addr+0xB8d8, check_point);
	//ClearCaches();
	
	int res = module_start(4, argp);

	/*_sw(0xCCCCCCCC, 0xbc000000);
	_sw(0xCCCCCCCC, 0xbc000004);
	_sw(0xCCCCCCCC, 0xbc000008);
	_sw(0xCCCCCCCC, 0xbc00000c);*/

	buf[0] = 0x11111111;
	buf[1] = res;
	_sw(0x01FF009F, 0xbc000044);
	_sw(0, 0xbc000048);
	_sw(0x74, 0xbc10004C);
	_sw(0x181, 0xbc100054);
	_sw(0x30070, 0xbc100060);
	_sw(1, 0xbc100064);
	_sw(0x118225A, 0xbc100078);
	_sw(0x20000FF, 0xbc10007C);
	_sw(0x100, 0xbc100080);
	_sw(0, 0xbc10008C);
	_sw(0x14320F9C, 0xbc100090);
	_sw(0x26412, 0xbc100094);
	_sw(0xD4ED674D, 0xbc10009C);
	_sw(0, 0xbc1000b0);	
	_sw(0, 0xbc1000b4);	
	_sw(0, 0xbc1000d0);
	_sw(0, 0xbc1000d4);
	_sw(0, 0xbc1000d8);
	_sw(0, 0xbc1000dC);
	pspMsWriteSector(1, 0xbc000000);

	// -e90
	MAKE_CALL(0x88c0101c, check_point);
	ClearCaches();

	return res;
}

int LoadCoreStart(SceSize args, void *argp, int (* module_start)(SceSize, void *))
{
	u32 text_addr = ((u32)module_start) - 0x0AB8;

	// NoPlainModuleCheckPatch (mfc0 $t5, $22 -> li $t5, 1)
	_sw(0x340D0001, text_addr+0x2FE0);
	ClearCaches();
	
	Kprintf("Starting loadcore...\n");

	buf[0] = 0x33333333;
	
	//pspMsWriteSector(1, buf);
	return module_start(8, argp);
}


// a0 fat/slim
// 0000: 0x88000000
// 0004: 0x02000000 / 0x04000000
// 0008: 0
// 000C: 0
// 0010: 88401040 / 8BC01040
// 0014: 8841D3C0 / 8BC1D3C0
// 0018: 2
// 001C: 5
// 0020: 8841D040 / 8BC1D040
// 0024: 0
// 0028: 1
// 002C: 0 / 1
// 0048: 89FFF800 / 8BBFF700
// 004C: 89FFF900 / 8BBFF800

// a1 fat/slim
// 0000: 24
// 0004: 29
// 0008: 08844140 / 0882BB80
// 000C: 882F08C0 / 885EF8C0
// 0010: 400
// 0014: 09C77200 / 0A0A7200
// 0020: 10000
// 0024, 0028, 002C: FFFFFFFF
// 0030: 656D6167

// a2 fat/slim 141/141 (apitype)
// a3 fat/slim 54D9CF56/DD9F44A7

// a0[0x10/4] fat/slim
// all zero

// a0[0x14/4] fat/slim
// all zero

// a0[0x20/4] fat/slim
// 0000: 89FFFB00/8BBFFB00
// 0004: 29
// 0008: 2
// 001C: 89FFFA00/8BBFFA00
// 0020: 29 / 29
// 0024: 100 / 8BBFFA00 
// 0038: 89FFFC00 / 8BBFFC00 
// 003C: 400
// 0040: 4
// 0054: 89FFF800/8BBFF700
// 0058: 70
// 005C: 20
// 0070: 89FFF900/8BBFF800
// 0074: 0/1E0
// 0078: 0/8

// a0[48/4] union a0[4C/4]
// 0000: 70
// 0004: 13110
// 0040: 1
// 0058: 30382E33 (3.80)
// 0060: ?/ 1
// 0064: 03080010 (devkit)
// 0068: 30306
// 0070: 099F5510 / 0
// 0074: 08852AE0 / 0
// 0078: 088244F0 / 0
// 007C: 0881F120 / 0
// 0080: 09B7A9C0 / 0
// 0084: 09B4D418 / 0
// 0088: 9D / 0
// 008C: 2 / 0
// 0090: 09B4BD68 / 0
// 0094: 09FFF8D0 / 0
// 009C: 0881F120 / 0
// 00A0: 09B4C85C / 0
// 00A4: 08852AE0 / 0
// 00A8: 09B4C8A4 / 0
// 00AC: 088241BC / 0
// 00B0: 09BE22DC / 0
// 00B4: 08852AE0 / 0
// 00B8: 09B4CB84 / 0
// 00BC: 0881F120 / 0
// 00C0: 099F5130 / 0
// 00C4: 09AFFDC0 / 0
// 00C8: 088244F0 / 0
// 00CC: 09AFFD28 / 0
// 00D0: 099F5510 / 0
// 00D4: 0028 / 0
// 00DC: 1 / 0
// 00E0: 20 / 0
// 00E4: 20 / 0
// 00E8: 1C0 / 0
// 00EC: 1C0 / 0
// 00F0: 1A0 / 0
// 00F4: 1A / 0
// 00F8: 34 / 0
// 00FC: 1 / 0
// 0100: 5C2F430B
// 0104: ?/1
// 0108: 80010000
// 010C: F00 / 1E00
// 0110: 90 / 01A0
// 0114: D0 / 01E0
// 0118: 10 / 18
// 011C: 10 / 18
// 0120: 10 / 18
// 0124: 10 / 18
// 0128: 10202020 / 18
// 012C: 66303200 / 18
// 0130: 38353835 / 18
// 0134: 66300000 / 18
// 0138: 78A4DB53
// 013C: C30240B0
// 0140: 100000 / 180000
// 0144: A00099 / 1A40199
// 0148: B500A4 / 1A701A6
// 014C: CE00B8 / 1AA01A9
// 0150: 011D0109 / 1B101AD
// 0154: 015E0125 / 1B701B6
// 0158: 018C0189 / 1C601C0
// 015C: 01C301A8 / 1C901C7
// 0160: 01D501D0 / 1CB01CA
// 0164: 0100001 / 1D001CD
// 0168: 020C01FE / 1D201D1
// 016C: 022A021B / 1D701D3
// 0170: 02AD029B / 1DB01D9
// 0174: 02E102AE / 180001
// 0178: 0339031A / 399037A
// 017C: 03980352 / 39F039E
// 0180: 03BC03B0 / 3A103A0
// 0184: 03DC03D3 / 3AA03A7
// 0188: 100002 / 3AE03AC
// 018C: 0442042E / 3B203B0
// 0190: 049E048E / 3B503B4
// 0194: 04CC04B0 / 3B703B6
// 0198: 04E504D9 / 3B903B8
// 019C: 05440506 / 3BB03BA
// 01A0: 055C054A / 3C103BD
// 01A4: 0582056F / 3C303C2
// 01A8: 05A60587 / 180002
// 01AC: 100003 / 5AE05A8
// 01B0: 061E0613 / 5B205B0
// 01B4: 06280627 / 5B705B4
// 01B8: 06520638 / 5BB05B9
// 01BC: 07B7073B / 5C105BD
// 01C0: 05FE05F2 / 5C305C2
// 01C4: 05E105D5 / 5C505C4
// 01C8: 05ED05E2 / 5C705C6
// 01CC: 060D060C / 5C905C8
// 01D0: 08824190 / 5CB05CA
// 01D4: 0176 / 5CD05CC
// 01D8: 088242C0 / 5CF05CE
// 01DC: 09BE0000 / 180003
// 01E0: 0/67E064C
// 01E4: 0/6A7067F
// 01E8: 18/6AA06A8
// 01EC: 09B01CCC/6D806AB
// 01F0: 0/6FD06FC
// 01F4: 1/6FF06FE
// 01F8: 1A/7030702
// 01FC: 1/7050704


// a0[0x24/4][0/4] & a0[0x24/4][0x1C/4]
// (same) ms0:/PSP/GAME150/__SCE__USBSSS/EBOOT.PBP (size 0x28+1 = 0x29)

// a0[0x24/4][0x38/4]
// 0040: 1

// bc000000
// 0000-000F: CCCCCCCC...
// 0044: 1FF009F / FFF079F
// 0048: 0/1
// 0050: 1020244 / 1020244 (same)

// bc100000
// 0008: 80FFFFFF
// 003C: 1
// 0040: 40000001 / 50000102
// 004C: 74 / 3074
// 0050: 759C
// 0054: 181 / 191
// 0058: AC0603
// 005C: 12
// 0060: 30070 / 130070
// 0064: 1 / 31
// 0068: 5
// 0070: 3
// 0074: 3
// 0078: 118225A / 318225A
// 007C: 20000FF / 20000F9
// 0080: 100 / 104
// 008C: 0/1
// 0090: 14320F9C / F8CA1212
// 0094: 26412 / 6C21
// 0098: 2400 / 2400
// 009C: D4ED674D / 800D2832
// 00B0: 0/1
// 00B4: 0/20
// 00D0: 0/333333
// 00D4: 0/333333
// 00D8: 0/333
// 00DC: 0/FFFF0000
// 00FC: 1240901/1240901


int entry(u32 *a0, u32 *a1, void *a2, u32 a3)
{	
	// jal sysmem::module_start -> jal SysMemStart
	// li  $a0, 4 -> mov $a2, $s1  (a0 = 4 -> a2 = module_start)
	MAKE_CALL(0x88c01014, SysMemStart);
	_sw(0x02203021, 0x88c01018);
	
	// mov $a0, $s5 -> mov $a2, $v0 (a0 = 8 -> a2 = module_start)
	// mov $a1, $sp
	// jr  loadcore::module_start -> j LoadCoreStart 
	// mov $sp, $t3
	_sw(0x00403021, 0x88c00ff0);
	MAKE_JUMP(0x88c00ff8, LoadCoreStart);

	/* Kprintf stuff */
	// Change default putc handler
	u32 addr = (u32)uart_dbg_putc;
	_sw(addr, 0x88c16d54);
	_sh(addr >> 16, 0x88C0023C);
	_sw(0x34840000 | (addr & 0xFFFF), 0x88c00244);
	// Patch removeByDebugSection, make it return 1	
	_sw(0x03e00008, 0x88C01D20);
	_sw(0x24020001, 0x88C01D24);

	/* File open redirection */
	// Redirect sceBootLfatfsMount to MsFatMount
	MAKE_CALL(0x88c00074, MsFatMount);
	// Redirect sceBootLfatOpen to MsFatOpen
	MAKE_CALL(0x88C00084, MsFatOpen);
	// Redirect sceBootLfatRead to MsFatRead
	MAKE_CALL(0x88C000B4, MsFatRead);
	// Redirect sceBootLfatClose to MsFatClose
	MAKE_CALL(0x88C000E0, MsFatClose);



	ClearCaches();		

	// GPIO enable ?
	REG32(0xbc100058) |= 0x02;

	// GPIO enable
	REG32(0xbc10007c) |= 0xc8;
	asm("sync"::);
	
	pspSyscon_init();
	pspSysconCrlMsPower(1);
	pspSysconCrlHpPower(1);	

	u32 *r = (u32 *)a0[0x20/4];
	
	MsFatMount();
	//pspMsWriteSector(1, 0xbc100000);

	a0[4/4] = 0x02000000;

	memcpy((char *)0x88401040, (char *)a0[0x10/4], 0x1C000); 
	a0[0x10/4] = 0x88401040;

	memcpy((char *)0x8841D3C0, (char *)a0[0x14/4], 0x50000); 
	a0[0x14/4] = 0x8841D3C0;

	memcpy((char *)0x8841D040, (char *)a0[0x20/4], 0x380); 
	a0[0x20/4] = 0x8841D040;

	/*memcpy((char *)0x89FFF800, (char *)a0[0x48/4], 0x100); 
	a0[0x48/4] = 0x89FFF800;

	memcpy((char *)0x89FFF900, (char *)a0[0x4C/4], 0x100); 
	a0[0x4C/4] = 0x89FFF900;*/

	a0[0x2C/4] = 0;

	/*memcpy((char *)0x882F08C0, (char *)a1[0x0C/4], 0x200); 
	a1[0x0C/4] = 0x882F08C0;

	memcpy((char *)0x89C77200, (char *)(0x80000000 | a1[0x14/4]), 0x200); 
	a1[0x14/4] = 0x09C77200;*/

	

	memcpy((char *)0x89FFFB00, (char *)r[0/4], 0x100); 
	r[0/4] = 0x89FFFB00;

	memcpy((char *)0x89FFFA00, (char *)r[0x1C/4], 0x100); 
	r[0x1C/4] = 0x89FFFA00;

	memcpy((char *)0x89FFFC00, (char *)r[0x38/4], 0x100); 
	r[0x38/4] = 0x89FFFC00;

	/*memcpy((char *)0x89FFFC00, (char *)r[0x54/4], 0x100); 
	r[0x54/4] = 0x89FFF800;

	memcpy((char *)0x89FFF900, (char *)r[0x70/4], 0x100); 
	r[0x70/4] = 0x89FFF900;*/

	r[0x24/4] = 0x100;
	r[0x74/4] = r[0x78/4] = 0;

	a3 = 0x54D9CF56;

	a0[0x48/4] = a0[0x4C/4] = 0;
	a1[0x08/4] = a1[0x14/4] = a1[0x0C/4] = 0;
	

	//r[0] = r[0x1C/4] = 0;
	//r[0x38/4] = r[0x54/4] = 0;
	r[0x54/4] = r[0x70/4] = 0;

	/*_sw(0x1FF009F, 0xbc000044);
	_sw(0, 0xbc000048);*/
	
	//_sw(0x50000101, 0xbc100040);
	/*_sw(0x74, 0xbc10004C);
	_sw(0x181, 0xbc100054);
	_sw(0x30070, 0xbc100060);
	_sw(1, 0xbc100064);
	_sw(0x118225A, 0xbc100078);
	_sw(0x20000FF, 0xbc10007C);
	_sw(0x100, 0xbc100080);
	_sw(0, 0xbc10008C);
	_sw(0x14320F9C, 0xbc100090);
	_sw(0x26412, 0xbc100094);
	_sw(0xD4ED674D, 0xbc10009C);
	_sw(0, 0xbc1000b0);	
	_sw(0, 0xbc1000b4);	
	_sw(0, 0xbc1000d0);
	_sw(0, 0xbc1000d4);
	_sw(0, 0xbc1000d8);
	_sw(0, 0xbc1000dC);*/
	ClearCaches();

	//pspMsWriteSector(1, 0xbc000000);
	//pspMsWriteSector(2, 0xbc100000);

	return Reboot(a0, a1, a2, a3);	
}

int main(){ return 0; }

