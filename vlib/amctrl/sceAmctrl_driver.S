	.set noreorder

#include "pspimport.s"

// Build List
// sceAmctrl_driver_0000.o sceAmctrl_driver_0001.o sceAmctrl_driver_0002.o sceAmctrl_driver_0003.o sceAmctrl_driver_0004.o sceAmctrl_driver_0005.o sceAmctrl_driver_0006.o sceAmctrl_driver_0007.o 

#ifdef F_sceAmctrl_driver_0000
	IMPORT_START	"sceAmctrl_driver",0x00010011
#endif
#ifdef F_sceAmctrl_driver_0001
	IMPORT_FUNC	"sceAmctrl_driver",0x0785C974,sceDrmBBCipherUpdate
#endif
#ifdef F_sceAmctrl_driver_0002
	IMPORT_FUNC	"sceAmctrl_driver",0x1CCB66D2,sceDrmBBCipherInit
#endif
#ifdef F_sceAmctrl_driver_0003
	IMPORT_FUNC	"sceAmctrl_driver",0x525B8218,sceDrmBBMacInit
#endif
#ifdef F_sceAmctrl_driver_0004
	IMPORT_FUNC	"sceAmctrl_driver",0x58163FBE,sceDrmBBMacUpdate
#endif
#ifdef F_sceAmctrl_driver_0005
	IMPORT_FUNC	"sceAmctrl_driver",0x9951C50F,sceDrmBBCipherFinal
#endif
#ifdef F_sceAmctrl_driver_0006
	IMPORT_FUNC	"sceAmctrl_driver",0xEF95A213,sceDrmBBMacFinal
#endif
#ifdef F_sceAmctrl_driver_0007
	IMPORT_FUNC	"sceAmctrl_driver",0xF5186D8E,sceDrmBBMacFinal2
#endif
