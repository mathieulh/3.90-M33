	.set noreorder

#include "pspimport.s"

// Build List
// sceUsbBus_driver_0000.o sceUsbBus_driver_0001.o sceUsbBus_driver_0002.o sceUsbBus_driver_0003.o sceUsbBus_driver_0004.o sceUsbBus_driver_0005.o sceUsbBus_driver_0006.o sceUsbBus_driver_0007.o sceUsbBus_driver_0008.o sceUsbBus_driver_0009.o sceUsbBus_driver_0010.o sceUsbBus_driver_0011.o sceUsbBus_driver_0012.o sceUsbBus_driver_0013.o sceUsbBus_driver_0014.o sceUsbBus_driver_0015.o 

#ifdef F_sceUsbBus_driver_0000
	IMPORT_START	"sceUsbBus_driver",0x00010011
#endif
#ifdef F_sceUsbBus_driver_0001
	IMPORT_FUNC	"sceUsbBus_driver",0x23E51D8F,sceUsbbdReqSend
#endif
#ifdef F_sceUsbBus_driver_0002
	IMPORT_FUNC	"sceUsbBus_driver",0x48CCE3C1,sceUsbBus_driver_48CCE3C1
#endif
#ifdef F_sceUsbBus_driver_0003
	IMPORT_FUNC	"sceUsbBus_driver",0x7B87815D,sceUsbBus_driver_7B87815D
#endif
#ifdef F_sceUsbBus_driver_0004
	IMPORT_FUNC	"sceUsbBus_driver",0x8A3EB5D2,sceUsbBus_driver_8A3EB5D2
#endif
#ifdef F_sceUsbBus_driver_0005
	IMPORT_FUNC	"sceUsbBus_driver",0x90B82F55,sceUsbBus_driver_90B82F55
#endif
#ifdef F_sceUsbBus_driver_0006
	IMPORT_FUNC	"sceUsbBus_driver",0x913EC15D,sceUsbbdReqRecv
#endif
#ifdef F_sceUsbBus_driver_0007
	IMPORT_FUNC	"sceUsbBus_driver",0x951A24CC,sceUsbbdClearFIFO
#endif
#ifdef F_sceUsbBus_driver_0008
	IMPORT_FUNC	"sceUsbBus_driver",0xB1644BE7,sceUsbbdRegister
#endif
#ifdef F_sceUsbBus_driver_0009
	IMPORT_FUNC	"sceUsbBus_driver",0xC1E2A540,sceUsbbdUnregister
#endif
#ifdef F_sceUsbBus_driver_0010
	IMPORT_FUNC	"sceUsbBus_driver",0xC21645A4,sceUsbGetState
#endif
#ifdef F_sceUsbBus_driver_0011
	IMPORT_FUNC	"sceUsbBus_driver",0xC5E53685,sceUsbbdReqCancelAll
#endif
#ifdef F_sceUsbBus_driver_0012
	IMPORT_FUNC	"sceUsbBus_driver",0xCC57EC9D,sceUsbbdReqCancel
#endif
#ifdef F_sceUsbBus_driver_0013
	IMPORT_FUNC	"sceUsbBus_driver",0xE65441C1,sceUsbbdStall
#endif
#ifdef F_sceUsbBus_driver_0014
	IMPORT_FUNC	"sceUsbBus_driver",0xEDA8A020,sceUsbBus_driver_EDA8A020
#endif
#ifdef F_sceUsbBus_driver_0015
	IMPORT_FUNC	"sceUsbBus_driver",0xFBA2072B,sceUsbBus_driver_FBA2072B
#endif
