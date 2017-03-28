	.set noreorder

#include "pspimport.s"

// Build files
// pspUsbDevice_0000.o pspUsbDevice_0001.o pspUsbDevice_0002.o 

#ifdef F_pspUsbDevice_0000
	IMPORT_START "pspUsbDevice",0x40090000
#endif
#ifdef F_pspUsbDevice_0001
	IMPORT_FUNC  "pspUsbDevice",0xD4D90520,pspUsbDeviceSetDevice
#endif
#ifdef F_pspUsbDevice_0002
	IMPORT_FUNC  "pspUsbDevice",0xB3CF0329,pspUsbDeviceFinishDevice
#endif
