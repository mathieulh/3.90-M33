	.set noreorder

#include "pspimport.s"

// Build List
// sceI2c_driver_0000.o sceI2c_driver_0001.o sceI2c_driver_0002.o sceI2c_driver_0003.o sceI2c_driver_0004.o sceI2c_driver_0005.o sceI2c_driver_0006.o sceI2c_driver_0007.o sceI2c_driver_0008.o sceI2c_driver_0009.o sceI2c_driver_0010.o sceI2c_driver_0011.o 

#ifdef F_sceI2c_driver_0000
	IMPORT_START	"sceI2c_driver",0x00010011
#endif
#ifdef F_sceI2c_driver_0001
	IMPORT_FUNC	"sceI2c_driver",0x0397408B,sceI2c_driver_0397408B
#endif
#ifdef F_sceI2c_driver_0002
	IMPORT_FUNC	"sceI2c_driver",0x17B0DA59,sceI2cReset
#endif
#ifdef F_sceI2c_driver_0003
	IMPORT_FUNC	"sceI2c_driver",0x20ADF12A,sceI2cInit
#endif
#ifdef F_sceI2c_driver_0004
	IMPORT_FUNC	"sceI2c_driver",0x21A8282C,sceI2cSuspend
#endif
#ifdef F_sceI2c_driver_0005
	IMPORT_FUNC	"sceI2c_driver",0x4809D85F,sceI2cEnd
#endif
#ifdef F_sceI2c_driver_0006
	IMPORT_FUNC	"sceI2c_driver",0x6536B1B6,sceI2cMasterTransmitReceive
#endif
#ifdef F_sceI2c_driver_0007
	IMPORT_FUNC	"sceI2c_driver",0xB952BA65,sceI2cResume
#endif
#ifdef F_sceI2c_driver_0008
	IMPORT_FUNC	"sceI2c_driver",0xD6E9836A,sceI2cSetClock
#endif
#ifdef F_sceI2c_driver_0009
	IMPORT_FUNC	"sceI2c_driver",0xDB6C4F9B,sceI2c_driver_DB6C4F9B
#endif
#ifdef F_sceI2c_driver_0010
	IMPORT_FUNC	"sceI2c_driver",0xE9463B99,sceI2cMasterTransmit
#endif
#ifdef F_sceI2c_driver_0011
	IMPORT_FUNC	"sceI2c_driver",0xF87D1ED6,sceI2cMasterReceive
#endif
