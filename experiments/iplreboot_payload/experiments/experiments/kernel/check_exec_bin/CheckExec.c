
#include <pspkernel.h>
#include <pspdebug.h>
#include <stdlib.h>
#include <string.h>


// PSP firmware version
#define FW_VER 250

#define JR_RA_INSN 0x03e00008
#define MOVE_V0_0_INSN 0x00001021

typedef int (sceKernelCheckExecFile_fn)(void *ptr, void *check);

sceKernelCheckExecFile_fn *original_checkexec_fn;


// func declarations
int patched_checkexec(void *ptr, void *check);

/*****************************************************************************/
/* _start is aligned at 0x883f0000, and sets up the patches                  */
/*****************************************************************************/
void _start(void)
{
	unsigned long ljumpdest;
	unsigned long linstruc;

	unsigned int *mem;

	// Patching out ioctl NID from modulemgr so that all calls to it return success
#if FW_VER==250
	// This looks like it will be the address on v2.5 (unless I have miscounted NID's)
	// If this does not work try 968 & 970
	mem = 0x8805f96c;
#endif

#if FW_VER==260
	// This is removing the ioctl NID
	mem = 0x8805fcfc;
#endif

	*mem = JR_RA_INSN;
	*(mem+1) = MOVE_V0_0_INSN;

	//  at 0x8801a210 on v2.6  contains the instruc to jump to the checkexec proc
#if FW_VER==260
	linstruc = * ((unsigned long*)0x8801a210);
#endif
#if FW_VER==250
	linstruc = * ((unsigned long*)0x88016ca0);
#endif

	ljumpdest = 0x80000000L | ((linstruc & 0x03FFFFFF) << 2);

	original_checkexec_fn = (sceKernelCheckExecFile_fn*) ljumpdest;


	// Now patch in a jump to our new func

  // jal target
  // Encoding:  0000 11ii iiii iiii iiii iiii iiii iiii

	ljumpdest = (unsigned long) patched_checkexec;
	linstruc = 0x0c000000 | ((ljumpdest & 0x0fffffff) >> 2);

#if FW_VER==260
	// It does direct calls rather than a nid table, so need to patch all cases
    * ((unsigned long*)0x8801a210) = linstruc;
	* ((unsigned long*)0x8801809c) = linstruc;
	* ((unsigned long*)0x880180ec) = linstruc;
#endif
#if FW_VER==250
	* ((unsigned long*)0x88016ca0) = linstruc;
#endif

#if 0
	unsigned int *start = (unsigned int*) 0x88000000; //+0x5b7b0/4;
	for(mem = start; mem<(start+0x200000/4); mem++)
	{
		// Search for the error code and set the value to count
		if((*mem & 0xF400FFFF) == 0x34000148)//0x0149)
		{
			*mem = (*mem & 0xFFFF0000) + ((int)mem & 0xffff);
		}
	}
#endif

	// We should invalidate cache here
	// sceKernelDcacheWritebackInvalidateAll()
	// XXX NEEDS PATCHING
#if FW_VER==260
	asm("syscall 0x22a5");
#endif

#if FW_VER==250
	asm("syscall 0x20d6");
#endif

}

// XXX NEEDS PATCHING
// function args are readily in registers, so just do syscall, seems to work
// syscall numbers from PSPInside's syscalls.txt --abu
int IoOpen(char *name, int flags, int mode) {
	asm("syscall 0x2246");
}

void IoClose(int fd) {
	asm("syscall 0x2256");
}

int IoWrite(SceUID fd, const void *data, SceSize size) {
	asm("syscall 0x224c");
}


int patched_checkexec(void *ptr, void *check)
{
	*(unsigned short *)(check+0x5a) = 0;
//	unsigned int size = *(unsigned int*)(check+0x58c);

	int rc = original_checkexec_fn(ptr, check);
	
	/* Check if we managed to decrypt the file */
	if(*(unsigned short *)(check+0x5a) & 1)
	{
		return rc;
	}
	else
	{
		unsigned int outputBuffer = *(unsigned int*)(check+0x24);

//		memset(check, 0, 0x60);

		// This seems to be constant for using prx'es in v1.5

		*(unsigned char *)(check+0x20) = 0x00000001;
		*(unsigned char *)(check+0x44) = 0x00000001;
//		*(unsigned char *)(check+0x54) = 0x00000001;

//		*(unsigned int*)(check+0x1c) = ;
		*(unsigned int*)(check+0x24) = ptr;

		// version codes in one of the following maybe???
		*(unsigned int*)(check+0x10) = 0x000026f0;	
		*(unsigned int*)(check+0x28) = 0xffffffff;
		*(unsigned int*)(check+0x30) = 0x00003730;
		*(unsigned int*)(check+0x3c) = 0x00000070;
		*(unsigned int*)(check+0x4c) = 0x00003170;

		*(unsigned int*)(check+0x58) = 0x00011006;
		*(unsigned int*)(check+0x5c) = 0x00004e30;

		if(outputBuffer)
		{
			// Could also possibly just copy the pointer from the input
			// to the output.
//			memcpy(outputBuffer, ptr, 0x00004e30);
//			if(size == 0)
//				flashscreen(0, 0x00FF0000);
		}

	return 1;
	}
}

