#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspidstorage.h>
#include <psputilsforkernel.h>
#include <pspsysmem_kernel.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "leafs.h"


PSP_MODULE_INFO("pspDegeneration_Driver", 0x1000, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

#define PSP_DEGENERATION_KEY4	1
#define PSP_DEGENERATION_KEY5	2
#define PSP_DEGENERATION_KEY6	4
#define PSP_DEGENERATION_KEY7	8
#define PSP_DEGENERATION_KEY8	16

#define PSP_DEGENERATION_ALL (PSP_DEGENERATION_KEY4 | PSP_DEGENERATION_KEY5 | PSP_DEGENERATION_KEY6 | PSP_DEGENERATION_KEY7 | PSP_DEGENERATION_KEY8)

#define RETURN(x) pspSdkSetK1(k1); return x

int pspDegCheckDegenerationState()
{
	int res = 0;
	u32 x;
	u16 baryon[2];
	int extrakeys = 0;

	int k1 = pspSdkSetK1(0);

	if (sceKernelGetModel() == PSP_MODEL_SLIM_AND_LITE)
	{
		RETURN(0);
	}

	if (sceIdStorageLookup(4, 0, &x, 4) < 0)
	{
		RETURN(-1);
	}

	if (x != 0x4272796e)
	{
		res |= PSP_DEGENERATION_KEY4;
	}

	if (sceIdStorageLookup(5, 0, &x, 4) < 0)
	{
		RETURN(-2);
	}

	if (x != 0x436c6b67)
	{
		res |= PSP_DEGENERATION_KEY5;
	}

	if (sceIdStorageLookup(6, 0, &x, 4) < 0)
	{
		RETURN(-3);
	}

	if (x != 0x4d446472)
	{
		res |= PSP_DEGENERATION_KEY6;
	}

	while ((x = sceSysconGetBaryonVersion((u16 *)baryon)) < 0);

	x = baryon[1];

	if (x & 0xF0)
	{
		if (x == 0x10)
		{
			x = baryon[1]&0xFF;

			if (x < 0x10)
			{
				extrakeys = 1;
			}
			else if (x >= 0x12)
			{
				extrakeys = 1;
			}
		}
		else
		{
			extrakeys = 1;
		}
	}
	else
	{
		extrakeys = 0;
	}

	if (extrakeys)
	{
		if (sceIdStorageLookup(7, 0, &x, 4) >= 0)
		{
			if (x != 0x41506144)
			{
				res |= PSP_DEGENERATION_KEY7;
			}
		}

		if (sceIdStorageLookup(8, 0, &x, 4) >= 0)
		{
			if (x != 0x4C434470)
			{
				res |= PSP_DEGENERATION_KEY8;
			}
		}
	}

	RETURN(res);
}

int pspDegCorrectDegeneration(int state)
{
	int k1 = pspSdkSetK1(0);

	if ((state & PSP_DEGENERATION_ALL) == 0)
	{
		RETURN(0);
	}

	if (state & PSP_DEGENERATION_KEY4)
	{
		if (sceIdStorageWriteLeaf(4, leaf4) < 0)
			return -1;
	}

	if (state & PSP_DEGENERATION_KEY5)
	{
		if (sceIdStorageWriteLeaf(5, leaf5) < 0)
			return -2;
	}

	if (state & PSP_DEGENERATION_KEY6)
	{
		if (sceIdStorageWriteLeaf(6, leaf6) < 0)
			return -3;
	}

	if (state & PSP_DEGENERATION_KEY7)
	{
		if (sceIdStorageWriteLeaf(7, leaf7) < 0)
			return -4;
	}

	if (state & PSP_DEGENERATION_KEY8)
	{
		if (sceIdStorageWriteLeaf(8, leaf8) < 0)
			return -5;
	}

	sceIdStorageFlush();

	RETURN(0);
}

int module_start(SceSize args, void *argp)
{
	return 0;
}

int module_stop(void)
{
	return 0;
}
