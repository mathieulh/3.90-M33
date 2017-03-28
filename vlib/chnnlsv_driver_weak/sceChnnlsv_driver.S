	.set noreorder

#include "pspimport.s"

// Build List
// sceChnnlsv_driver_0000.o sceChnnlsv_driver_0001.o sceChnnlsv_driver_0002.o sceChnnlsv_driver_0003.o sceChnnlsv_driver_0004.o sceChnnlsv_driver_0005.o sceChnnlsv_driver_0006.o 

#ifdef F_sceChnnlsv_driver_0000
	IMPORT_START	"sceChnnlsv_driver",0x00090011
#endif
#ifdef F_sceChnnlsv_driver_0001
	IMPORT_FUNC	"sceChnnlsv_driver",0x21BE78B4,sceChnnlsv_driver_21BE78B4
#endif
#ifdef F_sceChnnlsv_driver_0002
	IMPORT_FUNC	"sceChnnlsv_driver",0x850A7FA1,sceChnnlsv_driver_850A7FA1
#endif
#ifdef F_sceChnnlsv_driver_0003
	IMPORT_FUNC	"sceChnnlsv_driver",0xABFDFC8B,sceChnnlsv_driver_ABFDFC8B
#endif
#ifdef F_sceChnnlsv_driver_0004
	IMPORT_FUNC	"sceChnnlsv_driver",0xC4C494F8,sceChnnlsv_driver_C4C494F8
#endif
#ifdef F_sceChnnlsv_driver_0005
	IMPORT_FUNC	"sceChnnlsv_driver",0xE7833020,sceChnnlsv_driver_E7833020
#endif
#ifdef F_sceChnnlsv_driver_0006
	IMPORT_FUNC	"sceChnnlsv_driver",0xF21A1FCA,sceChnnlsv_driver_F21A1FCA
#endif
