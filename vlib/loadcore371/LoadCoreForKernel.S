	.set noreorder

#include "pspimport.s"

// Build List
// LoadCoreForKernel_0000.o LoadCoreForKernel_0001.o LoadCoreForKernel_0002.o LoadCoreForKernel_0003.o LoadCoreForKernel_0004.o LoadCoreForKernel_0005.o LoadCoreForKernel_0006.o LoadCoreForKernel_0007.o LoadCoreForKernel_0008.o LoadCoreForKernel_0009.o LoadCoreForKernel_0010.o LoadCoreForKernel_0011.o LoadCoreForKernel_0012.o LoadCoreForKernel_0013.o LoadCoreForKernel_0014.o LoadCoreForKernel_0015.o LoadCoreForKernel_0016.o LoadCoreForKernel_0017.o LoadCoreForKernel_0018.o LoadCoreForKernel_0019.o LoadCoreForKernel_0020.o LoadCoreForKernel_0021.o LoadCoreForKernel_0022.o LoadCoreForKernel_0023.o LoadCoreForKernel_0024.o LoadCoreForKernel_0025.o LoadCoreForKernel_0026.o LoadCoreForKernel_0027.o LoadCoreForKernel_0028.o LoadCoreForKernel_0029.o LoadCoreForKernel_0030.o LoadCoreForKernel_0031.o LoadCoreForKernel_0032.o LoadCoreForKernel_0033.o LoadCoreForKernel_0034.o LoadCoreForKernel_0035.o LoadCoreForKernel_0036.o LoadCoreForKernel_0037.o LoadCoreForKernel_0038.o LoadCoreForKernel_0039.o LoadCoreForKernel_0040.o 

#ifdef F_LoadCoreForKernel_0000
	IMPORT_START	"LoadCoreForKernel",0x00010011
#endif
#ifdef F_LoadCoreForKernel_0001
	IMPORT_FUNC	"LoadCoreForKernel",0x05D915DB,sceKernelGetModuleIdListForKernel
#endif
#ifdef F_LoadCoreForKernel_0002
	IMPORT_FUNC	"LoadCoreForKernel",0x0B464512,sceKernelReleaseLibrary
#endif
#ifdef F_LoadCoreForKernel_0003
	IMPORT_FUNC	"LoadCoreForKernel",0x0DE1F600,sceKernelLinkLibraryEntriesForUser
#endif
#ifdef F_LoadCoreForKernel_0004
	IMPORT_FUNC	"LoadCoreForKernel",0x0E760DBA,sceKernelLinkLibraryEntries
#endif
#ifdef F_LoadCoreForKernel_0005
	IMPORT_FUNC	"LoadCoreForKernel",0x2952F5AC,sceKernelDcacheWBinvAll
#endif
#ifdef F_LoadCoreForKernel_0006
	IMPORT_FUNC	"LoadCoreForKernel",0x3BB7AC18,sceKernelLoadCoreMode
#endif
#ifdef F_LoadCoreForKernel_0007
	IMPORT_FUNC	"LoadCoreForKernel",0x4440853B,LoadCoreForKernel_4440853B
#endif
#ifdef F_LoadCoreForKernel_0008
	IMPORT_FUNC	"LoadCoreForKernel",0x5248A98F,sceKernelLoadModuleBootLoadCore
#endif
#ifdef F_LoadCoreForKernel_0009
	IMPORT_FUNC	"LoadCoreForKernel",0x52A86C21,sceKernelGetModuleFromUID
#endif
#ifdef F_LoadCoreForKernel_0010
	IMPORT_FUNC	"LoadCoreForKernel",0x54AB2675,sceKernelApplyPspRelSection
#endif
#ifdef F_LoadCoreForKernel_0011
	IMPORT_FUNC	"LoadCoreForKernel",0x5873A31F,sceKernelRegisterLibraryForUser
#endif
#ifdef F_LoadCoreForKernel_0012
	IMPORT_FUNC	"LoadCoreForKernel",0x616FCCCD,sceKernelSetBootCallbackLevel
#endif
#ifdef F_LoadCoreForKernel_0013
	IMPORT_FUNC	"LoadCoreForKernel",0x6B2371C2,sceKernelDeleteModule
#endif
#ifdef F_LoadCoreForKernel_0014
	IMPORT_FUNC	"LoadCoreForKernel",0x7068E6BA,sceKernelLoadExecutableObject
#endif
#ifdef F_LoadCoreForKernel_0015
	IMPORT_FUNC	"LoadCoreForKernel",0x74CF001A,sceKernelReleaseModule
#endif
#ifdef F_LoadCoreForKernel_0016
	IMPORT_FUNC	"LoadCoreForKernel",0x7BE1421C,sceKernelCheckExecFile
#endif
#ifdef F_LoadCoreForKernel_0017
	IMPORT_FUNC	"LoadCoreForKernel",0x7E63F86D,sceKernelGetLibraryLinkInfo
#endif
#ifdef F_LoadCoreForKernel_0018
	IMPORT_FUNC	"LoadCoreForKernel",0x8D8A8ACE,sceKernelAssignModule
#endif
#ifdef F_LoadCoreForKernel_0019
	IMPORT_FUNC	"LoadCoreForKernel",0x929B5C69,sceKernelGetModuleListWithAlloc
#endif
#ifdef F_LoadCoreForKernel_0020
	IMPORT_FUNC	"LoadCoreForKernel",0x99A695F0,sceKernelRegisterLibrary
#endif
#ifdef F_LoadCoreForKernel_0021
	IMPORT_FUNC	"LoadCoreForKernel",0x9BAF90F6,sceKernelCanReleaseLibrary
#endif
#ifdef F_LoadCoreForKernel_0022
	IMPORT_FUNC	"LoadCoreForKernel",0xA6D40F56,LoadCoreForKernel_A6D40F56
#endif
#ifdef F_LoadCoreForKernel_0023
	IMPORT_FUNC	"LoadCoreForKernel",0xA8E2D53D,LoadCoreForKernel_A8E2D53D
#endif
#ifdef F_LoadCoreForKernel_0024
	IMPORT_FUNC	"LoadCoreForKernel",0xACE23476,sceKernelCheckPspConfig
#endif
#ifdef F_LoadCoreForKernel_0025
	IMPORT_FUNC	"LoadCoreForKernel",0xAE7C6E76,sceKernelRegisterModule
#endif
#ifdef F_LoadCoreForKernel_0026
	IMPORT_FUNC	"LoadCoreForKernel",0xAFF947D4,sceKernelCreateAssignModule
#endif
#ifdef F_LoadCoreForKernel_0027
	IMPORT_FUNC	"LoadCoreForKernel",0xB370DF29,sceKernelGetLibraryList
#endif
#ifdef F_LoadCoreForKernel_0028
	IMPORT_FUNC	"LoadCoreForKernel",0xB4D6FECC,sceKernelApplyElfRelSection
#endif
#ifdef F_LoadCoreForKernel_0029
	IMPORT_FUNC	"LoadCoreForKernel",0xBF983EF2,sceKernelProbeExecutableObject
#endif
#ifdef F_LoadCoreForKernel_0030
	IMPORT_FUNC	"LoadCoreForKernel",0xC0913394,LoadCoreForKernel_C0913394
#endif
#ifdef F_LoadCoreForKernel_0031
	IMPORT_FUNC	"LoadCoreForKernel",0xC99DD47A,sceKernelQueryLoadCoreCB
#endif
#ifdef F_LoadCoreForKernel_0032
	IMPORT_FUNC	"LoadCoreForKernel",0xCCE4A157,sceKernelFindModuleByUID
#endif
#ifdef F_LoadCoreForKernel_0033
	IMPORT_FUNC	"LoadCoreForKernel",0xCD0F3BAC,sceKernelCreateModule
#endif
#ifdef F_LoadCoreForKernel_0034
	IMPORT_FUNC	"LoadCoreForKernel",0xCDCAA8B2,LoadCoreForKernel_CDCAA8B2
#endif
#ifdef F_LoadCoreForKernel_0035
	IMPORT_FUNC	"LoadCoreForKernel",0xCF8A41B1,sceKernelFindModuleByName
#endif
#ifdef F_LoadCoreForKernel_0036
	IMPORT_FUNC	"LoadCoreForKernel",0xD8779AC6,sceKernelIcacheClearAll
#endif
#ifdef F_LoadCoreForKernel_0037
	IMPORT_FUNC	"LoadCoreForKernel",0xDA1B09AA,sceKernelUnLinkLibraryEntries
#endif
#ifdef F_LoadCoreForKernel_0038
	IMPORT_FUNC	"LoadCoreForKernel",0xEC20C661,LoadCoreForKernel_EC20C661
#endif
#ifdef F_LoadCoreForKernel_0039
	IMPORT_FUNC	"LoadCoreForKernel",0xFA3101A4,LoadCoreForKernel_FA3101A4
#endif
#ifdef F_LoadCoreForKernel_0040
	IMPORT_FUNC	"LoadCoreForKernel",0xFB8AE27D,sceKernelFindModuleByAddress
#endif
