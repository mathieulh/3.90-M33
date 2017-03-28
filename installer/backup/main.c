//**************************************************************************
//		PSP Project: 'PSPDMPR' - main.cpp
//**************************************************************************

#include "main.h"

PSP_MODULE_INFO("PSPDMPR", 0x1000, 1, 1);
PSP_MAIN_THREAD_ATTR(0);


u8 block[32*32*528];

u32 ppb;

#define printf pspDebugScreenPrintf


void readNand(u32 page, u8 *buffer)
{
	u32 i, j;

	for (i = 0; i < ppb; i++)
	{
		for (j = 0; j < 4; j++)
		{
			sceNandReadAccess(page, buffer, NULL, 1, 49);
			sceNandReadExtraOnly(page, buffer+512, 1);
		}

		page++;
		buffer += 528;
	}
}


int main(int argc, char **argv) 
{
	ppb = sceNandGetPagesPerBlock();

	pspDebugScreenInit();

	int n;

	n = sceNandGetTotalBlocks();

	SceUID fd = sceIoOpen("ms0:/nand-dump.bin", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	printf("dumping...\n");

	u32 i, j;

	for (i = 0; i < (n*ppb);)
	{
		u8 *p;
		memset(block, 0xff, sizeof(block));

		p = block;
		
		for (j = 0; j < 32; j++)
		{
			LockFlash();
			if (!sceNandIsBadBlock(i))
			{
				readNand(i, p);
			}
			else
			{
				printf("bad block at page %d block %d\n", i, i/32);
			}
			UnlockFlash();

			i += ppb;
			p += (528*ppb);
		}

		sceIoWrite(fd, block, sizeof(block));
	}

	sceIoClose(fd);

	printf("Done.\n");

	sceKernelDelayThread(5*1000*1000);
	sceSysconPowerStandby();

	return 0;
}



///
/// Disable system-wide flash write access
///
void LockFlash() {
	if (!flashLocked)
		sceNandLock(0);
	flashLocked = 1;
}

///
/// Enable system-wide flash write access
///
void UnlockFlash() {
	if (flashLocked)
		sceNandUnlock();
	flashLocked = 0;
}


