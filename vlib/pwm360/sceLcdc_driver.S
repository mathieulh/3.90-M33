	.set noreorder

#include "pspimport.s"

// Build List
// sceLcdc_driver_0000.o sceLcdc_driver_0001.o sceLcdc_driver_0002.o sceLcdc_driver_0003.o sceLcdc_driver_0004.o sceLcdc_driver_0005.o sceLcdc_driver_0006.o sceLcdc_driver_0007.o sceLcdc_driver_0008.o sceLcdc_driver_0009.o sceLcdc_driver_0010.o sceLcdc_driver_0011.o sceLcdc_driver_0012.o sceLcdc_driver_0013.o sceLcdc_driver_0014.o sceLcdc_driver_0015.o sceLcdc_driver_0016.o sceLcdc_driver_0017.o sceLcdc_driver_0018.o sceLcdc_driver_0019.o sceLcdc_driver_0020.o 

#ifdef F_sceLcdc_driver_0000
	IMPORT_START	"sceLcdc_driver",0x00010011
#endif
#ifdef F_sceLcdc_driver_0001
	IMPORT_FUNC	"sceLcdc_driver",0x00EF7E87,sceLcdcSetHOffset
#endif
#ifdef F_sceLcdc_driver_0002
	IMPORT_FUNC	"sceLcdc_driver",0x0BC2B5E2,sceLcdcSetMode
#endif
#ifdef F_sceLcdc_driver_0003
	IMPORT_FUNC	"sceLcdc_driver",0x0E8E8774,sceLcdcEnd
#endif
#ifdef F_sceLcdc_driver_0004
	IMPORT_FUNC	"sceLcdc_driver",0x236095AC,sceLcdcGetLcdcClockFreq
#endif
#ifdef F_sceLcdc_driver_0005
	IMPORT_FUNC	"sceLcdc_driver",0x3107417C,sceLcdcGetVsyncFreq
#endif
#ifdef F_sceLcdc_driver_0006
	IMPORT_FUNC	"sceLcdc_driver",0x551C5BC3,sceLcdcInsertDisplay_default
#endif
#ifdef F_sceLcdc_driver_0007
	IMPORT_FUNC	"sceLcdc_driver",0x73A3A01D,sceLcdcCheckMode
#endif
#ifdef F_sceLcdc_driver_0008
	IMPORT_FUNC	"sceLcdc_driver",0x7BDC15C8,sceLcdcReadVPC
#endif
#ifdef F_sceLcdc_driver_0009
	IMPORT_FUNC	"sceLcdc_driver",0x825DB9C5,sceLcdc_driver_825DB9C5
#endif
#ifdef F_sceLcdc_driver_0010
	IMPORT_FUNC	"sceLcdc_driver",0x965D1633,sceLcdcGetPixelClockFreq
#endif
#ifdef F_sceLcdc_driver_0011
	IMPORT_FUNC	"sceLcdc_driver",0xA0032C3D,sceLcdcDisable
#endif
#ifdef F_sceLcdc_driver_0012
	IMPORT_FUNC	"sceLcdc_driver",0xA182B32C,sceLcdcEnable
#endif
#ifdef F_sceLcdc_driver_0013
	IMPORT_FUNC	"sceLcdc_driver",0xA2E70DA6,sceLcdcReset
#endif
#ifdef F_sceLcdc_driver_0014
	IMPORT_FUNC	"sceLcdc_driver",0xB3BA421A,sceLcdcGetHsyncFreq
#endif
#ifdef F_sceLcdc_driver_0015
	IMPORT_FUNC	"sceLcdc_driver",0xB55500A3,sceLcdcInit
#endif
#ifdef F_sceLcdc_driver_0016
	IMPORT_FUNC	"sceLcdc_driver",0xC6F10C77,sceLcdcResume
#endif
#ifdef F_sceLcdc_driver_0017
	IMPORT_FUNC	"sceLcdc_driver",0xDCD51769,sceLcdcSuspend
#endif
#ifdef F_sceLcdc_driver_0018
	IMPORT_FUNC	"sceLcdc_driver",0xE08B076B,sceLcdc_driver_E08B076B
#endif
#ifdef F_sceLcdc_driver_0019
	IMPORT_FUNC	"sceLcdc_driver",0xF6F413C5,sceLcdcGetMode
#endif
#ifdef F_sceLcdc_driver_0020
	IMPORT_FUNC	"sceLcdc_driver",0xFE5A8859,sceLcdcReadHPC
#endif
