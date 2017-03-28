#include <pspsdk.h>
#include <psputilsforkernel.h>
#include <string.h>
#include <zlib.h>
#include "dxar.h"

DXAR_Header header;
DXAR_Section section;
SceUID	fd=-1;
int		inited=0;
int		insection;
char	filename[256];


int dxarInit(const char *name)
{
	if (inited)
		return -1;

	strncpy(filename, name, 256);
	
	fd = sceIoOpen(filename, PSP_O_RDWR | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (fd < 0)
		return fd;

	memset(&header, 0, sizeof(DXAR_Header));

	header.magic = DXAR_MAGIC;
	header.version = 1;

	if (sceIoWrite(fd, &header, sizeof(header)) < sizeof(header))
		return -1;

	insection = 0;
	inited = 1;	
	
	return 0;
}

int dxarInitSection(char *sectname)
{
	if (!inited)
		return -1;

	if (insection)
		return -1;

	if (strlen(sectname) > 31)
		return -1;

	memset(&section, 0, sizeof(section));
	strcpy(section.sectionname, sectname);

	header.sections[header.nsections] = sceIoLseek(fd, 0, PSP_SEEK_CUR);
	
	if (sceIoWrite(fd, &section, sizeof(section)) < sizeof(section))
		return -1;

	insection = 1;

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

int	dxarAddFile(char *file, void *buf, int size, int forcecompress, int sigcheck, void *buf2, int buf2size)
{
	DXAR_FileEntry entry;
	int	writeplain = 0;
	
	if (!inited)
		return -1;

	if (!insection)
		return -1;

	if (strlen(file) > 127)
		return -1;

	if (!buf || !buf2)
		return -1;

	memset(&entry, 0, sizeof(entry));
	strcpy(entry.filepath, file);
	
	entry.filesize = size;
	entry.sigcheck = sigcheck;

	if (sceKernelUtilsMd5Digest(buf, size, entry.md5) < 0)
	{
		return -1;
	}

	if (!forcecompress)
	{
		u32 *buf32 = (u32 *)buf;
		
		if (buf32[0] == 0x5053507E) // ~PSP
		{
			u16 *buf16 = (u16 *)buf;

			if (buf16[6/2] & 1) // already comprressed
				writeplain = 1;
		}
	}

	if (size < (20*1024))
		writeplain = 1;

	if (!writeplain)
	{
		int compsize = deflateCompress(buf, size, buf2, buf2size);

		if (compsize <= 0)
			return -1;
	
		if ((compsize >= size) && !forcecompress)
			writeplain = 1;
		else
		{
			entry.compfilesize = compsize;
			entry.compression = COMPRESSION_DEFLATE;
		}		
	}

	if (sceIoWrite(fd, &entry, sizeof(entry)) < sizeof(entry))
		return -1;

	if (writeplain)
	{
		if (sceIoWrite(fd, buf, size) < size)
		{
			return -1;
		}
	}

	else
	{
		if (sceIoWrite(fd, buf2, entry.compfilesize) < entry.compfilesize)
		{
			return -1;
		}
	}

	section.nfiles++;

	return 0;
}

int	dxarAddDirectory(char *dir)
{
	DXAR_FileEntry entry;	
	
	if (!inited)
		return -1;

	if (!insection)
		return -1;

	if (strlen(dir) > 127)
		return -1;

	memset(&entry, 0, sizeof(entry));
	strcpy(entry.filepath, dir);

	entry.isdirectory = 1;
	
	if (sceIoWrite(fd, &entry, sizeof(entry)) < sizeof(entry))
		return -1;	

	section.nfiles++;

	return 0;
}

int dxarEndSection(void *buf, int bufsize)
{
	SceKernelUtilsSha1Context ctx;
	int remaining;
	
	if (!inited)
		return -1;

	if (!insection)
		return -1;

	section.sectionsize = sceIoLseek(fd, 0, PSP_SEEK_CUR) - header.sections[header.nsections] - sizeof(DXAR_Section);
	remaining = section.sectionsize;

	sceIoLseek(fd, header.sections[header.nsections]+sizeof(DXAR_Section), PSP_SEEK_SET);

	sceKernelUtilsSha1BlockInit(&ctx);
	
	while (remaining > 0)
	{
		int blocksize;

		if (remaining > bufsize)
			blocksize = bufsize;
		else
			blocksize = remaining;

		sceIoRead(fd, buf, blocksize);
		sceKernelUtilsSha1BlockUpdate(&ctx, buf, blocksize);
		remaining -= blocksize;
	}

	sceIoLseek(fd, header.sections[header.nsections], PSP_SEEK_SET);
	sceKernelUtilsSha1BlockResult(&ctx, section.sha1);

	if (sceIoWrite(fd, &section, sizeof(section)) < sizeof(section))
		return -1;

	sceIoLseek(fd, header.sections[header.nsections]+sizeof(DXAR_Section)+section.sectionsize, PSP_SEEK_SET);

	header.nsections++;	
	insection = 0;

	return 0;
}

int dxarEnd(void *buf, int bufsize)
{
	
	SceKernelUtilsSha1Context ctx;
	int remaining;
	
	if (!inited)
		return -1;

	if (insection)
		return -1;

	sceIoClose(fd);

	fd = sceIoOpen(filename, PSP_O_RDWR, 0777);

	if (fd < 0)
		return fd;

	header.size = sceIoLseek(fd, 0, PSP_SEEK_END) - sizeof(DXAR_Header);
	remaining = header.size;
	sceIoLseek(fd, sizeof(header), PSP_SEEK_SET);

	sceKernelUtilsSha1BlockInit(&ctx);
	
	while (remaining > 0)
	{
		int blocksize;

		if (remaining > bufsize)
			blocksize = bufsize;
		else
			blocksize = remaining;

		sceIoRead(fd, buf, blocksize);
		sceKernelUtilsSha1BlockUpdate(&ctx, buf, blocksize);
		remaining -= blocksize;
	}

	sceKernelUtilsSha1BlockResult(&ctx, header.sha1);
	sceIoLseek(fd, 0, PSP_SEEK_SET);

	if (sceIoWrite(fd, &header, sizeof(header)) < sizeof(header))
		return -1;

	sceIoClose(fd);
	inited = 0;

	return 0;

}
