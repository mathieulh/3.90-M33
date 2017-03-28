	.set noreorder

#include "pspimport.s"

// Build List
// StdioForKernel_0000.o StdioForKernel_0001.o StdioForKernel_0002.o StdioForKernel_0003.o StdioForKernel_0004.o StdioForKernel_0005.o StdioForKernel_0006.o StdioForKernel_0007.o StdioForKernel_0008.o StdioForKernel_0009.o StdioForKernel_0010.o StdioForKernel_0011.o StdioForKernel_0012.o StdioForKernel_0013.o StdioForKernel_0014.o StdioForKernel_0015.o StdioForKernel_0016.o StdioForKernel_0017.o StdioForKernel_0018.o StdioForKernel_0019.o 

#ifdef F_StdioForKernel_0000
	IMPORT_START	"StdioForKernel",0x00010011
#endif
#ifdef F_StdioForKernel_0001
	IMPORT_FUNC	"StdioForKernel",0x11A5127A,fdgets
#endif
#ifdef F_StdioForKernel_0002
	IMPORT_FUNC	"StdioForKernel",0x172D316E,sceKernelStdin
#endif
#ifdef F_StdioForKernel_0003
	IMPORT_FUNC	"StdioForKernel",0x2CCF071A,fdprintf
#endif
#ifdef F_StdioForKernel_0004
	IMPORT_FUNC	"StdioForKernel",0x2D8551AB,sceKernelStdoutReset
#endif
#ifdef F_StdioForKernel_0005
	IMPORT_FUNC	"StdioForKernel",0x36B23B8B,fdputs
#endif
#ifdef F_StdioForKernel_0006
	IMPORT_FUNC	"StdioForKernel",0x432D8F5C,sceKernelRegisterStdoutPipe
#endif
#ifdef F_StdioForKernel_0007
	IMPORT_FUNC	"StdioForKernel",0x4F78930A,fdputc
#endif
#ifdef F_StdioForKernel_0008
	IMPORT_FUNC	"StdioForKernel",0x6F797E03,sceKernelRegisterStderrPipe
#endif
#ifdef F_StdioForKernel_0009
	IMPORT_FUNC	"StdioForKernel",0x7E338487,getchar
#endif
#ifdef F_StdioForKernel_0010
	IMPORT_FUNC	"StdioForKernel",0x9662BF86,sceKernelStderrReset
#endif
#ifdef F_StdioForKernel_0011
	IMPORT_FUNC	"StdioForKernel",0x98220F3E,sceKernelStdoutReopen
#endif
#ifdef F_StdioForKernel_0012
	IMPORT_FUNC	"StdioForKernel",0xA6BAB2E9,sceKernelStdout
#endif
#ifdef F_StdioForKernel_0013
	IMPORT_FUNC	"StdioForKernel",0xBFF7E760,gets
#endif
#ifdef F_StdioForKernel_0014
	IMPORT_FUNC	"StdioForKernel",0xCAB439DF,printf
#endif
#ifdef F_StdioForKernel_0015
	IMPORT_FUNC	"StdioForKernel",0xD2B2A2A7,fdgetc
#endif
#ifdef F_StdioForKernel_0016
	IMPORT_FUNC	"StdioForKernel",0xD768752A,putchar
#endif
#ifdef F_StdioForKernel_0017
	IMPORT_FUNC	"StdioForKernel",0xD97C8CB9,puts
#endif
#ifdef F_StdioForKernel_0018
	IMPORT_FUNC	"StdioForKernel",0xF78BA90A,sceKernelStderr
#endif
#ifdef F_StdioForKernel_0019
	IMPORT_FUNC	"StdioForKernel",0xFB5380C5,sceKernelStderrReopen
#endif
