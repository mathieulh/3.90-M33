	.set noreorder

#include "pspimport.s"

// Build List
// sceSysreg_driver_0000.o sceSysreg_driver_0001.o sceSysreg_driver_0002.o sceSysreg_driver_0003.o sceSysreg_driver_0004.o sceSysreg_driver_0005.o sceSysreg_driver_0006.o sceSysreg_driver_0007.o 

#ifdef F_sceSysreg_driver_0000
	IMPORT_START	"sceSysreg_driver",0x00010011
#endif
#ifdef F_sceSysreg_driver_0001
	IMPORT_FUNC	"sceSysreg_driver",0x51437D5D,sceSysregMsifResetEnable
#endif
#ifdef F_sceSysreg_driver_0002
	IMPORT_FUNC	"sceSysreg_driver",0x98DC24EA,sceSysregMsifIoDisable
#endif
#ifdef F_sceSysreg_driver_0003
	IMPORT_FUNC	"sceSysreg_driver",0xED339752,sceSysregMsifClkSelect
#endif
#ifdef F_sceSysreg_driver_0004
	IMPORT_FUNC	"sceSysreg_driver",0xE941CCDC,sceSysregMsifDelaySelect
#endif
#ifdef F_sceSysreg_driver_0005
	IMPORT_FUNC	"sceSysreg_driver",0x251FB288,sceSysregMsifBusClockDisable
#endif
#ifdef F_sceSysreg_driver_0006
	IMPORT_FUNC	"sceSysreg_driver",0x53E149E6,sceSysregMsifClkDisable
#endif
#ifdef F_sceSysreg_driver_0007
	IMPORT_FUNC	"sceSysreg_driver",0x20540572,sceSysregMsifResetDisable
#endif


