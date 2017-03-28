	.set noreorder

#include "pspimport.s"

// Build files
// pspUsbDevice_driver_0000.o pspUsbDevice_driver_0001.o pspUsbDevice_driver_0002.o 

#ifdef F_pspUsbDevice_driver_0000
	IMPORT_START "pspUsbDevice_driver",0x00090000
#endif
#ifdef F_pspUsbDevice_driver_0001
	IMPORT_FUNC  "pspUsbDevice_driver",0xD4D90520,pspUsbDeviceSetDevice
#endif
#ifdef F_pspUsbDevice_driver_0002
	IMPORT_FUNC  "pspUsbDevice_driver",0xB3CF0329,pspUsbDeviceFinishDevice
#endif
