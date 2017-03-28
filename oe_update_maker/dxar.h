#ifndef __DXAR_H__
#define __DXAR_H__

#include <pspkernel.h>

#define DXAR_MAGIC 0x52415844

enum
{
	COMPRESSION_NONE = 0,
	COMPRESSION_DEFLATE = 1,
};


typedef struct
{
	int magic; // DX_MAGIC
	int version; // the version
	int	size; // size without header
	int	nsections; // Number of sections (max 32)
	u32	sections[32]; // sections offsets
	u8	sha1[20]; // SHA-1 of file without header
	u32	reserved[11];
} __attribute__((packed)) DXAR_Header;

typedef struct
{
	char	sectionname[32];
	int		sectionsize; // Section Size without header
	int		nfiles;
	u8		sha1[20]; // SHA1 of file without header
	u32		reserved[9];
	u32		custom[4];
} __attribute__((packed)) DXAR_Section;

typedef struct
{
	char	filepath[128];
	int		filesize;
	int		compfilesize;
	int		compression; 
	int		sigcheck;
	int		isdirectory;
	u8		md5[16]; /* MD5 of file when UNCOMPRESSED */
	u32		reserved[8];
} __attribute__((packed)) DXAR_FileEntry;

int deflateCompress(void *inbuf, int insize, void *outbuf, int outsize);

int dxarInit(const char *filename);
int dxarInitSection(char *sectname);
int	dxarAddFile(char *file, void *buf, int size, int forcecompress, int sigcheck, void *buf2, int buf2size);
int	dxarAddDirectory(char *file);
int dxarEndSection(void *buf, int bufsize);
int dxarEnd(void *buf, int bufsize);

#endif
