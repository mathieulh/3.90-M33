	.set noreorder

#include "pspimport.s"

#ifdef F_sceMesgd_driver_0000
	IMPORT_START	"sceMesgd_driver",0x00090000
#endif
#ifdef F_sceMesgd_driver_0001
	IMPORT_FUNC	"sceMesgd_driver",0x102DC8AF,sceMesgd_driver_102DC8AF
#endif
#ifdef F_sceMesgd_driver_0002
	IMPORT_FUNC	"sceMesgd_driver",0xADD0CB66,sceMesgd_driver_ADD0CB66
#endif

