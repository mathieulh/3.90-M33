	.set noreorder

#include "pspimport.s"

// Build files
// VshCtrlLib_0000.o VshCtrlLib_0001.o VshCtrlLib_0002.o 

#ifdef F_VshCtrlLib_0000
	IMPORT_START "VshCtrlLib",0x40090000
#endif
#ifdef F_VshCtrlLib_0001
	IMPORT_FUNC  "VshCtrlLib",0xFD26DA72,vctrlVSHRegisterVshMenu
#endif
#ifdef F_VshCtrlLib_0002
	IMPORT_FUNC  "VshCtrlLib",0xCD6B3913,vctrlVSHExitVSHMenu
#endif
