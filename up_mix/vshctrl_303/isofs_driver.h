#ifndef __ISOFS_DRIVER_H__

#define __ISOFS_DRIVER_H__

#include "umd9660_driver.h"

#define ISO9660_FILEFLAGS_FILE	1
#define ISO9660_FILEFLAGS_DIR	2

typedef struct __attribute__((packed))
{
	/* Directory record length. */
	u8	len_dr;
	/* Extended attribute record length. */
	u8	XARlength;
	/* First logical block where file starts. */
	u32	lsbStart;
	u32	msbStart;
	/* Number of bytes in file. */
	u32	lsbDataLength;
	u32	msbDataLength;
	/* Since 1900. */
	u8	year;
	u8	month;
	u8	day;
	u8	hour;
	u8	minute;
	u8	second;
	/* 15-minute offset from Universal Time. */
	u8	gmtOffse;
	/* Attributes of a file or directory. */
	u8	fileFlags;
	/* Used for interleaved files. */
	u8	interleaveSize;
	/* Used for interleaved files. */
	u8	interleaveSkip;
	/* Which volume in volume set contains this file. */
	u16	lsbVolSetSeqNum;
	u16	msbVolSetSeqNum;
	/* Length of file identifier that follows. */
	u8	len_fi;
	/* File identifier: actual is len_fi. */
	/* Contains extra blank byte if len_fi odd. */
	char    fi;
} Iso9660DirectoryRecord;

typedef struct
{
	int opened;
	int lba;
	int filesize;
	int filepointer;
	int attributes;
	int olddirlen;
	int eof;
	int curlba;
} FileHandle;

int isofs_init(PspIoDrvArg* arg);
int isofs_fastinit();
int isofs_exit(PspIoDrvArg* arg); 
int isofs_reset();
int isofs_open(PspIoDrvFileArg *arg, char *fullpath, int flags, SceMode mode); 
int isofs_close(PspIoDrvFileArg *arg);
int isofs_read(PspIoDrvFileArg *arg, char *data, int len);
SceOff isofs_lseek(PspIoDrvFileArg *arg, SceOff ofs, int whence);
int isofs_getstat(PspIoDrvFileArg *arg, const char *file, SceIoStat *stat);
int isofs_dopen(PspIoDrvFileArg *arg, const char *dirname); 
int isofs_dclose(PspIoDrvFileArg *arg);
int isofs_dread(PspIoDrvFileArg *arg, SceIoDirent *dir);
int isofs_ioctl(PspIoDrvFileArg *arg, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen);
int isofs_devctl(PspIoDrvFileArg *arg, const char *devname, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen);
PspIoDrv *getisofs_driver();




#endif



