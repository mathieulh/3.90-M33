
#ifndef MODULE_ENTRY_H
#define MODULE_ENTRY_H

class CModuleEntry
{
public:
	CModuleEntry(void);
	~CModuleEntry();

	int ParseBinary(char *buffer);
	int ParseText(int mode, char *file, char *key, char *modNames, int &namePos);

	int WriteBinary(FILE *fd);
	int WriteText(FILE *fd, int mode, char *modNames);

	void Print(int mode, char *modNames);

private:
	struct ModEntryHeader
	{
		short Mode;			// 0001 - vsh mode, 
							// 0002 - game (presume)
							// 0004 - update
							// 0008 - pops
							// 0010 - licensegame
							// 0040 - app 
		char Type;			// for the modules which used to have $ -> 01, $% -> 2, $%% -> 4
		unsigned char unk1;	// 0x80 in pspbtcnf.bin
		int ModNameOffset;	// Module name offset 
		int zero[2];
		int key[4];			// Hash of some description
	};

	int mType;
	int mMode;
	int mNameOffset;
	int mKey[4];
	unsigned char mUnk;
};

#endif

