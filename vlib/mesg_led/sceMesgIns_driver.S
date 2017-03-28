	.set noreorder

#include "pspimport.s"

// Build List
// sceMesgIns_driver_0000.o sceMesgIns_driver_0001.o sceMesgIns_driver_0002.o 

#ifdef F_sceMesgIns_driver_0000
	IMPORT_START	"sceMesgIns_driver",0x00090011
#endif
#ifdef F_sceMesgIns_driver_0001
	IMPORT_FUNC	"sceMesgIns_driver",0x55C358E8,sceMesgIns_driver_55C358E8
#endif
#ifdef F_sceMesgIns_driver_0002
	IMPORT_FUNC	"sceMesgIns_driver",0x75336BDD,sceMesgIns_driver_75336BDD
#endif
