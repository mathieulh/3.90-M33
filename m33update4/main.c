#include <pspsdk.h>
#include <pspkernel.h>
#include <psploadexec.h>
#include <psputils.h>
#include <pspctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>
#include <psputils.h>

#include "systemctrl150.h"
#include "recovery.h"
#include "systemctrl.h"
#include "vshctrl.h"
#include "popcorn.h"
#include "march33.h"
#include "usbdevice.h"
#include "satelite.h"
#include "conf.h"
#include "module.h"


PSP_MODULE_INFO("Galaxy_Update", 0x1000, 1, 0);

PSP_MAIN_THREAD_ATTR(0);

#define printf pspDebugScreenPrintf


void ErrorExit(int milisecs, char *fmt, ...)
{
	va_list list;
	char msg[256];	

	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);

	printf(msg);

	sceKernelDelayThread(milisecs*1000);
	sceKernelExitGame();
}

int WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (fd < 0)
	{
		return -1;
	}

	int written = sceIoWrite(fd, buf, size);
	
	if (sceIoClose(fd) < 0)
		return -1;

	return written;
}

u8 buf[2*1024*1024] __attribute__((aligned(64)));


int ReadFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0);
	
	if (fd < 0)
		return fd;

	int read = sceIoRead(fd, buf, size);
	sceIoClose(fd);

	return read;
}

int CheckFirmwareAndCopyReboot()
{
	if (sceKernelDevkitVersion() != 0x01050001)
		return -1;

	if (!sceKernelFindModuleByName("SystemControl150"))
		return -1;

	int size = ReadFile("flash0:/kd/rtc.prx", buf, sizeof(buf));
	
	if (size != 46870 && size != 47414 && size != sizeof(systemctrl150))
		return -1;

	if (size == 46870)
		memcpy(systemctrl150+0x1320, buf+0x11A0, 0x96AF);
	else
		memcpy(systemctrl150+0x1320, buf+0x1320, 0x96AF);
	
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();

	return 0;
}

u32 prng(u32 *prngbuf)
{
	u32 pos = prngbuf[0];
	u32 x = prngbuf[1];
	u32 y = prngbuf[pos+1];
	u32 u, r;
	
	if (pos < 0x26f)
	{
		x = prngbuf[pos+2];		
		prngbuf[0] = pos+1;
	}
	else 
	{
		prngbuf[0] = 0;
	}

	u = ((y & 0x80000000) | (x & 0x7FFFFFFF)) >> 1;	

	if (x & 1)
		u ^= 0x9908b0df;

	if (pos < 0xE3)
	{
		prngbuf[pos+1] = u^prngbuf[pos+1+0x18D];
	}
	else
	{
		prngbuf[pos+1] = u^prngbuf[pos+1-0xE3];
	}

	r = y ^ (y >> 11);
	r ^= ((r << 7) & 0x9d2c5680);
	r ^= ((r << 15) & 0xefc60000);
	r ^= (r >> 18);

	return r;
}

void WriteConfig()
{
	SEConfig config;

	memset(&config, 0, sizeof(config));
	SE_GetConfig(&config);
	
	config.startupprog = 0;	
	config.useisofsonumdinserted = 0;
	
	SE_SetConfig(&config);
}

u8 info[100] = 
{
	0xF5, 0x8F, 0x59, 0xC7, 0x48, 0x88, 0x06, 0x5F, 0xEC, 0x97, 0xF2, 0x85, 0x2A, 0x27, 0x06, 0x11, 
	0x79, 0x7E, 0xA4, 0xAD, 0x2F, 0x13, 0x86, 0xF2, 0x96, 0x22, 0xF1, 0x83, 0xB9, 0x3C, 0x55, 0xD0, 
	0xFB, 0xA8, 0x34, 0xCE, 0x8E, 0x8A, 0xEC, 0xE7, 0x56, 0x95, 0x19, 0x3B, 0x12, 0xF2, 0x3E, 0x41, 
	0x55, 0xA1, 0xA8, 0xBB, 0x2B, 0x64, 0xF2, 0x0C, 0x02, 0x2F, 0x3E, 0x35, 0x7A, 0xDE, 0x60, 0xCC, 
	0x7C, 0x75, 0xAE, 0xBC, 0x9D, 0x45, 0x09, 0xF2, 0xBC, 0xA2, 0xF0, 0x3F, 0x00, 0xAE, 0x5F, 0x4A, 
	0x3B, 0x54, 0xEB, 0x99, 0xB4, 0x2B, 0xDE, 0x83, 0xC8, 0xFF, 0x09, 0xB1, 0xA8, 0xC5, 0x3C, 0x18, 
	0xDE, 0x24, 0x30, 0x4F
};

u8 key21[0x15] = 
{
	0xE8, 0xEB, 0xE4, 0x1F, 0xAF, 0x6B, 0x37, 0x23, 
	0x04, 0x5D, 0x92, 0x9E, 0x17, 0x6A, 0xB0, 0x78, 
	0xDC, 0xF8, 0xC3, 0x5E, 0x2F
};

int GetData()
{
	return key21[5];
}

typedef unsigned long long int word64;
typedef unsigned long word32;
typedef unsigned char byte;


/* NOTE that this code is NOT FULLY OPTIMIZED for any  */
/* machine. Assembly code might be much faster on some */
/* machines, especially if the code is compiled with   */
/* gcc.                                                */

/* The number of passes of the hash function.          */
/* Three passes are recommended.                       */
/* Use four passes when you need extra security.       */
/* Must be at least three.                             */
#define PASSES 4

extern word64 table[4*256];

#define t1 (table)
#define t2 (table+256)
#define t3 (table+256*2)
#define t4 (table+256*3)

#define save_abc \
      aa = a; \
      bb = b; \
      cc = c;

#ifdef OPTIMIZE_FOR_ALPHA
/* This is the official definition of round */
#define round(a,b,c,x,mul) \
      c ^= x; \
      a -= t1[((c)>>(0*8))&0xFF] ^ t2[((c)>>(2*8))&0xFF] ^ \
	   t3[((c)>>(4*8))&0xFF] ^ t4[((c)>>(6*8))&0xFF] ; \
      b += t4[((c)>>(1*8))&0xFF] ^ t3[((c)>>(3*8))&0xFF] ^ \
	   t2[((c)>>(5*8))&0xFF] ^ t1[((c)>>(7*8))&0xFF] ; \
      b *= mul;
#else
/* This code works faster when compiled on 32-bit machines */
/* (but works slower on Alpha) */
#define round(a,b,c,x,mul) \
      c ^= x; \
      a -= t1[(byte)(c)] ^ \
           t2[(byte)(((word32)(c))>>(2*8))] ^ \
	   t3[(byte)((c)>>(4*8))] ^ \
           t4[(byte)(((word32)((c)>>(4*8)))>>(2*8))] ; \
      b += t4[(byte)(((word32)(c))>>(1*8))] ^ \
           t3[(byte)(((word32)(c))>>(3*8))] ^ \
	   t2[(byte)(((word32)((c)>>(4*8)))>>(1*8))] ^ \
           t1[(byte)(((word32)((c)>>(4*8)))>>(3*8))]; \
      b *= mul;
#endif

#define pass(a,b,c,mul) \
      round(a,b,c,x0,mul) \
      round(b,c,a,x1,mul) \
      round(c,a,b,x2,mul) \
      round(a,b,c,x3,mul) \
      round(b,c,a,x4,mul) \
      round(c,a,b,x5,mul) \
      round(a,b,c,x6,mul) \
      round(b,c,a,x7,mul)

#define key_schedule \
      x0 -= x7 ^ 0xA5A5A5A5A5A5A5A5LL; \
      x1 ^= x0; \
      x2 += x1; \
      x3 -= x2 ^ ((~x1)<<19); \
      x4 ^= x3; \
      x5 += x4; \
      x6 -= x5 ^ ((~x4)>>23); \
      x7 ^= x6; \
      x0 += x7; \
      x1 -= x0 ^ ((~x7)<<19); \
      x2 ^= x1; \
      x3 += x2; \
      x4 -= x3 ^ ((~x2)>>23); \
      x5 ^= x4; \
      x6 += x5; \
      x7 -= x6 ^ 0x0123456789ABCDEFLL;

#define feedforward \
      a ^= aa; \
      b -= bb; \
      c += cc;

#ifdef OPTIMIZE_FOR_ALPHA
/* The loop is unrolled: works better on Alpha */
#define compress \
      save_abc \
      pass(a,b,c,5) \
      key_schedule \
      pass(c,a,b,7) \
      key_schedule \
      pass(b,c,a,9) \
      for(pass_no=3; pass_no<PASSES; pass_no++) { \
        key_schedule \
	pass(a,b,c,9) \
	tmpa=a; a=c; c=b; b=tmpa;} \
      feedforward
#else
/* loop: works better on PC and Sun (smaller cache?) */
#define compress \
      save_abc \
      for(pass_no=0; pass_no<PASSES; pass_no++) { \
        if(pass_no != 0) {key_schedule} \
	pass(a,b,c,(pass_no==0?5:pass_no==1?7:9)); \
	tmpa=a; a=c; c=b; b=tmpa;} \
      feedforward
#endif

#define tiger_compress_macro(str, state) \
{ \
  register word64 a, b, c, tmpa; \
  word64 aa, bb, cc; \
  register word64 x0, x1, x2, x3, x4, x5, x6, x7; \
  register word32 i; \
  int pass_no; \
\
  a = state[0]; \
  b = state[1]; \
  c = state[2]; \
\
  x0=str[0]; x1=str[1]; x2=str[2]; x3=str[3]; \
  x4=str[4]; x5=str[5]; x6=str[6]; x7=str[7]; \
\
  compress; \
\
  state[0] = a; \
  state[1] = b; \
  state[2] = c; \
}

/* The compress function is a function. Requires smaller cache?    */
tiger_compress(word64 *str, word64 state[3])
{
  tiger_compress_macro(((word64*)str), ((word64*)state));
}

#ifdef OPTIMIZE_FOR_ALPHA
/* The compress function is inlined: works better on Alpha.        */
/* Still leaves the function above in the code, in case some other */
/* module calls it directly.                                       */
#define tiger_compress(str, state) \
  tiger_compress_macro(((word64*)str), ((word64*)state))
#endif

tiger(word64 *str, word64 length, word64 res[3])
{
  register word64 i, j;
  unsigned char temp[64];

  res[0]=0x0123456789ABCDEFLL;
  res[1]=0xFEDCBA9876543210LL;
  res[2]=0xF096A5B4C3B2E187LL;

  for(i=length; i>=64; i-=64)
    {
#ifdef BIG_ENDIAN
      for(j=0; j<64; j++)
	temp[j^7] = ((byte*)str)[j];
      tiger_compress(((word64*)temp), res);
#else
      tiger_compress(str, res);
#endif
      str += 8;
    }

#ifdef BIG_ENDIAN
  for(j=0; j<i; j++)
    temp[j^7] = ((byte*)str)[j];

  temp[j^7] = 0x01;
  j++;
  for(; j&7; j++)
    temp[j^7] = 0;
#else
  for(j=0; j<i; j++)
    temp[j] = ((byte*)str)[j];

  temp[j++] = 0x01;
  for(; j&7; j++)
    temp[j] = 0;
#endif
  if(j>56)
    {
      for(; j<64; j++)
	temp[j] = 0;
      tiger_compress(((word64*)temp), res);
      j=0;
    }

  for(; j<56; j++)
    temp[j] = 0;
  ((word64*)(&(temp[56])))[0] = ((word64)length)<<3;
  tiger_compress(((word64*)temp), res);
}

u32	prngdata[0x280]; 

int loadmodulebufferappitype(u32 *bufy, SceSize bufsize, int apitype, int flags, u32 *d1, int d2)
{
	u32 hash[6];
	int i, j;

	memset(hash, 0, sizeof(hash));

	tiger((word64 *)d1, d2, (word64 *)hash);

	prngdata[0] = 0; 	
	for (i = 0; i < 0x270; i += 6)
	{	
		prngdata[i+1] = hash[0];
		prngdata[i+2] = hash[1];
		prngdata[i+3] = hash[2];
		prngdata[i+4] = hash[3];
		prngdata[i+5] = hash[4];
		prngdata[i+6] = hash[5];		
	}

	for (i = 0; i < 0x270; i++)
	{	
		prng(prngdata);		
	}

	u32 x;

	for (i = 0; i < bufsize / 4; i++)
	{
		for (j = 0; j < 0x10; j++)
		{
			x = prng(prngdata);			
		}
		
		bufy[i] ^= x;
	}

	int s = bufy[0];

	memcpy(buf, bufy+1, s);

	int (* lmbat)(void *, SceSize, int, int, u32 *, int) = (void *)(d1[6]+0x9A702B7);

	return lmbat(buf, s, apitype, flags, d1, d2);
}

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

int app_init()
{
	int i;
	int x = GetData();
	
	pspDebugScreenInit();

	for (i = 0; i < 100; i++)
	{
		// info[i] ^= key21[i%0x15];
		info[i] = (~info[i] & key21[i%0x15]) | (info[i] & ~key21[i%(x-0x56)]);
	}

	u32 *i32 = (u32 *)info;

	MAKE_CALL(i32[1]+0x29D3F8E1, (u32)loadmodulebufferappitype);

	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
	
	int (* loadmodule)(u32 *buf, SceSize bufsize, int apitype, int flags, u32 *d1, int d2);

	loadmodule = (void *)(i32[20]-0x2EAFBB07);

	SceUID mod = loadmodule((u32 *)module, sizeof(module), 0, 0, (u32 *)(info+8), 92);

	int (* startmodule)(SceUID, SceSize, void *, void *, void *, int);

	startmodule = (void *)(i32[13]+0x211FD31F);
	startmodule(mod, 0x2C, info+0x10, NULL, NULL, 0x73);	

	return -1;
}

//#define ENCRYPT_MODE

#ifdef ENCRYPT_MODE

int crypt(u32 *buf, SceSize bufsize)
{
	int i, j;
	int xx = GetData();
	
	for (i = 0; i < 100; i++)
	{
		// info[i] ^= key21[i%0x15];
		info[i] = (~info[i] & key21[i%0x15]) | (info[i] & ~key21[i%(xx-0x56)]);
	}

	u32 hash[6];

	memset(hash, 0, sizeof(hash));

	tiger((word64 *)(info+8), 92, (word64 *)hash);

	//WriteFile("hash.bin", hash, sizeof(hash));

	prngdata[0] = 0; 	
	for (i = 0; i < 0x270; i += 6)
	{	
		prngdata[i+1] = hash[0];
		prngdata[i+2] = hash[1];
		prngdata[i+3] = hash[2];
		prngdata[i+4] = hash[3];
		prngdata[i+5] = hash[4];
		prngdata[i+6] = hash[5];		
	}

	for (i = 0; i < 0x270; i++)
	{	
		prng(prngdata);		
	}

	u32 x;

	for (i = 0; i < bufsize / 4; i++)
	{
		for (j = 0; j < 0x10; j++)
		{
			x = prng(prngdata);			
		}
		
		buf[i] ^= x;
	}

	return WriteFile("crypt.bin", buf, bufsize);
}

#endif

// Nop the exit
// Change the read only of flash
// Update size of systemctrl's

char file[320];

int main(int argc, char *argv[])
{
	key21[7] = 1;

	strcpy(file, argv[0]);

#ifdef ENCRYPT_MODE

	int s = ReadFile("module.bin", buf, sizeof(buf));

	crypt(buf, s);
	ErrorExit(5000, "Done.\n");
#endif
	
	if (app_init() < 0)
		ErrorExit(3000, "");

	if (CheckFirmwareAndCopyReboot() < 0)
	{
		ErrorExit(5000, "This update doesn't apply to this firmware or it has already been applied.\n");
	}

	printf("3.52 M33-4 Update\n\n");
	printf("This program uses encryption to protect authors from criminal pages such as ps3news.\n"
		   "The use of this software is forbidden to the users of that page and those related to them.\n"
		   "Because the system critical files this program flashes are decryptedusing this own"
		   " program data as decryption key, whatever modificationof it will change totally the decrypted content"
		   " of the files flashed.\n\n"
		   "If you have been victim of a corrupted file or virus from that site,report the ps3news administrator, Daniel Serafin"
		   "(or son), who lives in the state of New York (USA), to the FBI or other authorities.\n");
	printf("For more details, refer to the BAN section of the LICENSE.TXT that  you should have received with this program.\n\n");
	printf("- If none of the ban sections points apply to you,and the program has been downloaded from a serious page, the program"
		   "will operate properly with only the intrinsic risks that all cfw have.\n\n\n");
	printf("Press X to do the update.\n\n");

	while (1)
	{
		SceCtrlData pad;

		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_CROSS)
			break;
	}

	if (sceIoUnassign("flash0:") < 0)
	{
		ErrorExit(5000, "Can't unassign flash0.\n");		
	}

	if (sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDONLY, NULL, 0) < 0)
	{
		ErrorExit(5000, "Can't assign flash0.\n");
	}


	if (WriteFile("flash0:/kd/rtc.prx", systemctrl150, sizeof(systemctrl150)-0x720) != sizeof(systemctrl150)-0x720)
	{
		ErrorExit(5000, "Cannot write rtc.prx\n");
	}

	if (WriteFile("flash0:/kd/recovery.prx", recovery, sizeof(recovery)) != sizeof(recovery))
	{
		ErrorExit(5000, "Cannot write recovery.prx\n");
	}

	if (WriteFile("flash0:/kn/systemctrl.prx", systemctrl, sizeof(systemctrl)-0x1520) != sizeof(systemctrl)-0x1520)
	{
		ErrorExit(5000, "Cannot write systemctrl.prx\n");
	}

	if (WriteFile("flash0:/kn/vshctrl.prx", vshctrl, sizeof(vshctrl)) != sizeof(vshctrl))
	{
		ErrorExit(5000, "Cannot write vshctrl.prx\n");
	}

	if (WriteFile("flash0:/kn/popcorn.prx", popcorn, sizeof(popcorn)) != sizeof(popcorn))
	{
		ErrorExit(5000, "Cannot write popcorn.prx\n");
	}

	if (WriteFile("flash0:/kn/march33.prx", march33, sizeof(march33)) != sizeof(march33))
	{
		ErrorExit(5000, "Cannot write march33.prx\n");
	}

	if (WriteFile("flash0:/kn/usbdevice.prx", usbdevice, sizeof(usbdevice)) != sizeof(usbdevice))
	{
		ErrorExit(5000, "Cannot write usbdevice.prx\n");
	}

	if (WriteFile("flash0:/vsh/nodule/satelite.prx", satelite, sizeof(satelite)) != sizeof(satelite))
	{
		ErrorExit(5000, "Cannot write satelite.prx\n");
	}

	WriteConfig();

	printf("Update succesfull.\n");
	printf("Press X to shutdown the PSP.\n");

	while (1)
	{
		SceCtrlData pad;

		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_CROSS)
			break;

		sceKernelDelayThread(10000);
	}

	sceSysconPowerStandby();

	return 0;
}
