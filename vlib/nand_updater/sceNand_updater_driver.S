	.set noreorder

#include "pspimport.s"

// Build List
// sceNand_updater_driver_0000.o sceNand_updater_driver_0001.o 

#ifdef F_sceNand_updater_driver_0000
	IMPORT_START	"sceNand_updater_driver",0x00090011
#endif
#ifdef F_sceNand_updater_driver_0001
	IMPORT_FUNC	"sceNand_updater_driver",0x0BEE8F36,sceNand_updater_driver_0BEE8F36
#endif
