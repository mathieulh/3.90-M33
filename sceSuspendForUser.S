	.set noreorder

#include "pspimport.s"

// Build List
// sceSuspendForUser_0000.o sceSuspendForUser_0001.o sceSuspendForUser_0002.o sceSuspendForUser_0003.o sceSuspendForUser_0004.o sceSuspendForUser_0005.o sceSuspendForUser_0006.o 

#ifdef F_sceSuspendForUser_0000
	IMPORT_START	"sceSuspendForUser",0x40000011
#endif
#ifdef F_sceSuspendForUser_0001
	IMPORT_FUNC	"sceSuspendForUser",0x090CCB3F,sceSuspendForUser_090CCB3F
#endif
#ifdef F_sceSuspendForUser_0002
	IMPORT_FUNC	"sceSuspendForUser",0x3AEE7261,sceSuspendForUser_3AEE7261
#endif
#ifdef F_sceSuspendForUser_0003
	IMPORT_FUNC	"sceSuspendForUser",0x3E0271D3,sceSuspendForUser_3E0271D3
#endif
#ifdef F_sceSuspendForUser_0004
	IMPORT_FUNC	"sceSuspendForUser",0xA14F40B2,sceSuspendForUser_A14F40B2
#endif
#ifdef F_sceSuspendForUser_0005
	IMPORT_FUNC	"sceSuspendForUser",0xA569E425,sceSuspendForUser_A569E425
#endif
#ifdef F_sceSuspendForUser_0006
	IMPORT_FUNC	"sceSuspendForUser",0xEADB1BD7,sceSuspendForUser_EADB1BD7
#endif
