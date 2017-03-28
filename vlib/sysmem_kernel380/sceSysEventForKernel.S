	.set noreorder

#include "pspimport.s"

// Build List
// sceSysEventForKernel_0000.o sceSysEventForKernel_0001.o sceSysEventForKernel_0002.o sceSysEventForKernel_0003.o sceSysEventForKernel_0004.o sceSysEventForKernel_0005.o 

#ifdef F_sceSysEventForKernel_0000
	IMPORT_START	"sceSysEventForKernel",0x00010011
#endif
#ifdef F_sceSysEventForKernel_0001
	IMPORT_FUNC	"sceSysEventForKernel",0x36331294,sceKernelSysEventDispatch
#endif
#ifdef F_sceSysEventForKernel_0002
	IMPORT_FUNC	"sceSysEventForKernel",0x68D55505,sceKernelReferSysEventHandler
#endif
#ifdef F_sceSysEventForKernel_0003
	IMPORT_FUNC	"sceSysEventForKernel",0xAEB300AE,sceKernelIsRegisterSysEventHandler
#endif
#ifdef F_sceSysEventForKernel_0004
	IMPORT_FUNC	"sceSysEventForKernel",0xCD9E4BB5,sceKernelRegisterSysEventHandler
#endif
#ifdef F_sceSysEventForKernel_0005
	IMPORT_FUNC	"sceSysEventForKernel",0xD7D3FDCD,sceKernelUnregisterSysEventHandler
#endif
