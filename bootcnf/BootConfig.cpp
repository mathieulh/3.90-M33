
#include <stdio.h>

#include "BootConfig.h"

///////////////////////////////////////////////////////////////////////////////////////
//
// Constructors/Destructors
// 
///////////////////////////////////////////////////////////////////////////////////////
CBootConfig::CBootConfig(void)
{
	mModeCount		= 0;
	mModuleCount	= 0;
	mModNameSize	= 0;
	mpModNames		= 0;
}

CBootConfig::~CBootConfig()
{
	if(mpModNames)
	{
		free(mpModNames);
	}
}

///////////////////////////////////////////////////////////////////////////////////////
//
// Parsers - Binary and text
// 
///////////////////////////////////////////////////////////////////////////////////////
int CBootConfig::ParseBinary(const char *file)
{
	int i;
	char *modePtr;
	char *modulePtr;

	FILE *fd = fopen(file, "rb");

	// A temp buffer to allow for 50k binary
	char buffer[50*1024];

	if(!fd)
	{
		printf("Unable to open file\n");
		return -1;
	}

	int fileSize = fread(buffer, 1, 50*1024, fd);

	if(fileSize > 0)
	{
		struct BootBinHeader *bootHdr = (BootBinHeader *)buffer;

		// Deal with all the module modes
		mModeCount = bootHdr->ModeCount;
		
		for(i=0; i<mModeCount; i++)
		{
			mBootMode.push_back(CBootMode());

			modePtr = buffer + bootHdr->ModeStart + (i*0x20);

			if(mBootMode[i].ParseBinary(modePtr) < 0)
			{
				return -1;
			}
		}

		// Now to sort out the module definitions
		mModuleCount = bootHdr->ModuleCount;

		for(i=0; i<mModuleCount; i++)
		{
			mModule.push_back(CModuleEntry());

			modulePtr = buffer + bootHdr->ModuleStart + (i*0x20);

			if(mModule[i].ParseBinary(modulePtr) < 0)
			{
				return -1;
			}
		}

		// Now finally the module string table
		char *moduleNames = buffer + bootHdr->ModNameStart;
		int nameSize = bootHdr->ModNameEnd - bootHdr->ModNameStart;

		mpModNames = (char*)malloc(nameSize);
		mModNameSize = nameSize;

		memcpy(mpModNames, moduleNames, nameSize);
	}

	return 0;
}

int CBootConfig::ParseText(const char *file)
{
	int i;
	char *modePtr;

	FILE *fd = fopen(file, "r");

	// A temp buffer to allow for 50k text file
	char buffer[50*1024];
	char textbuf[50*1024];
	int textPos = 1;

	textbuf[0] = '*';

	if(!fd)
	{
		printf("Unable to open file\n");
		return -1;
	}

	int fileSize = fread(buffer, 1, 50*1024, fd);

	modePtr = buffer;

	if(fileSize > 0)
	{
		do
		{
			modePtr = strstr(modePtr, "Mode");

			if(modePtr)
			{
				mBootMode.push_back(CBootMode());
				
				mModuleCount += mBootMode[mModeCount].ParseText(modePtr, mModule, textbuf, textPos);

				mModeCount++;

				modePtr++;
			}
		} while (modePtr);

		// Now to copy the text across to its correct location
		mModNameSize = textPos;
		mpModNames = (char*)malloc(mModNameSize);

		memcpy(mpModNames, textbuf, mModNameSize);

		for(i=0; i<mModNameSize; i++)
		{
			if(mpModNames[i] == '*')
			{
				mpModNames[i] = 0;
			}
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////
//
// Write - Binary and text
// 
///////////////////////////////////////////////////////////////////////////////////////
int CBootConfig::WriteBinary(const char *file)
{
	int i;

	FILE *fd = fopen(file, "w+b");

	if(fd <= 0)
	{
		printf("Unable to open file\n");
		return -1;
	}

	BootBinHeader hdr;
	memset(&hdr, 0, sizeof(hdr));

	// Write the header
	hdr.FileHeader	= 0x0F803001;
	hdr.devkit		= 0x03070110;

	int temp = sizeof(hdr);
	hdr.ModeStart	= temp;
	hdr.ModeCount	= mModeCount;

	temp += 0x20 * mModeCount;
	hdr.ModuleStart	= temp;
	hdr.ModuleCount = mModuleCount;

	temp += 0x20 * mModuleCount;
	hdr.ModNameStart = temp;

	temp += mModNameSize;
	hdr.ModNameEnd   = temp;

	fwrite(&hdr, sizeof(hdr), 1, fd);

	unsigned int modOffset = 0;

	for(i=0; i<mModeCount; i++)
	{
		modOffset = mBootMode[i].WriteBinary(fd, modOffset);
	}

	for(i=0; i<mModuleCount; i++)
	{
		mModule[i].WriteBinary(fd);
	}

	fwrite(mpModNames, mModNameSize, 1, fd);
}

int CBootConfig::WriteText(const char *file)
{
	FILE *fd = fopen(file, "w+");

	int i,j,mode;
	for(i=0; i<mModeCount; i++)
	{
		mBootMode[i].WriteText(fd);
		
		mode = mBootMode[i].GetMode();

		for(j=0; j<mModuleCount; j++)
		{
			mModule[j].WriteText(fd, mode, mpModNames);
		}
		
		fprintf(fd, "\n");
	}

	fclose(fd);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////
//
// Debug
// 
///////////////////////////////////////////////////////////////////////////////////////
void CBootConfig::Print(void)
{
	printf("CBootConfig\tModeCount = %d\n\n", mModeCount);

	int i,j,mode;
	for(i=0; i<mModeCount; i++)
	{
		printf("CBootConfig\tMode %d\n", i);
		mBootMode[i].Print();
		
		mode = mBootMode[i].GetMode();

		for(j=0; j<mModuleCount; j++)
		{
			mModule[j].Print(mode, mpModNames);
		}
		
		printf("\n");
	}
}
