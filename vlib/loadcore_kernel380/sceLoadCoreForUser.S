	.set noreorder

#include "pspimport.s"

// Build List
// sceLoadCoreForUser_0000.o sceLoadCoreForUser_0001.o 

#ifdef F_sceLoadCoreForUser_0000
	IMPORT_START	"sceLoadCoreForUser",0x40010011
#endif
#ifdef F_sceLoadCoreForUser_0001
	IMPORT_FUNC	"sceLoadCoreForUser",0xDA1B09AA,sceKernelUnLinkLibraryEntries
#endif
