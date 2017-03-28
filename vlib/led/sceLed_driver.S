	.set noreorder

#include "pspimport.s"

// Build List
// sceLed_driver_0000.o sceLed_driver_0001.o sceLed_driver_0002.o sceLed_driver_0003.o sceLed_driver_0004.o sceLed_driver_0005.o 

#ifdef F_sceLed_driver_0000
	IMPORT_START	"sceLed_driver",0x00010011
#endif
#ifdef F_sceLed_driver_0001
	IMPORT_FUNC	"sceLed_driver",0xB0B6A883,sceLedInit
#endif
#ifdef F_sceLed_driver_0002
	IMPORT_FUNC	"sceLed_driver",0xA8542C48,sceLedEnd
#endif
#ifdef F_sceLed_driver_0003
	IMPORT_FUNC	"sceLed_driver",0xDE91D3A4,sceLedSuspend
#endif
#ifdef F_sceLed_driver_0004
	IMPORT_FUNC	"sceLed_driver",0xA13B3D38,sceLedResume
#endif
#ifdef F_sceLed_driver_0005
	IMPORT_FUNC	"sceLed_driver",0xEA24BE03,sceLedSetMode
#endif
