	.set noreorder

#include "pspimport.s"

// Build List
// sceResmgr_driver_0000.o sceResmgr_driver_0001.o 

#ifdef F_sceResmgr_driver_0000
	IMPORT_START	"sceResmgr_driver",0x00090011
#endif
#ifdef F_sceResmgr_driver_0001
	IMPORT_FUNC	"sceResmgr_driver",0x9DC14891,sceResmgr_driver_9DC14891
#endif
