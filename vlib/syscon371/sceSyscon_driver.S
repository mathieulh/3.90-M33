	.set noreorder

#include "pspimport.s"

// Build List
// sceSyscon_driver_0000.o sceSyscon_driver_0001.o sceSyscon_driver_0002.o 

#ifdef F_sceSyscon_driver_0000
	IMPORT_START	"sceSyscon_driver",0x00010011
#endif
#ifdef F_sceSyscon_driver_0001
	IMPORT_FUNC	"sceSyscon_driver",0x0D87AA65,sceSysconGetWlanSwitch
#endif
#ifdef F_sceSyscon_driver_0002
	IMPORT_FUNC	"sceSyscon_driver",0x765F7E9B,sceSysconCtrlWlanPower
#endif
#ifdef F_sceSyscon_driver_0003
	IMPORT_FUNC	"sceSyscon_driver",0xEDA507CD,sceSysconCtrlHRPower
#endif




