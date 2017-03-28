	.set noreorder

#include "pspimport.s"

// Build List
// LoadExecForUser_0000.o LoadExecForUser_0001.o LoadExecForUser_0002.o LoadExecForUser_0003.o LoadExecForUser_0004.o LoadExecForUser_0005.o LoadExecForUser_0006.o 

#ifdef F_LoadExecForUser_0000
	IMPORT_START	"LoadExecForUser",0x40010011
#endif
#ifdef F_LoadExecForUser_0001
	IMPORT_FUNC	"LoadExecForUser",0x05572A5F,sceKernelExitGame
#endif
#ifdef F_LoadExecForUser_0002
	IMPORT_FUNC	"LoadExecForUser",0x2AC9954B,sceKernelExitGameWithStatus
#endif
#ifdef F_LoadExecForUser_0003
	IMPORT_FUNC	"LoadExecForUser",0x4AC57943,sceKernelRegisterExitCallback
#endif
#ifdef F_LoadExecForUser_0004
	IMPORT_FUNC	"LoadExecForUser",0x8ADA38D3,LoadExecForUser_8ADA38D3
#endif
#ifdef F_LoadExecForUser_0005
	IMPORT_FUNC	"LoadExecForUser",0xBD2F1094,sceKernelLoadExec
#endif
#ifdef F_LoadExecForUser_0006
	IMPORT_FUNC	"LoadExecForUser",0xD1FB50DC,LoadExecForUser_D1FB50DC
#endif
