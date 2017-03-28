#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <utime.h>
#include <time.h>		
#include <signal.h>		
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>	
#include <arpa/inet.h>
#include <fcntl.h>
#include <mhash.h>

#define u8	unsigned char
#define u16 unsigned short
#define u32	unsigned long

u8 buffer[256*1024];

int ReadFile(char *name, u8 *buf, u32 size)
{
	FILE *f = fopen(name, "rb");

	if (!f)
	{
		return -1;
	}

	int r = fread(buf, 1, size, f);
	fclose(f);

	return r;
}

void SaveFile(char *name, u8 *buf, u32 size)
{
	FILE *f = fopen(name, "wb");

	if (!f)
	{
		printf("Cannot create %s\n", name);
		return;
	}

	fwrite(buf, 1, size, f);
	fclose(f);
}

typedef struct BtcnfHeader
{
	int signature; // 0
	int devkit;		// 4
	int unknown[2];  // 8
	int modestart;  // 0x10
	int nmodes;  // 0x14
	int unknown2[2];  // 0x18
	int modulestart; // 0x20
	int nmodules;  // 0x24
	int unknown3[2]; // 0x28
	int modnamestart; // 0x30
	int modnameend; // 0x34
	int unknown4[2]; // 0x38
}  __attribute__((packed)) BtcnfHeader;

typedef struct ModeEntry
{
	u16 maxsearch;
	u16 searchstart; //
	int mode1;
	int mode2;
	int reserved[5];
} __attribute__((packed)) ModeEntry;

typedef struct ModuleEntry
{
	u32 stroffset;
	int reserved;
	u16 flags;
	u8 loadmode;
	u8 signcheck;
	int reserved2;
	u8  hash[0x10];
} __attribute__((packed)) ModuleEntry;

void Bin2Txt(u8 *buf, int size, FILE *out)
{
	BtcnfHeader *btcnf = (BtcnfHeader *)buf;
	ModeEntry *modes = (ModeEntry *)(buf+btcnf->modestart);
	ModuleEntry *modules = (ModuleEntry *)(buf+btcnf->modulestart);
	char *modnames = (char *)(buf+btcnf->modnamestart);
	int i, j, x;

	if (btcnf->signature != 0xF803001)
	{
		printf("Invalid signature 0x%08X.\n", btcnf->signature);
		return;
	}

	printf("Devkit 0x%08X.\n", btcnf->devkit);

	for (i = 0; i < btcnf->nmodes; i++)
	{		
		fprintf(out, "Mode = %d - %d\r\n", modes[i].mode1, modes[i].mode2);

		if (modes[i].searchstart != 0)
		{
			printf("Warning: cannot handle yet search start not being 0.\n");
		}
		
		for (x = 0; x < 5; x++)
		{
			if (modes[i].reserved[x] != 0)
			{
				printf("Warning: reserved field %d from mode number %d is not zero, it is 0x%08X\n", x, i, modes[i].reserved[x]);
			}
		}
		
		for (j = 0; j < modes[i].maxsearch; j++)
		{
			if (modules[j].reserved || modules[j].reserved2)
			{
				printf("Warning: one of reserved fields of module %d is not 0.\n", j);
			}

			if (modules[j].flags & modes[i].mode1)
			{
				if (modules[j].signcheck == 0x80)
				{
					fprintf(out, "$");
				}
				else if (modules[j].signcheck == 0)
				{
				}
				else
				{
					printf("Unknown signcheck field 0x%02X in module %d.\n", modules[j].signcheck, j);
				}

				if (modules[j].loadmode == 1)
				{
				}
				else if (modules[j].loadmode == 2)
				{
					fprintf(out, "%%");
				}
				else if (modules[j].loadmode == 4)
				{
					fprintf(out, "%%%%");
				}
				else
				{
					printf("Unknown loadmode 0x%02X at module %d.\n", modules[j].loadmode, j);
				}

				fprintf(out, "%s ", modnames+modules[j].stroffset);

				for (x = 0; x < 0x10; x++)
				{
					fprintf(out, "%02X", modules[j].hash[x]);
				}

				fprintf(out, "\r\n");
			}
		}

		fprintf(out, "\r\n");
	}
}

char *WaitPrintableCharacter(char *in, int firstprintable)
{
	char x;

	while ((x = *in) < firstprintable)
	{
		if (x == 0)
		{
			return NULL;
		}

		in++;
	}

	return in;
}

char *ReadMode(char *in, ModeEntry *mode)
{
	in = WaitPrintableCharacter(in, 0x21);

	if (!in)
	{
		printf("End of stream reached, no more modes.\n");
		return NULL;
	}

	in += sscanf(in, "Mode = %d - %d", &mode->mode1, &mode->mode2);
	printf("Mode found %d - %d.\n", mode->mode1, mode->mode2);

	return in;
}

void TxtToBin(char *in, FILE *out)
{
	ModeEntry modes[20];
	ModuleEntry modules[100];
	int i = 0, nmodes = 0;
	
	while ((ReadMode = ParseMode(in, modes+i)))
	{
		nmodes++;
	}
}

int main(int argc, char *argv[])
{
	if (argv[1][0] == '0')
	{	
		int size = ReadFile(argv[2], buffer, sizeof(buffer));
		FILE *w = fopen(argv[3], "w");

		Bin2Txt(buffer, size, w);
		fclose(w);
	}

	else
	{
	}	

	return 0;
}

