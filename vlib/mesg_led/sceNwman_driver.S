	.set noreorder

#include "pspimport.s"

// Build List
// sceNwman_driver_0000.o sceNwman_driver_0001.o 

#ifdef F_sceNwman_driver_0000
	IMPORT_START	"sceNwman_driver",0x00090011
#endif
#ifdef F_sceNwman_driver_0001
	IMPORT_FUNC	"sceNwman_driver",0x9555D68D,sceNwman_driver_9555D68D
#endif
