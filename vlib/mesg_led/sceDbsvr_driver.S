	.set noreorder

#include "pspimport.s"

// Build List
// sceDbsvr_driver_0000.o sceDbsvr_driver_0001.o 

#ifdef F_sceDbsvr_driver_0000
	IMPORT_START	"sceDbsvr_driver",0x00090011
#endif
#ifdef F_sceDbsvr_driver_0001
	IMPORT_FUNC	"sceDbsvr_driver",0x94561901,sceDbsvrGetData
#endif
