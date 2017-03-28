
#ifndef BOOT_CONFIG_H
#define BOOT_CONFIG_H

#include <vector>

#include "BootMode.h"

class CBootConfig
{
public:
	CBootConfig(void);
	~CBootConfig();

	// Binary and text parser to read in the data
	int ParseBinary(const char *file);
	int ParseText(const char *file);

	// Output to either a binary or a text file
	int WriteBinary(const char *file);
	int WriteText(const char *file);

	// Debug
	void Print(void);

private:
	std::vector<CBootMode>		mBootMode;
	std::vector<CModuleEntry>	mModule;
	char *mpModNames;

	int mModeCount;
	int mModuleCount;
	int mModNameSize;

	struct BootBinHeader
	{
		int FileHeader;
		int devkit;		
		int unknown[2];
		int ModeStart;
		int ModeCount;
		int unknown2[2];
		int ModuleStart;
		int ModuleCount;
		int unknown3[2];
		int ModNameStart;
		int ModNameEnd;
		int unknown4[2];		
	} __attribute__((packed));
};

#endif
