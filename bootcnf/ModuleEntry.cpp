
#include <stdio.h>
#include <string>

#include "ModuleEntry.h"

int GetBigLong(unsigned char *data_p)
{
	int val = 0;
	val = *(data_p)<<24;
	val = val + (*(data_p+1)<<16);
	val = val + (*(data_p+2)<<8);
	val = val + *(data_p+3);
	data_p += 4;
	return val;
}

CModuleEntry::CModuleEntry(void)
{
	mType		= 0;
	mMode		= 0;
	mNameOffset	= 0;
	mUnk		= 0;

	memset(mKey, 0, 0x10);
}

CModuleEntry::~CModuleEntry()
{

}

int CModuleEntry::ParseBinary(char *buffer)
{
	struct ModEntryHeader *modHdr = (ModEntryHeader *)buffer;

	mType		= modHdr->Type;
	mMode		= modHdr->Mode;
	mUnk		= modHdr->unk1;
	mNameOffset	= modHdr->ModNameOffset;

	memcpy(mKey, modHdr->key, 0x10);

	return 0;
}

int CModuleEntry::ParseText(int mode, char *buf, char *key, char *modNames, int &namePos)
{
	char *str;

	mMode = mode;

	if(buf[0] == '$')
	{
		mUnk = 0x80;
		buf++;
	}

	mType = 1;

	if(buf[0] == '%')
	{
		buf++;
		if(buf[0] != '%')
		{
			mType = 2;
		}
		else
		{
			mType = 4;
			buf++;
		}
	}

	str = strstr(modNames, buf);

	if(str == 0)
	{
		mNameOffset = namePos;

		strcpy(modNames+namePos, buf);
		namePos += strlen(buf);
		modNames[namePos]='*';
		namePos ++;
	}
	else
	{
		mNameOffset = str - modNames;
	}

	int tmp[4];

	sscanf(key, "%08X%08X%08X%08X\n", &tmp[0], 
									  &tmp[1], 
									  &tmp[2], 
									  &tmp[3]);

	mKey[0] = GetBigLong((unsigned char *)&tmp[0]);
	mKey[1] = GetBigLong((unsigned char *)&tmp[1]);
	mKey[2] = GetBigLong((unsigned char *)&tmp[2]);
	mKey[3] = GetBigLong((unsigned char *)&tmp[3]);

	return 0;
}

int CModuleEntry::WriteBinary(FILE *fd)
{
	struct ModEntryHeader hdr;
	memset(&hdr, 0, sizeof(hdr));

	hdr.Mode = mMode;
	hdr.Type = mType;
	hdr.unk1 = mUnk;

	hdr.ModNameOffset = mNameOffset;

	memcpy(hdr.key, mKey, 0x10);

	fwrite(&hdr, sizeof(hdr), 1, fd);

	return 0;
}

int CModuleEntry::WriteText(FILE *fd, int mode, char *modNames)
{
	if(mode == mMode)
	{
		if(mUnk == 0x80)
		{
			fprintf(fd, "$");
		}

		if(mType == 2)
		{
			fprintf(fd, "%%");
		}
		else if(mType == 4)
		{
			fprintf(fd, "%%%%");
		}

		fprintf(fd, "%s ", modNames+mNameOffset);

		fprintf(fd, "%08X%08X%08X%08X\n", GetBigLong((unsigned char *)&mKey[0]), 
										  GetBigLong((unsigned char *)&mKey[1]), 
										  GetBigLong((unsigned char *)&mKey[2]), 
										  GetBigLong((unsigned char *)&mKey[3]));
	}

	return 0;
}

void CModuleEntry::Print(int mode, char *modNames)
{
	if(mode == mMode)
	{
		if(mType == 2)
		{
			printf("%%");
		}
		else if(mType == 4)
		{
			printf("%%%%");
		}

		printf("%s\n", modNames+mNameOffset);
	}
}
