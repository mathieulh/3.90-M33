	.set noreorder

#include "pspimport.s"

// Build List
// ModuleMgrForUser_0000.o ModuleMgrForUser_0001.o ModuleMgrForUser_0002.o ModuleMgrForUser_0003.o ModuleMgrForUser_0004.o ModuleMgrForUser_0005.o ModuleMgrForUser_0006.o ModuleMgrForUser_0007.o ModuleMgrForUser_0008.o ModuleMgrForUser_0009.o ModuleMgrForUser_0010.o ModuleMgrForUser_0011.o ModuleMgrForUser_0012.o ModuleMgrForUser_0013.o ModuleMgrForUser_0014.o ModuleMgrForUser_0015.o ModuleMgrForUser_0016.o ModuleMgrForUser_0017.o ModuleMgrForUser_0018.o ModuleMgrForUser_0019.o ModuleMgrForUser_0020.o ModuleMgrForUser_0021.o 

#ifdef F_ModuleMgrForUser_0000
	IMPORT_START	"ModuleMgrForUser",0x40010011
#endif
#ifdef F_ModuleMgrForUser_0001
	IMPORT_FUNC	"ModuleMgrForUser",0x1196472E,ModuleMgrForUser_1196472E
#endif
#ifdef F_ModuleMgrForUser_0002
	IMPORT_FUNC	"ModuleMgrForUser",0x24EC0641,ModuleMgrForUser_24EC0641
#endif
#ifdef F_ModuleMgrForUser_0003
	IMPORT_FUNC	"ModuleMgrForUser",0x2E0911AA,sceKernelUnloadModule
#endif
#ifdef F_ModuleMgrForUser_0004
	IMPORT_FUNC	"ModuleMgrForUser",0x50F0C1EC,sceKernelStartModule
#endif
#ifdef F_ModuleMgrForUser_0005
	IMPORT_FUNC	"ModuleMgrForUser",0x644395E2,sceKernelGetModuleIdList
#endif
#ifdef F_ModuleMgrForUser_0006
	IMPORT_FUNC	"ModuleMgrForUser",0x710F61B5,sceKernelLoadModuleMs
#endif
#ifdef F_ModuleMgrForUser_0007
	IMPORT_FUNC	"ModuleMgrForUser",0x748CBED9,sceKernelQueryModuleInfo
#endif
#ifdef F_ModuleMgrForUser_0008
	IMPORT_FUNC	"ModuleMgrForUser",0x8F2DF740,ModuleMgrForUser_8F2DF740
#endif
#ifdef F_ModuleMgrForUser_0009
	IMPORT_FUNC	"ModuleMgrForUser",0x977DE386,sceKernelLoadModule
#endif
#ifdef F_ModuleMgrForUser_0010
	IMPORT_FUNC	"ModuleMgrForUser",0xB7F46618,sceKernelLoadModuleByID
#endif
#ifdef F_ModuleMgrForUser_0011
	IMPORT_FUNC	"ModuleMgrForUser",0xCC1D3699,sceKernelStopUnloadSelfModule
#endif
#ifdef F_ModuleMgrForUser_0012
	IMPORT_FUNC	"ModuleMgrForUser",0xD1FF982A,sceKernelStopModule
#endif
#ifdef F_ModuleMgrForUser_0013
	IMPORT_FUNC	"ModuleMgrForUser",0xD2FBC957,sceKernelGetModuleGPByAddress
#endif
#ifdef F_ModuleMgrForUser_0014
	IMPORT_FUNC	"ModuleMgrForUser",0xD675EBB8,sceKernelSelfStopUnloadModule
#endif
#ifdef F_ModuleMgrForUser_0015
	IMPORT_FUNC	"ModuleMgrForUser",0xD8B73127,sceKernelGetModuleIdByAddress
#endif
#ifdef F_ModuleMgrForUser_0016
	IMPORT_FUNC	"ModuleMgrForUser",0xE4C4211C,ModuleMgrForUser_E4C4211C
#endif
#ifdef F_ModuleMgrForUser_0017
	IMPORT_FUNC	"ModuleMgrForUser",0xF0A26395,sceKernelGetModuleId
#endif
#ifdef F_ModuleMgrForUser_0018
	IMPORT_FUNC	"ModuleMgrForUser",0xF2D8D1B4,ModuleMgrForUser_F2D8D1B4
#endif
#ifdef F_ModuleMgrForUser_0019
	IMPORT_FUNC	"ModuleMgrForUser",0xF9275D98,sceKernelLoadModuleBufferUsbWlan
#endif
#ifdef F_ModuleMgrForUser_0020
	IMPORT_FUNC	"ModuleMgrForUser",0xFBE27467,ModuleMgrForUser_FBE27467
#endif
#ifdef F_ModuleMgrForUser_0021
	IMPORT_FUNC	"ModuleMgrForUser",0xFEF27DC1,ModuleMgrForUser_FEF27DC1
#endif
