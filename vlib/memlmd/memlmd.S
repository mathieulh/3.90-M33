	.set noreorder

#include "pspimport.s"

// Build List
// memlmd_0000.o memlmd_0001.o memlmd_0002.o memlmd_0003.o memlmd_0004.o memlmd_0005.o memlmd_0006.o memlmd_0007.o memlmd_0008.o memlmd_0009.o 

#ifdef F_memlmd_0000
	IMPORT_START	"memlmd",0x00090011
#endif
#ifdef F_memlmd_0001
	IMPORT_FUNC	"memlmd",0x1570BAB4,memlmd_1570BAB4
#endif
#ifdef F_memlmd_0002
	IMPORT_FUNC	"memlmd",0x30132606,memlmd_30132606
#endif
#ifdef F_memlmd_0003
	IMPORT_FUNC	"memlmd",0x323366CA,memlmd_323366CA
#endif
#ifdef F_memlmd_0004
	IMPORT_FUNC	"memlmd",0x3E76F7E7,memlmd_3E76F7E7
#endif
#ifdef F_memlmd_0005
	IMPORT_FUNC	"memlmd",0x436FC37A,memlmd_436FC37A
#endif
#ifdef F_memlmd_0006
	IMPORT_FUNC	"memlmd",0x6A811217,memlmd_6A811217
#endif
#ifdef F_memlmd_0007
	IMPORT_FUNC	"memlmd",0x7CF1CD3E,memlmd_7CF1CD3E
#endif
#ifdef F_memlmd_0008
	IMPORT_FUNC	"memlmd",0xD0071311,memlmd_D0071311
#endif
#ifdef F_memlmd_0009
	IMPORT_FUNC	"memlmd",0xD1732BE4,memlmd_D1732BE4
#endif
