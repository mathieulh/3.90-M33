
#include <stdio.h>
#include <string>

#include "BootMode.h"

CBootMode::CBootMode()
{
	mMode1 = 0;
	mMode2 = 0;
	mModCount = 0;
}

CBootMode::~CBootMode()
{
}

int CBootMode::ParseBinary(char *buffer)
{
	struct ModeBinHeader *modeHeader = (ModeBinHeader *)buffer;

	mModCount = modeHeader->ModCount;
	mMode1 = modeHeader->Mode1;
	mMode2 = modeHeader->Mode2;

	return 0;
}

int CBootMode::ParseText(char *buffer, std::vector<CModuleEntry>&modEntry, char *modNames, int &namePos)
{
	int mode1, mode2;
	char temp[255];
	char file[255];
	char key[255];
	bool done = false;

	// We should only be here if we have been passed "Mode"
	sscanf(buffer, "%s = %d - %d", temp, &mode1, &mode2);

	do
	{
		buffer = strchr(buffer, '\n');
		if(buffer)
		{
			buffer++;
			int i = sscanf(buffer, "%s %s", file, key);

			if(i > 0)
			{
				if((file[0] == '$') || (file[0] == '%') || (file[0] == '/'))
				{
					CModuleEntry tmpEntry;
					tmpEntry.ParseText(mode1, file, key, modNames, namePos);

					modEntry.push_back(tmpEntry);

					mModCount++;
				}
				else
				{
					done = true;
				}
			}
		}
		else
		{
			done = true;
		}
	} while (!done);

	mMode1 = mode1;
	mMode2 = mode2;

	return mModCount;
}

unsigned int CBootMode::WriteBinary(FILE *fd, unsigned int modOffset)
{
	struct ModeBinHeader hdr;
	memset(&hdr, 0, sizeof(hdr));

	hdr.Mode1 = mMode1;
	hdr.Mode2 = mMode2;
	hdr.ModOffset = modOffset;
	hdr.ModCount  = mModCount;

	fwrite(&hdr, sizeof(hdr), 1, fd);

	return modOffset + mModCount;
}

int CBootMode::WriteText(FILE *fd)
{
	fprintf(fd, "Mode = %d - %d\n", mMode1, mMode2);
}

void CBootMode::Print(void)
{
	printf("CBootMode\tmMode1=0x%x\n", mMode1);
	printf("CBootMode\tmMode2=0x%x\n", mMode2);
	printf("CBootMode\tmModCount=%d\n", mModCount);
}
