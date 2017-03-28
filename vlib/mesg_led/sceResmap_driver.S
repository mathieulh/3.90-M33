	.set noreorder

#include "pspimport.s"

// Build List
// sceResmap_driver_0000.o sceResmap_driver_0001.o sceResmap_driver_0002.o 

#ifdef F_sceResmap_driver_0000
	IMPORT_START	"sceResmap_driver",0x00090011
#endif
#ifdef F_sceResmap_driver_0001
	IMPORT_FUNC	"sceResmap_driver",0x4434E59F,sceResmapPrepareByPolling
#endif
#ifdef F_sceResmap_driver_0002
	IMPORT_FUNC	"sceResmap_driver",0xE5659590,sceResmapPrepare
#endif
