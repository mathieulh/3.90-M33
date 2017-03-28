	.set noreorder

#include "pspimport.s"

// Build List
// sceGpio_driver_0000.o sceGpio_driver_0001.o sceGpio_driver_0002.o sceGpio_driver_0003.o sceGpio_driver_0004.o sceGpio_driver_0005.o sceGpio_driver_0006.o sceGpio_driver_0007.o sceGpio_driver_0008.o sceGpio_driver_0009.o sceGpio_driver_0010.o sceGpio_driver_0011.o sceGpio_driver_0012.o sceGpio_driver_0013.o sceGpio_driver_0014.o sceGpio_driver_0015.o sceGpio_driver_0016.o sceGpio_driver_0017.o sceGpio_driver_0018.o sceGpio_driver_0019.o sceGpio_driver_0020.o 

#ifdef F_sceGpio_driver_0000
	IMPORT_START	"sceGpio_driver",0x00010011
#endif
#ifdef F_sceGpio_driver_0001
	IMPORT_FUNC	"sceGpio_driver",0x103C3EB2,sceGpioPortClear
#endif
#ifdef F_sceGpio_driver_0002
	IMPORT_FUNC	"sceGpio_driver",0x17DAA8C2,sceGpioSuspend
#endif
#ifdef F_sceGpio_driver_0003
	IMPORT_FUNC	"sceGpio_driver",0x2CDC8EDC,sceGpio_driver_2CDC8EDC
#endif
#ifdef F_sceGpio_driver_0004
	IMPORT_FUNC	"sceGpio_driver",0x310F0CCF,sceGpioPortSet
#endif
#ifdef F_sceGpio_driver_0005
	IMPORT_FUNC	"sceGpio_driver",0x317D9D2C,sceGpioSetPortMode
#endif
#ifdef F_sceGpio_driver_0006
	IMPORT_FUNC	"sceGpio_driver",0x31F34AE6,sceGpioQueryIntr
#endif
#ifdef F_sceGpio_driver_0007
	IMPORT_FUNC	"sceGpio_driver",0x37C8DADC,sceGpioSetIntrMode
#endif
#ifdef F_sceGpio_driver_0008
	IMPORT_FUNC	"sceGpio_driver",0x4250D44A,sceGpioPortRead
#endif
#ifdef F_sceGpio_driver_0009
	IMPORT_FUNC	"sceGpio_driver",0x4A992B24,sceGpioEnd
#endif
#ifdef F_sceGpio_driver_0010
	IMPORT_FUNC	"sceGpio_driver",0x5691CEFA,sceGpio_driver_5691CEFA
#endif
#ifdef F_sceGpio_driver_0011
	IMPORT_FUNC	"sceGpio_driver",0x64CD4536,sceGpioResume
#endif
#ifdef F_sceGpio_driver_0012
	IMPORT_FUNC	"sceGpio_driver",0x6B38B826,sceGpio_driver_6B38B826
#endif
#ifdef F_sceGpio_driver_0013
	IMPORT_FUNC	"sceGpio_driver",0x785206CD,sceGpioEnableIntr
#endif
#ifdef F_sceGpio_driver_0014
	IMPORT_FUNC	"sceGpio_driver",0x95135905,sceGpio_driver_95135905
#endif
#ifdef F_sceGpio_driver_0015
	IMPORT_FUNC	"sceGpio_driver",0x95D7F3B8,sceGpioDisableIntr
#endif
#ifdef F_sceGpio_driver_0016
	IMPORT_FUNC	"sceGpio_driver",0xBE77D1D0,sceGpioAcquireIntr
#endif
#ifdef F_sceGpio_driver_0017
	IMPORT_FUNC	"sceGpio_driver",0xC6928224,sceGpio_driver_C6928224
#endif
#ifdef F_sceGpio_driver_0018
	IMPORT_FUNC	"sceGpio_driver",0xCA8BE2EA,sceGpioGetPortMode
#endif
#ifdef F_sceGpio_driver_0019
	IMPORT_FUNC	"sceGpio_driver",0xEABDB328,sceGpioInit
#endif
#ifdef F_sceGpio_driver_0020
	IMPORT_FUNC	"sceGpio_driver",0xF856CE46,sceGpioGetIntrMode
#endif
