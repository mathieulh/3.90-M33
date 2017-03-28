
#include <pspkernel.h>
#include <pspdebug.h>
#include <stdlib.h>
#include <string.h>

/* Define the module info section */
PSP_MODULE_INFO("CheckExec", 0x1000, 1, 0);

/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR(0);

/* Define printf, just to make typing easier */
#define printf	pspDebugScreenPrintf

int sceKernelCheckExecFile(void *ptr, void *check);

int patchedCheckExec(void *ptr, void *check)
{
	*(unsigned short *)(check+0x5a) = 0;

	int rc = sceKernelCheckExecFile(ptr, check);
	
	/* Check if we managed to decrypt the file */
	if(*(unsigned short *)(check+0x5a) & 1)
	{
		return rc;
	}
	else
	{
		unsigned int outputBuffer = *(unsigned int*)(check+0x24);

		// This seems to be constant for using prx'es in v1.5

		// Lets fake some code to make it look like we have succeeded
		// then we just need to copy across the data to output buffer
		*(unsigned char *)(check+0x20) = 0x00000001;
		*(unsigned char *)(check+0x48) = 0x00000001; 
		*(unsigned char *)(check+0x54) = 0x00000001;

		*(unsigned int*)(check+0x1c) = outputBuffer;
		*(unsigned int*)(check+0x24) = 0;

		// version codes in one of the following maybe???
		*(unsigned int*)(check+0x10) = 0x00001a0c;	
		*(unsigned int*)(check+0x28) = 0x0000fef0;
		*(unsigned int*)(check+0x3c) = 0x000004a0;
		*(unsigned int*)(check+0x4c) = 0x00002900;

		*(unsigned int*)(check+0x58) = 0x0001124c;

		*(unsigned int*)(check+0x58c) = 12345; // how do we get the size

		if(outputBuffer)
		{
//			memcpy(
		}

		return 1;
	}
}



int main(void)
{
	// Need to store the address of the checkexec into 
	// Check exec call location to change
	// v2.6
	mem = 0x8801a210;
#if 0
	// v2.5
	mem = 0x88016ca0;
#endif

	sceKernelCheckExecProc = *mem;
	// need to patchedCheckExec;
	// *mem = sort out jump to patchedcheckexec
}
