#include <pspsdk.h>
#include <pspkernel.h>

#include "umd9660_driver.h"

int IsofileReadSectors(int lba, int nsectors, void *buf, int *eod)
{
	int read = ReadUmdFileRetry(buf, SECTOR_SIZE*nsectors, lba*SECTOR_SIZE);

	if (read < 0)
	{
		return read;
	}

	read = read / SECTOR_SIZE;
	
	if (eod)
	{
		*eod = 0;
	}

	return read;	
}

int IsofileGetDiscSize(int umdfd)
{
	int ret = sceIoLseek(umdfd, 0, PSP_SEEK_CUR);
	int size = sceIoLseek(umdfd, 0, PSP_SEEK_END);

	sceIoLseek(umdfd, ret, PSP_SEEK_SET);

	if (size < 0)
		return size;

	return size / SECTOR_SIZE;
}

