	.set noreorder

#include "pspimport.s"

// Build List
// sceUsb_driver_0000.o sceUsb_driver_0001.o sceUsb_driver_0002.o sceUsb_driver_0003.o sceUsb_driver_0004.o sceUsb_driver_0005.o sceUsb_driver_0006.o sceUsb_driver_0007.o sceUsb_driver_0008.o sceUsb_driver_0009.o sceUsb_driver_0010.o sceUsb_driver_0011.o sceUsb_driver_0012.o 

#ifdef F_sceUsb_driver_0000
	IMPORT_START	"sceUsb_driver",0x00090011
#endif
#ifdef F_sceUsb_driver_0001
	IMPORT_FUNC	"sceUsb_driver",0x112CC951,sceUsbGetDrvState
#endif
#ifdef F_sceUsb_driver_0002
	IMPORT_FUNC	"sceUsb_driver",0x1C360735,sceUsbWaitCancel
#endif
#ifdef F_sceUsb_driver_0003
	IMPORT_FUNC	"sceUsb_driver",0x4E537366,sceUsbGetDrvList
#endif
#ifdef F_sceUsb_driver_0004
	IMPORT_FUNC	"sceUsb_driver",0x586DB82C,sceUsbActivate
#endif
#ifdef F_sceUsb_driver_0005
	IMPORT_FUNC	"sceUsb_driver",0x5BE0E002,sceUsbWaitState
#endif
#ifdef F_sceUsb_driver_0006
	IMPORT_FUNC	"sceUsb_driver",0x616F2B61,sceUsbWaitStateCB
#endif
#ifdef F_sceUsb_driver_0007
	IMPORT_FUNC	"sceUsb_driver",0xAE5DE6AF,sceUsbStart
#endif
#ifdef F_sceUsb_driver_0008
	IMPORT_FUNC	"sceUsb_driver",0xC21645A4,sceUsbGetState
#endif
#ifdef F_sceUsb_driver_0009
	IMPORT_FUNC	"sceUsb_driver",0xC2464FA0,sceUsbStop
#endif
#ifdef F_sceUsb_driver_0010
	IMPORT_FUNC	"sceUsb_driver",0xC572A9C8,sceUsbDeactivate
#endif
#ifdef F_sceUsb_driver_0011
	IMPORT_FUNC	"sceUsb_driver",0xE20B23A6,sceUsb_driver_E20B23A6
#endif
#ifdef F_sceUsb_driver_0012
	IMPORT_FUNC	"sceUsb_driver",0xEDA8A020,sceUsb_driver_EDA8A020
#endif
