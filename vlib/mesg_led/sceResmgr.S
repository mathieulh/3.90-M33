	.set noreorder

#include "pspimport.s"

// Build List
// sceResmgr_0000.o sceResmgr_0001.o 

#ifdef F_sceResmgr_0000
	IMPORT_START	"sceResmgr",0x40090011
#endif
#ifdef F_sceResmgr_0001
	IMPORT_FUNC	"sceResmgr",0x9DC14891,sceResmgr_9DC14891
#endif
