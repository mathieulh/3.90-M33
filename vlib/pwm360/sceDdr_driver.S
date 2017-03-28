	.set noreorder

#include "pspimport.s"

// Build List
// sceDdr_driver_0000.o sceDdr_driver_0001.o sceDdr_driver_0002.o sceDdr_driver_0003.o sceDdr_driver_0004.o sceDdr_driver_0005.o sceDdr_driver_0006.o sceDdr_driver_0007.o sceDdr_driver_0008.o sceDdr_driver_0009.o sceDdr_driver_0010.o sceDdr_driver_0011.o sceDdr_driver_0012.o sceDdr_driver_0013.o sceDdr_driver_0014.o sceDdr_driver_0015.o sceDdr_driver_0016.o sceDdr_driver_0017.o sceDdr_driver_0018.o 

#ifdef F_sceDdr_driver_0000
	IMPORT_START	"sceDdr_driver",0x00010011
#endif
#ifdef F_sceDdr_driver_0001
	IMPORT_FUNC	"sceDdr_driver",0x0DC43DE4,sceDdr_driver_0DC43DE4
#endif
#ifdef F_sceDdr_driver_0002
	IMPORT_FUNC	"sceDdr_driver",0x17D39E17,sceDdrInit
#endif
#ifdef F_sceDdr_driver_0003
	IMPORT_FUNC	"sceDdr_driver",0x286E1372,sceDdr_driver_286E1372
#endif
#ifdef F_sceDdr_driver_0004
	IMPORT_FUNC	"sceDdr_driver",0x4CE55E76,sceDdrChangePllClock
#endif
#ifdef F_sceDdr_driver_0005
	IMPORT_FUNC	"sceDdr_driver",0x5B5831E5,sceDdr_driver_5B5831E5
#endif
#ifdef F_sceDdr_driver_0006
	IMPORT_FUNC	"sceDdr_driver",0x6078F911,sceDdr_driver_6078F911
#endif
#ifdef F_sceDdr_driver_0007
	IMPORT_FUNC	"sceDdr_driver",0x623A233F,sceDdrSuspend
#endif
#ifdef F_sceDdr_driver_0008
	IMPORT_FUNC	"sceDdr_driver",0x655A9AB0,sceDdrWriteMaxAllocate
#endif
#ifdef F_sceDdr_driver_0009
	IMPORT_FUNC	"sceDdr_driver",0x6955346C,sceDdr_driver_6955346C
#endif
#ifdef F_sceDdr_driver_0010
	IMPORT_FUNC	"sceDdr_driver",0x7251F5AB,sceDdr_driver_7251F5AB
#endif
#ifdef F_sceDdr_driver_0011
	IMPORT_FUNC	"sceDdr_driver",0x8698F5DA,sceDdrEnd
#endif
#ifdef F_sceDdr_driver_0012
	IMPORT_FUNC	"sceDdr_driver",0x87D86769,sceDdrFlush
#endif
#ifdef F_sceDdr_driver_0013
	IMPORT_FUNC	"sceDdr_driver",0x888129E8,sceDdr_driver_888129E8
#endif
#ifdef F_sceDdr_driver_0014
	IMPORT_FUNC	"sceDdr_driver",0x91CD8F94,sceDdrResetDevice
#endif
#ifdef F_sceDdr_driver_0015
	IMPORT_FUNC	"sceDdr_driver",0x92289EC0,sceDdrResume
#endif
#ifdef F_sceDdr_driver_0016
	IMPORT_FUNC	"sceDdr_driver",0xB5694ECD,sceDdr_driver_B5694ECD
#endif
#ifdef F_sceDdr_driver_0017
	IMPORT_FUNC	"sceDdr_driver",0xD7C6C313,sceDdrSetup
#endif
#ifdef F_sceDdr_driver_0018
	IMPORT_FUNC	"sceDdr_driver",0xF23B7983,sceDdr_driver_F23B7983
#endif
