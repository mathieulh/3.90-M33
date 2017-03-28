	.set noreorder

#include "pspimport.s"

// Build List
// scePwm_driver_0000.o scePwm_driver_0001.o scePwm_driver_0002.o scePwm_driver_0003.o scePwm_driver_0004.o scePwm_driver_0005.o scePwm_driver_0006.o scePwm_driver_0007.o scePwm_driver_0008.o scePwm_driver_0009.o 

#ifdef F_scePwm_driver_0000
	IMPORT_START	"scePwm_driver",0x00010011
#endif
#ifdef F_scePwm_driver_0001
	IMPORT_FUNC	"scePwm_driver",0x1FC555BE,scePwmStop
#endif
#ifdef F_scePwm_driver_0002
	IMPORT_FUNC	"scePwm_driver",0x526F94FC,scePwmSuspend
#endif
#ifdef F_scePwm_driver_0003
	IMPORT_FUNC	"scePwm_driver",0x68BA9CC1,scePwmInit
#endif
#ifdef F_scePwm_driver_0004
	IMPORT_FUNC	"scePwm_driver",0x850ED3D3,scePwmStart
#endif
#ifdef F_scePwm_driver_0005
	IMPORT_FUNC	"scePwm_driver",0x93F30DAC,scePwmIsLoading
#endif
#ifdef F_scePwm_driver_0006
	IMPORT_FUNC	"scePwm_driver",0xAB6D2E36,scePwm_driver_AB6D2E36
#endif
#ifdef F_scePwm_driver_0007
	IMPORT_FUNC	"scePwm_driver",0xB3169427,scePwmEnd
#endif
#ifdef F_scePwm_driver_0008
	IMPORT_FUNC	"scePwm_driver",0xE449F656,scePwmResume
#endif
#ifdef F_scePwm_driver_0009
	IMPORT_FUNC	"scePwm_driver",0xF624C1A0,scePwm_driver_F624C1A0
#endif
