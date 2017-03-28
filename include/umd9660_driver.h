#ifndef __UMD9660_DRIVER_H__

#define __UMD9660_DRIVER_H__

#define SECTOR_SIZE	0x0800

#define GetUmdFile sctrlSEGetUmdFile
#define SetUmdFile sctrlSESetUmdFile

int  OpenIso();
int  ReadUmdFileRetry(void *buf, int size, int fpointer);
int  Umd9660ReadSectors(int lba, int nsectors, void *buf);
PspIoDrv *getumd9660_driver();
void DoAnyUmd();


#endif

