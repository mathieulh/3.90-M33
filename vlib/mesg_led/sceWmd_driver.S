	.set noreorder

#include "pspimport.s"

// Build List
// sceWmd_driver_0000.o sceWmd_driver_0001.o sceWmd_driver_0002.o 

#ifdef F_sceWmd_driver_0000
	IMPORT_START	"sceWmd_driver",0x00090011
#endif
#ifdef F_sceWmd_driver_0001
	IMPORT_FUNC	"sceWmd_driver",0x7A0E484C,sceWmd_driver_7A0E484C
#endif
#ifdef F_sceWmd_driver_0002
	IMPORT_FUNC	"sceWmd_driver",0xB7CE9041,sceWmd_driver_B7CE9041
#endif
