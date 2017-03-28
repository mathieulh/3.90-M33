	.set noreorder

#include "pspimport.s"

// 1904  1CD8

// Build List
// ModuleMgrForKernel_0000.o ModuleMgrForKernel_0001.o ModuleMgrForKernel_0002.o ModuleMgrForKernel_0003.o ModuleMgrForKernel_0004.o ModuleMgrForKernel_0005.o ModuleMgrForKernel_0006.o ModuleMgrForKernel_0007.o ModuleMgrForKernel_0008.o ModuleMgrForKernel_0009.o ModuleMgrForKernel_0010.o ModuleMgrForKernel_0011.o ModuleMgrForKernel_0012.o ModuleMgrForKernel_0013.o ModuleMgrForKernel_0014.o ModuleMgrForKernel_0015.o ModuleMgrForKernel_0016.o ModuleMgrForKernel_0017.o ModuleMgrForKernel_0018.o ModuleMgrForKernel_0019.o ModuleMgrForKernel_0020.o ModuleMgrForKernel_0021.o ModuleMgrForKernel_0022.o ModuleMgrForKernel_0023.o ModuleMgrForKernel_0024.o ModuleMgrForKernel_0025.o ModuleMgrForKernel_0026.o ModuleMgrForKernel_0027.o ModuleMgrForKernel_0028.o ModuleMgrForKernel_0029.o ModuleMgrForKernel_0030.o ModuleMgrForKernel_0031.o ModuleMgrForKernel_0032.o ModuleMgrForKernel_0033.o ModuleMgrForKernel_0034.o ModuleMgrForKernel_0035.o ModuleMgrForKernel_0036.o ModuleMgrForKernel_0037.o ModuleMgrForKernel_0038.o ModuleMgrForKernel_0039.o ModuleMgrForKernel_0040.o ModuleMgrForKernel_0041.o ModuleMgrForKernel_0042.o ModuleMgrForKernel_0043.o ModuleMgrForKernel_0044.o ModuleMgrForKernel_0045.o ModuleMgrForKernel_0046.o ModuleMgrForKernel_0047.o ModuleMgrForKernel_0048.o 

#ifdef F_ModuleMgrForKernel_0000
	IMPORT_START	"ModuleMgrForKernel",0x00090011
#endif
#ifdef F_ModuleMgrForKernel_0001
	IMPORT_FUNC	"ModuleMgrForKernel",0x121349C9,sceKernelSelfStopUnloadModule
#endif
#ifdef F_ModuleMgrForKernel_0002
	IMPORT_FUNC	"ModuleMgrForKernel",0x1B03F834,sceKernelSearchModuleByAddress
#endif
#ifdef F_ModuleMgrForKernel_0003
	IMPORT_FUNC	"ModuleMgrForKernel",0x1D313DE9,sceKernelStartModule
#endif
#ifdef F_ModuleMgrForKernel_0004
	IMPORT_FUNC	"ModuleMgrForKernel",0x1F0F8DF2,sceKernelLoadModuleBufferForLoadExecBufferVSHUsbWlan
#endif
#ifdef F_ModuleMgrForKernel_0005
	IMPORT_FUNC	"ModuleMgrForKernel",0x23FAE71A,sceKernelLoadModuleBufferVSH
#endif
#ifdef F_ModuleMgrForKernel_0006
// New func?
	IMPORT_FUNC	"ModuleMgrForKernel",0x245B698D,ModuleMgrForKernel_245B698D
#endif
#ifdef F_ModuleMgrForKernel_0007
	IMPORT_FUNC	"ModuleMgrForKernel",0x2531910F,sceKernelLoadModuleBufferForLoadExecBufferVSHUsbWlanDebug
#endif
#ifdef F_ModuleMgrForKernel_0008
	IMPORT_FUNC	"ModuleMgrForKernel",0x25EDFE8C,sceKernelLoadModuleByID
#endif
#ifdef F_ModuleMgrForKernel_0009
	IMPORT_FUNC	"ModuleMgrForKernel",0x288070CC,ModuleMgrForKernel_25E1F458
#endif
#ifdef F_ModuleMgrForKernel_0010
// New func?
	IMPORT_FUNC	"ModuleMgrForKernel",0x2BE2B17E,ModuleMgrForKernel_2BE2B17E
#endif
#ifdef F_ModuleMgrForKernel_0011
	IMPORT_FUNC	"ModuleMgrForKernel",0x3B33BC14,sceKernelLoadModuleVSHByID
#endif
#ifdef F_ModuleMgrForKernel_0012
	IMPORT_FUNC	"ModuleMgrForKernel",0x3F92B3CA,sceKernelLoadModuleByIDBootInitConfig
#endif
#ifdef F_ModuleMgrForKernel_0013
	IMPORT_FUNC	"ModuleMgrForKernel",0x42ED1407,sceKernelLoadModuleForLoadExecVSHMs2
#endif
#ifdef F_ModuleMgrForKernel_0014
	IMPORT_FUNC	"ModuleMgrForKernel",0x4729151D,sceKernelQueryModuleInfo
#endif
#ifdef F_ModuleMgrForKernel_0015
	IMPORT_FUNC	"ModuleMgrForKernel",0x494900F3,sceKernelLoadModuleBootInitConfig
#endif
#ifdef F_ModuleMgrForKernel_0016
	IMPORT_FUNC	"ModuleMgrForKernel",0x4F52F0D6,sceKernelRebootBeforeForKernel
#endif
#ifdef F_ModuleMgrForKernel_0017
	IMPORT_FUNC	"ModuleMgrForKernel",0x50FD3802,sceKernelLoadModuleDeci
#endif
#ifdef F_ModuleMgrForKernel_0018
	IMPORT_FUNC	"ModuleMgrForKernel",0x51512315,sceKernelStopUnloadSelfModule
#endif
#ifdef F_ModuleMgrForKernel_0019
	IMPORT_FUNC	"ModuleMgrForKernel",0x51BC8985,sceKernelLoadModuleForLoadExecForUser
#endif
#ifdef F_ModuleMgrForKernel_0020
	IMPORT_FUNC	"ModuleMgrForKernel",0x5BE741EE,sceKernelGetModuleIdByAddress
#endif
#ifdef F_ModuleMgrForKernel_0021
	IMPORT_FUNC	"ModuleMgrForKernel",0x609FA413,sceKernelGetModuleIdList
#endif
#ifdef F_ModuleMgrForKernel_0022
	IMPORT_FUNC	"ModuleMgrForKernel",0x61129E61,ModuleMgrForKernel_2BE4807D
#endif
#ifdef F_ModuleMgrForKernel_0023
	IMPORT_FUNC	"ModuleMgrForKernel",0x70DB8A38,sceKernelSearchModuleByName
#endif
#ifdef F_ModuleMgrForKernel_0024
	IMPORT_FUNC	"ModuleMgrForKernel",0x7190BEFB,sceKernelLoadModuleBufferForRebootKernel
#endif
#ifdef F_ModuleMgrForKernel_0025
	IMPORT_FUNC	"ModuleMgrForKernel",0x797989AF,sceKernelLoadModuleForLoadExecVSHDiscUpdater
#endif
#ifdef F_ModuleMgrForKernel_0026
	IMPORT_FUNC	"ModuleMgrForKernel",0x82E217DD,sceKernelLoadModuleForLoadExecVSHMs3
#endif
#ifdef F_ModuleMgrForKernel_0027
	IMPORT_FUNC	"ModuleMgrForKernel",0x8FF98580,sceKernelGetModuleId
#endif
#ifdef F_ModuleMgrForKernel_0028
	IMPORT_FUNC	"ModuleMgrForKernel",0x931AE95E,sceKernelRebootBeforeForUser
#endif
#ifdef F_ModuleMgrForKernel_0029
	IMPORT_FUNC	"ModuleMgrForKernel",0x96817B71,sceKernelLoadModuleBuffer
#endif
#ifdef F_ModuleMgrForKernel_0030
	IMPORT_FUNC	"ModuleMgrForKernel",0x984F84F4,sceKernelLoadModuleBufferForExitGame
#endif
#ifdef F_ModuleMgrForKernel_0031
	IMPORT_FUNC	"ModuleMgrForKernel",0xA35EBDDF,sceKernelLoadModuleForLoadExecVSHMs5
#endif
#ifdef F_ModuleMgrForKernel_0032
	IMPORT_FUNC	"ModuleMgrForKernel",0xAE4F1DCF,sceKernelStopModule
#endif
#ifdef F_ModuleMgrForKernel_0033
	IMPORT_FUNC	"ModuleMgrForKernel",0xAE802E8F,sceKernelLoadModule
#endif
#ifdef F_ModuleMgrForKernel_0034
	IMPORT_FUNC	"ModuleMgrForKernel",0xB4173233,ModuleMgrForKernel_1B91F6EC
#endif
#ifdef F_ModuleMgrForKernel_0035
	IMPORT_FUNC	"ModuleMgrForKernel",0xB98CD891,sceKernelUnloadModule
#endif
#ifdef F_ModuleMgrForKernel_0036
	IMPORT_FUNC	"ModuleMgrForKernel",0xBDFEEC4F,sceKernelLoadModuleVSH
#endif
#ifdef F_ModuleMgrForKernel_0037
	IMPORT_FUNC	"ModuleMgrForKernel",0xC2EBF6EC,sceKernelLoadModuleForLoadExecVSHDiscDebug
#endif
#ifdef F_ModuleMgrForKernel_0038
	IMPORT_FUNC	"ModuleMgrForKernel",0xC8F0090D,sceKernelLoadModuleForLoadExecVSHDisc
#endif
#ifdef F_ModuleMgrForKernel_0039
	IMPORT_FUNC	"ModuleMgrForKernel",0xD543FAEB,sceKernelLoadModuleBufferForExitVSHKernel
#endif
#ifdef F_ModuleMgrForKernel_0040
	IMPORT_FUNC	"ModuleMgrForKernel",0xDE7EDA2C,sceKernelLoadModuleBufferBootInitConfig
#endif
#ifdef F_ModuleMgrForKernel_0041
	IMPORT_FUNC	"ModuleMgrForKernel",0xE2E2C6C6,ModuleMgrForKernel_EF7A7F02
#endif
#ifdef F_ModuleMgrForKernel_0042
	IMPORT_FUNC	"ModuleMgrForKernel",0xE4860ED1,sceKernelLoadModuleForLoadExecVSHMs1
#endif
#ifdef F_ModuleMgrForKernel_0043
	IMPORT_FUNC	"ModuleMgrForKernel",0xE88E4A10,sceKernelRebootPhaseForKernel
#endif
#ifdef F_ModuleMgrForKernel_0044
	IMPORT_FUNC	"ModuleMgrForKernel",0xE9048641,sceKernelLoadModuleForLoadExecVSHMs4
#endif
#ifdef F_ModuleMgrForKernel_0045
	IMPORT_FUNC	"ModuleMgrForKernel",0xEF4B241D,sceKernelLoadModuleToBlock
#endif
#ifdef F_ModuleMgrForKernel_0046
	IMPORT_FUNC	"ModuleMgrForKernel",0xF5FCCE17,sceKernelLoadModuleBufferForExitVSHVSH
#endif
#ifdef F_ModuleMgrForKernel_0047
// New func?
	IMPORT_FUNC	"ModuleMgrForKernel",0xF7E46A72,ModuleMgrForKernel_F7E46A72
#endif
#ifdef F_ModuleMgrForKernel_0048
	IMPORT_FUNC	"ModuleMgrForKernel",0xFD8B706F,sceKernelModuleMgrMode
#endif
