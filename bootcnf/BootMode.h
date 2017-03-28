
#ifndef BOOT_MODE_H
#define BOOT_MODE_H

#include <vector>

#include "ModuleEntry.h"

class CBootMode
{
public:
	CBootMode();
	~CBootMode();

	int ParseBinary(char *buffer);
	int ParseText(char *buffer, std::vector<CModuleEntry>&modEntry, char *modNames, int &namePos);

	unsigned int WriteBinary(FILE *fd, unsigned int modOffset);
	int WriteText(FILE *fd);

	int GetMode(void)
	{
		return mMode1;
	}

	void Print(void);

private:
	int mMode1;
	int mMode2;
	int mModCount;

	struct ModeBinHeader
	{
		int Mode1;
		int Mode2;
		int zero1;
		short ModOffset;
		short ModCount;
		int zero2[4];
	};
};

#endif
