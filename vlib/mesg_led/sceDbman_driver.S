	.set noreorder

#include "pspimport.s"

// Build List
// sceDbman_driver_0000.o sceDbman_driver_0001.o sceDbman_driver_0002.o 

#ifdef F_sceDbman_driver_0000
	IMPORT_START	"sceDbman_driver",0x00090011
#endif
#ifdef F_sceDbman_driver_0001
	IMPORT_FUNC	"sceDbman_driver",0x34B53D46,sceDbmanSelectByPolling
#endif
#ifdef F_sceDbman_driver_0002
	IMPORT_FUNC	"sceDbman_driver",0xB2B8C3F9,sceDbmanSelect
#endif
