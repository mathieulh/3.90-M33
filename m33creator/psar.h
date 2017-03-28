#ifndef __DXAR_H__
#define __DXAR_H__

#include <pspkernel.h>

#define PSAR_MAGIC 0x52415350

typedef struct
{
	u32 magic;
	u32 version;
	u32 filesize;
	u32 unk1; // to 1
} __attribute__((packed)) PSAR_Header;

typedef struct
{
	u32  unk1; // to 1
	char filename[256];
	u32	 chunksize;
	u32	 filesize;
	u32  flags;
} __attribute__((packed)) PSAR_Entry;

int deflateCompress(void *inbuf, int insize, void *outbuf, int outsize);

int psarInit(SceUID descriptor);
int	psarAddFile(char *file, void *buf, int size, u8 *buf2, int buf2size, int sigcheck, int origsize);
int psarWriteBuf(void *buf, int size);
int psarEnd(void *buf, int bufsize);

#endif
