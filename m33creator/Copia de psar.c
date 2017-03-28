#include <pspsdk.h>
#include <psputilsforkernel.h>
#include <string.h>
#include <zlib.h>
#include "psar.h"

PSAR_Header header;
SceUID	fd=-1;
int		inited=0;
int position;


int psarInit(SceUID descriptor)
{
	if (inited)
		return -1;

	fd = descriptor;
	position = sceIoLseek(fd, 0, PSP_SEEK_CUR);

	memset(&header, 0, sizeof(PSAR_Header));

	header.magic = PSAR_MAGIC;
	header.version = 2;
	header.unk1 = 1;

	if (sceIoWrite(fd, &header, sizeof(header)) < sizeof(header))
		return -1;

	inited = 1;	
	
	return 0;
}

z_stream z;

int deflateCompress(void *inbuf, int insize, void *outbuf, int outsize)
{
	int res;
	
	z.zalloc = Z_NULL;
	z.zfree  = Z_NULL;
	z.opaque = Z_NULL;

	if (deflateInit2(&z, Z_DEFAULT_COMPRESSION , Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY) != Z_OK)
		return -1;

	z.next_out  = outbuf;
	z.avail_out = outsize;
	z.next_in   = inbuf;
	z.avail_in  = insize;

	if (deflate(&z, Z_FINISH) != Z_STREAM_END)
	{
		return -1;
	}

	res = outsize - z.avail_out;

	if (deflateEnd(&z) != Z_OK)
		return -1;

	return res;
}

int	psarAddFile(char *file, void *buf, int size, u8 *buf2, int buf2size, int sigcheck, int origsize)
{
	PSAR_Entry entry;
	u8 *p;
		
	if (!inited)
		return -1;

	if (strlen(file) > 255)
		return -1;

	if (!buf || !buf2)
		return -1;

	memset(&entry, 0, sizeof(entry));
	strcpy(entry.filename, file);
	
	entry.flags = 0x1010000;
	entry.unk1 = 1;

	if (sigcheck)
		entry.flags |= 2;
	else
		entry.flags |= 1;

	if (origsize)
	{
		entry.chunksize = size;
		entry.filesize = origsize;
		p = buf;
	}
	else
	{
		buf2[0] = 0x78;
		buf2[1] = 0x9C;

		int compsize = deflateCompress(buf, size, buf2+2, buf2size);

		if (compsize <= 0)
			return -1;
	
		entry.filesize = size;
		entry.chunksize = compsize+2;
		p = buf2;
	}

	if (sceIoWrite(fd, &entry, sizeof(entry)) != sizeof(entry))
		return -1;

	if (sceIoWrite(fd, p, entry.chunksize) != entry.chunksize)
	{
		return -1;		
	}

	return 0;
}

int psarWriteBuf(void *buf, int size)
{
	if (!inited)
		return -1;

	if (!buf || size <= 0)
		return -1;

	if (sceIoWrite(fd, buf, size) != size)
		return -1;

	return 0;
}

int psarEnd(void *buf, int bufsize)
{	
	if (!inited)
		return -1;

	int size = sceIoLseek(fd, 0, PSP_SEEK_CUR) - 0x20 - position;
	
	sceIoLseek(fd, position+8, PSP_SEEK_SET);
	sceIoWrite(fd, &size, 4);
	sceIoClose(fd);	

	return 0;

}
