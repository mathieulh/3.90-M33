#include <pspsdk.h>
#include <psputilsforkernel.h>
#include <pspcrypt.h>
#include <string.h>
#include "dxar.h"

SceUID fd = -1;
int opened = 0;
int insection = 0;

DXAR_Header header;
DXAR_Section section;

void *readbuf = NULL;
int nextfile = -1;

int dxarOpenAndValidate(char *filename, void *buf, int bufsize)
{
	SceKernelUtilsSha1Context ctx;
	int remaining;	
	u8	sha1[20];
	
	if (opened)
		return -1;

	fd = sceIoOpen(filename, PSP_O_RDONLY, 0777);

	if (fd < 0)
	{
		return -1;
	}

	if (sceIoRead(fd, &header, sizeof(header)) < sizeof(header))
	{
		return -1;
		sceIoClose(fd);
	}

	if (header.magic != DXAR_MAGIC)
	{
		sceIoClose(fd);
		return -1;
	}

	if (header.version != 1)
	{
		sceIoClose(fd);
		return -1;
	}

	if ((sceIoLseek(fd, 0, PSP_SEEK_END) - sizeof(DXAR_Header)) != header.size)
	{
		sceIoClose(fd);
		return -1;
	}	
	
	remaining = header.size;

	sceIoLseek(fd, sizeof(DXAR_Header), PSP_SEEK_SET);

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

	sceKernelUtilsSha1BlockResult(&ctx, sha1);

	if (memcmp(header.sha1, sha1, 20) != 0)
	{
		sceIoClose(fd);
		return -1;
	}

	opened = 1;

	return 0;
}

int dxarOpenSection(char *secname)
{
	if (!opened)
		return -1;

	if (insection)
		return -1;

	int i;

	for (i = 0; i < header.nsections; i++)
	{
		sceIoLseek(fd, header.sections[i], PSP_SEEK_SET);		
		
		if (sceIoRead(fd, &section, sizeof(DXAR_Section)) < sizeof(DXAR_Section))
			return -1;

		if (strcmp(section.sectionname, secname) == 0)
		{
			insection = 1;			

			return section.sectionsize;
		}
	}

	return -1;
}

int	dxarReadSection(void *buf)
{
	if (!opened)
		return -1;

	if (!insection)
		return -1;

	if (sceIoRead(fd, buf, section.sectionsize) < section.sectionsize)
		return -1;

	nextfile = 0;
	readbuf = buf;
	return 0;
}

u8 check_keys0[0x10] =
{
	0x71, 0xF6, 0xA8, 0x31, 0x1E, 0xE0, 0xFF, 0x1E,
	0x50, 0xBA, 0x6C, 0xD2, 0x98, 0x2D, 0xD6, 0x2D
}; 

u8 check_keys1[0x10] =
{
	0xAA, 0x85, 0x4D, 0xB0, 0xFF, 0xCA, 0x47, 0xEB,
	0x38, 0x7F, 0xD7, 0xE4, 0x3D, 0x62, 0xB0, 0x10
};

int Encrypt(u32 *buf, int size)
{
	buf[0] = 4;
	buf[1] = buf[2] = 0;
	buf[3] = 0x100;
	buf[4] = size;

	/* Note: this encryption returns different data in each psp,
	   But it always returns the same in a specific psp (even if it has two nands) */
	if (semaphore_4C537C72(buf, size+0x14, buf, size+0x14, 5) < 0)
		return -1;

	return 0;
}

int GenerateSigCheck(u8 *buf)
{
	u8 enc[0xD0+0x14];
	int iXOR, res;

	memcpy(enc+0x14, buf+0x110, 0x40);
	memcpy(enc+0x14+0x40, buf+0x80, 0x90);
	
	for (iXOR = 0; iXOR < 0xD0; iXOR++)
	{
		enc[0x14+iXOR] ^= check_keys0[iXOR&0xF]; 
	}

	if ((res = Encrypt((u32 *)enc, 0xD0)) < 0)
	{
		//printf("Encrypt failed.\n");
		return res;
	}

	for (iXOR = 0; iXOR < 0xD0; iXOR++)
	{
		enc[0x14+iXOR] ^= check_keys1[iXOR&0xF];
	}

	memcpy(buf+0x80, enc+0x14, 0xD0);
	return 0;
}

int dxarGetNextFile(DXAR_FileEntry *entry, void *buf, int *retPos, int *retMax)
{
	if (!opened)
		return -1;

	if (!insection)
		return -1;

	if (nextfile < 0)
		return -1;

	if (!readbuf)
		return -1;

	if (nextfile >= section.nfiles)
	{
		if (retPos)
			*retPos = section.nfiles;
		if (retMax)
			*retMax = section.nfiles;
		nextfile = -1;
		readbuf = NULL;
		return 1;
	}

	memcpy(entry, readbuf, sizeof(DXAR_FileEntry));

	readbuf += sizeof(DXAR_FileEntry);


	if (entry->compression != COMPRESSION_DEFLATE)
	{
		memcpy(buf, readbuf, entry->filesize);
		readbuf += entry->filesize;
	}
	else
	{
		int size = sceKernelDeflateDecompress(buf, entry->filesize, readbuf, NULL);

		if (size != entry->filesize)
			return -1;

		readbuf += entry->compfilesize;
	}

	if (entry->sigcheck)
	{
		GenerateSigCheck(buf);
	}

	nextfile++;

	if (retPos)
		*retPos = nextfile;

	if (retMax)
		*retMax = section.nfiles;

	return 0;
}

int dxarCloseSection()
{
	if (!opened)
		return -1;

	if (!insection)
		return -1;

	nextfile = -1;
	readbuf = NULL;
	insection = 0;

	return 0;
}

int dxarClose()
{
	if (!opened)
		return -1;

	if (insection)
		return -1;

	opened = 0;
	sceIoClose(fd);

	return 0;
}
