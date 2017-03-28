	.set noreorder

#include "pspimport.s"

// Build List
// sceSuspendForKernel_0000.o sceSuspendForKernel_0001.o sceSuspendForKernel_0002.o sceSuspendForKernel_0003.o sceSuspendForKernel_0004.o sceSuspendForKernel_0005.o sceSuspendForKernel_0006.o sceSuspendForKernel_0007.o sceSuspendForKernel_0008.o sceSuspendForKernel_0009.o sceSuspendForKernel_0010.o sceSuspendForKernel_0011.o sceSuspendForKernel_0012.o sceSuspendForKernel_0013.o sceSuspendForKernel_0014.o sceSuspendForKernel_0015.o sceSuspendForKernel_0016.o 

#ifdef F_sceSuspendForKernel_0000
	IMPORT_START	"sceSuspendForKernel",0x00010011
#endif
#ifdef F_sceSuspendForKernel_0001
	IMPORT_FUNC	"sceSuspendForKernel",0x090CCB3F,sceKernelPowerTick
#endif
#ifdef F_sceSuspendForKernel_0002
	IMPORT_FUNC	"sceSuspendForKernel",0x0AB0C6F3,sceKernelDispatchResumeHandlers
#endif
#ifdef F_sceSuspendForKernel_0003
	IMPORT_FUNC	"sceSuspendForKernel",0x3AEE7261,sceKernelPowerUnlock
#endif
#ifdef F_sceSuspendForKernel_0004
	IMPORT_FUNC	"sceSuspendForKernel",0x3E0271D3,sceKernelVolatileMemLock
#endif
#ifdef F_sceSuspendForKernel_0005
	IMPORT_FUNC	"sceSuspendForKernel",0x67B59042,sceSuspendForKernel_67B59042
#endif
#ifdef F_sceSuspendForKernel_0006
	IMPORT_FUNC	"sceSuspendForKernel",0x8F58B1EC,sceKernelDispatchSuspendHandlers
#endif
#ifdef F_sceSuspendForKernel_0007
	IMPORT_FUNC	"sceSuspendForKernel",0x91A77137,sceKernelRegisterSuspendHandler
#endif
#ifdef F_sceSuspendForKernel_0008
	IMPORT_FUNC	"sceSuspendForKernel",0x98A1D061,sceKernelPowerRebootStart
#endif
#ifdef F_sceSuspendForKernel_0009
	IMPORT_FUNC	"sceSuspendForKernel",0xA14F40B2,sceKernelVolatileMemTryLock
#endif
#ifdef F_sceSuspendForKernel_0010
	IMPORT_FUNC	"sceSuspendForKernel",0xA569E425,sceKernelVolatileMemUnlock
#endif
#ifdef F_sceSuspendForKernel_0011
	IMPORT_FUNC	"sceSuspendForKernel",0xB2C9640B,sceSuspendForKernel_B2C9640B
#endif
#ifdef F_sceSuspendForKernel_0012
	IMPORT_FUNC	"sceSuspendForKernel",0xB43D1A8C,sceKernelRegisterResumeHandler
#endif
#ifdef F_sceSuspendForKernel_0013
	IMPORT_FUNC	"sceSuspendForKernel",0xB53B2147,sceKernelPowerLockForUser
#endif
#ifdef F_sceSuspendForKernel_0014
	IMPORT_FUNC	"sceSuspendForKernel",0xBDE686CD,sceKernelRegisterPowerHandlers
#endif
#ifdef F_sceSuspendForKernel_0015
	IMPORT_FUNC	"sceSuspendForKernel",0xC7C928C7,sceKernelPowerUnlockForUser
#endif
#ifdef F_sceSuspendForKernel_0016
	IMPORT_FUNC	"sceSuspendForKernel",0xEADB1BD7,sceKernelPowerLock
#endif
