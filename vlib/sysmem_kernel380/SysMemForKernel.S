	.set noreorder

#include "pspimport.s"

// Build List


#ifdef F_SysMemForKernel_0000
	IMPORT_START	"SysMemForKernel",0x00010011
#endif
#ifdef F_SysMemForKernel_0001
	IMPORT_FUNC	"SysMemForKernel",0x012071B7,SysMemForKernel_915EF4AC
#endif
#ifdef F_SysMemForKernel_0002
	IMPORT_FUNC	"SysMemForKernel",0x03F11C51,sceKernelHoldUID
#endif
#ifdef F_SysMemForKernel_0003
	IMPORT_FUNC	"SysMemForKernel",0x06BB6385,sceKernelSetDNAS
#endif
#ifdef F_SysMemForKernel_0004
	IMPORT_FUNC	"SysMemForKernel",0x079C9E14,sceKernelDeleteHeap
#endif
#ifdef F_SysMemForKernel_0005
	IMPORT_FUNC	"SysMemForKernel",0x120433CF,sceKernelGetUIDcontrolBlockWithType
#endif
#ifdef F_SysMemForKernel_0006
	IMPORT_FUNC	"SysMemForKernel",0x132FBBEE,sceKernelSearchUIDbyName
#endif
#ifdef F_SysMemForKernel_0007
	IMPORT_FUNC	"SysMemForKernel",0x135CB831,sceKernelAllocPartitionMemory
#endif
#ifdef F_SysMemForKernel_0008
	IMPORT_FUNC	"SysMemForKernel",0x1385A8F2,SysMemForKernel_C7E57B9C
#endif
#ifdef F_SysMemForKernel_0009
	IMPORT_FUNC	"SysMemForKernel",0x18D7B532,sceKernelMemoryCloseSize
#endif
#ifdef F_SysMemForKernel_0010
	IMPORT_FUNC	"SysMemForKernel",0x226C2C30,sceKernelQueryMemoryInfo
#endif
#ifdef F_SysMemForKernel_0011
	IMPORT_FUNC	"SysMemForKernel",0x2736E92C,SysMemForKernel_4EC43DC4
#endif
#ifdef F_SysMemForKernel_0012
	IMPORT_FUNC	"SysMemForKernel",0x2986ED83,sceKernelQueryMemoryBlockInfo
#endif
#ifdef F_SysMemForKernel_0013
	IMPORT_FUNC	"SysMemForKernel",0x2AFD39E8,SysMemForKernel_4852F8DD
#endif
#ifdef F_SysMemForKernel_0014
	IMPORT_FUNC	"SysMemForKernel",0x2C2F6493,sceKernelLookupUIDFunction
#endif
#ifdef F_SysMemForKernel_0015
	IMPORT_FUNC	"SysMemForKernel",0x30CA74FD,sceKernelIsHold
#endif
#ifdef F_SysMemForKernel_0016
	IMPORT_FUNC	"SysMemForKernel",0x36175DEF,sceKernelGetCompilerVersion
#endif
#ifdef F_SysMemForKernel_0017
	IMPORT_FUNC	"SysMemForKernel",0x3B2A4D7F,sceKernelAllocHeapMemoryWithOption
#endif
#ifdef F_SysMemForKernel_0018
	IMPORT_FUNC	"SysMemForKernel",0x3DBBB447,sceKernelHeapTotalFreeSize
#endif
#ifdef F_SysMemForKernel_0019
	IMPORT_FUNC	"SysMemForKernel",0x40C91389,sceKernelFreePartitionMemory
#endif
#ifdef F_SysMemForKernel_0020
	IMPORT_FUNC	"SysMemForKernel",0x40F41273,sceKernelPartitionTotalFreeMemSize
#endif
#ifdef F_SysMemForKernel_0021
	IMPORT_FUNC	"SysMemForKernel",0x468486F1,sceKernelCreateUIDtypeInherit
#endif
#ifdef F_SysMemForKernel_0022
	IMPORT_FUNC	"SysMemForKernel",0x49519904,SysMemForKernel_049CC735
#endif
#ifdef F_SysMemForKernel_0023
	IMPORT_FUNC	"SysMemForKernel",0x4F74E605,sceKernelCallUIDFunction
#endif
#ifdef F_SysMemForKernel_0024
	IMPORT_FUNC	"SysMemForKernel",0x51CE91FC,SysMemForKernel_1F01A9E2
#endif
#ifdef F_SysMemForKernel_0025
	IMPORT_FUNC	"SysMemForKernel",0x52A8B734,SysMemForKernel_EF2EE8C1
#endif
#ifdef F_SysMemForKernel_0026
	IMPORT_FUNC	"SysMemForKernel",0x539F8A51,SysMemForKernel_CDA3A2F7
#endif
#ifdef F_SysMemForKernel_0027
	IMPORT_FUNC	"SysMemForKernel",0x540364C7,sceKernelCreateUIDtype
#endif
#ifdef F_SysMemForKernel_0028
	IMPORT_FUNC	"SysMemForKernel",0x54247DD8,SysMemForKernel_536AD5E1
#endif
#ifdef F_SysMemForKernel_0029
	IMPORT_FUNC	"SysMemForKernel",0x5782A1A2,sceKernelRebootKernel
#endif
#ifdef F_SysMemForKernel_0030
	IMPORT_FUNC	"SysMemForKernel",0x618C235D,sceKernelGetSysMemoryInfo
#endif
#ifdef F_SysMemForKernel_0031
	IMPORT_FUNC	"SysMemForKernel",0x62813BDC,sceKernelMemmove
#endif
#ifdef F_SysMemForKernel_0032
	IMPORT_FUNC	"SysMemForKernel",0x6491D7FC,SysMemForKernel_82D3CEE3
#endif
#ifdef F_SysMemForKernel_0033
	IMPORT_FUNC	"SysMemForKernel",0x6880279D,sceKernelMemmoveWithFill
#endif
#ifdef F_SysMemForKernel_0034
	IMPORT_FUNC	"SysMemForKernel",0x6986B4E3,sceKernelAllocHeapMemory
#endif
#ifdef F_SysMemForKernel_0035
	IMPORT_FUNC	"SysMemForKernel",0x6AB69B9A,sceKernelSizeLockMemoryBlock
#endif
#ifdef F_SysMemForKernel_0036
	IMPORT_FUNC	"SysMemForKernel",0x7314BD1D,sceKernelMemoryExtendSize
#endif
#ifdef F_SysMemForKernel_0037
	IMPORT_FUNC	"SysMemForKernel",0x777FBE14,sceKernelCreateUID
#endif
#ifdef F_SysMemForKernel_0038
	IMPORT_FUNC	"SysMemForKernel",0x7EF4BDCB,sceKernelQueryHeapInfo
#endif
#ifdef F_SysMemForKernel_0039
	IMPORT_FUNC	"SysMemForKernel",0x88021868,sceKernelGetId
#endif
#ifdef F_SysMemForKernel_0040
	IMPORT_FUNC	"SysMemForKernel",0x881A84F7,sceKernelSysMemMaxFreeMemSize
#endif
#ifdef F_SysMemForKernel_0041
	IMPORT_FUNC	"SysMemForKernel",0x88F6A684,SysMemForKernel_03808C51
#endif
#ifdef F_SysMemForKernel_0042
	IMPORT_FUNC	"SysMemForKernel",0x89F405D2,sceKernelSetUmdData
#endif
#ifdef F_SysMemForKernel_0043
	IMPORT_FUNC	"SysMemForKernel",0x8A412A61,sceKernelRenameUID
#endif
#ifdef F_SysMemForKernel_0044
	IMPORT_FUNC	"SysMemForKernel",0x8B9178EF,SysMemForKernel_CBB05241
#endif
#ifdef F_SysMemForKernel_0045
	IMPORT_FUNC	"SysMemForKernel",0x8C0EEB1A,sceKernelFillFreeBlock
#endif
#ifdef F_SysMemForKernel_0046
	IMPORT_FUNC	"SysMemForKernel",0x8C2C0E71,sceKernelPartitionMaxFreeMemSize
#endif
#ifdef F_SysMemForKernel_0047
	IMPORT_FUNC	"SysMemForKernel",0x8C851B7A,sceKernelJointMemoryBlock
#endif
#ifdef F_SysMemForKernel_0048
	IMPORT_FUNC	"SysMemForKernel",0x8DBE8861,SysMemForKernel_9452B542
#endif
#ifdef F_SysMemForKernel_0049
	IMPORT_FUNC	"SysMemForKernel",0x9110439F,sceKernelGetCompiledSdkVersion
#endif
#ifdef F_SysMemForKernel_0050
	IMPORT_FUNC	"SysMemForKernel",0x91DBEA39,SysMemForKernel_BB90D841
#endif
#ifdef F_SysMemForKernel_0051
	IMPORT_FUNC	"SysMemForKernel",0x92E476D2,SysMemForKernel_CE05CCB7
#endif
#ifdef F_SysMemForKernel_0052
	IMPORT_FUNC	"SysMemForKernel",0x98A35BAE,SysMemForKernel_B9F8561C
#endif
#ifdef F_SysMemForKernel_0053
	IMPORT_FUNC	"SysMemForKernel",0x9AB85DA9,SysMemForKernel_CE8D3DB3
#endif
#ifdef F_SysMemForKernel_0054
	IMPORT_FUNC	"SysMemForKernel",0x9E4D87F7,sceKernelMemoryOpenSize
#endif
#ifdef F_SysMemForKernel_0055
	IMPORT_FUNC	"SysMemForKernel",0x9E7265D9,SysMemForKernel_38495D84
#endif
#ifdef F_SysMemForKernel_0056
	IMPORT_FUNC	"SysMemForKernel",0xA09B882C,sceKernelReleaseUID
#endif
#ifdef F_SysMemForKernel_0057
	IMPORT_FUNC	"SysMemForKernel",0xA267871C,SysMemForKernel_FC207849
#endif
#ifdef F_SysMemForKernel_0058
	IMPORT_FUNC	"SysMemForKernel",0xA27BEAD4,SysMemForKernel_960B888C
#endif
#ifdef F_SysMemForKernel_0059
	IMPORT_FUNC	"SysMemForKernel",0xA33A7B89,SysMemForKernel_D2E3A399
#endif
#ifdef F_SysMemForKernel_0060
	IMPORT_FUNC	"SysMemForKernel",0xA3B0B6BC,sceKernelGetModel
#endif
#ifdef F_SysMemForKernel_0061
	IMPORT_FUNC	"SysMemForKernel",0xA48F9782,sceKernelGetSystemStatus
#endif
#ifdef F_SysMemForKernel_0062
	IMPORT_FUNC	"SysMemForKernel",0xA530DAA9,SysMemForKernel_1F7F7F40
#endif
#ifdef F_SysMemForKernel_0063
	IMPORT_FUNC	"SysMemForKernel",0xA67BAD67,sceKernelSysMemTotalFreeMemSize
#endif
#ifdef F_SysMemForKernel_0064
	IMPORT_FUNC	"SysMemForKernel",0xAB1F7C42,sceKernelRegisterGetIdFunc
#endif
#ifdef F_SysMemForKernel_0065
	IMPORT_FUNC	"SysMemForKernel",0xAFDB003A,sceKernelCopyGameInfo
#endif
#ifdef F_SysMemForKernel_0066
	IMPORT_FUNC	"SysMemForKernel",0xB015F84F,SysMemForKernel_6D8E0CDF
#endif
#ifdef F_SysMemForKernel_0067
	IMPORT_FUNC	"SysMemForKernel",0xB1634112,sceKernelGetGameInfo
#endif
#ifdef F_SysMemForKernel_0068
	IMPORT_FUNC	"SysMemForKernel",0xB232294A,sceKernelMemset
#endif
#ifdef F_SysMemForKernel_0069
	IMPORT_FUNC	"SysMemForKernel",0xB6082F0D,sceKernelFreeHeapMemory
#endif
#ifdef F_SysMemForKernel_0070
	IMPORT_FUNC	"SysMemForKernel",0xB8637A4B,sceKernelSetParamSfo
#endif
#ifdef F_SysMemForKernel_0071
	IMPORT_FUNC	"SysMemForKernel",0xBA445A0A,sceKernelSysMemMemSize
#endif
#ifdef F_SysMemForKernel_0072
	IMPORT_FUNC	"SysMemForKernel",0xBBD786DD,sceKernelSetDdrMemoryProtection
#endif
#ifdef F_SysMemForKernel_0073
	IMPORT_FUNC	"SysMemForKernel",0xBC3AFFF2,sceKernelGetBlockHeadAddr
#endif
#ifdef F_SysMemForKernel_0074
	IMPORT_FUNC	"SysMemForKernel",0xC6500271,SysMemForKernel_02786087
#endif
#ifdef F_SysMemForKernel_0075
	IMPORT_FUNC	"SysMemForKernel",0xC80E5BE1,sceKernelDeleteUID
#endif
#ifdef F_SysMemForKernel_0076
	IMPORT_FUNC	"SysMemForKernel",0xCB168B8A,sceKernelSetCompilerVersion
#endif
#ifdef F_SysMemForKernel_0077
	IMPORT_FUNC	"SysMemForKernel",0xCDBFF97E,SysMemForKernel_55E4719C
#endif
#ifdef F_SysMemForKernel_0078
	IMPORT_FUNC	"SysMemForKernel",0xD145D1F5,sceKernelResizeMemoryBlock
#endif
#ifdef F_SysMemForKernel_0079
	IMPORT_FUNC	"SysMemForKernel",0xD26FFD63,sceKernelSetSystemStatus
#endif
#ifdef F_SysMemForKernel_0080
	IMPORT_FUNC	"SysMemForKernel",0xD2EC0FA8,sceKernelGetUIDtype
#endif
#ifdef F_SysMemForKernel_0081
	IMPORT_FUNC	"SysMemForKernel",0xD3846DAA,SysMemForKernel_071D9804
#endif
#ifdef F_SysMemForKernel_0082
	IMPORT_FUNC	"SysMemForKernel",0xD420C902,sceKernelCallUIDObjFunction
#endif
#ifdef F_SysMemForKernel_0083
	IMPORT_FUNC	"SysMemForKernel",0xDB9BE041,sceKernelGetDNAS
#endif
#ifdef F_SysMemForKernel_0084
	IMPORT_FUNC	"SysMemForKernel",0xDEA700CE,sceKernelMemoryShrinkSize
#endif
#ifdef F_SysMemForKernel_0085
	IMPORT_FUNC	"SysMemForKernel",0xE151223E,sceKernelGetUIDcontrolBlock
#endif
#ifdef F_SysMemForKernel_0086
	IMPORT_FUNC	"SysMemForKernel",0xE7E023FB,SysMemForKernel_2269BFA2
#endif
#ifdef F_SysMemForKernel_0087
	IMPORT_FUNC	"SysMemForKernel",0xE9C3D697,sceKernelDeleteUIDtype
#endif
#ifdef F_SysMemForKernel_0088
	IMPORT_FUNC	"SysMemForKernel",0xECFE305B,sceKernelQueryMemoryPartitionInfo
#endif
#ifdef F_SysMemForKernel_0089
	IMPORT_FUNC	"SysMemForKernel",0xEE010E3A,SysMemForKernel_A262FEF0
#endif
#ifdef F_SysMemForKernel_0090
	IMPORT_FUNC	"SysMemForKernel",0xEE1718BC,sceKernelDevkitVersion
#endif
#ifdef F_SysMemForKernel_0091
	IMPORT_FUNC	"SysMemForKernel",0xF01401E6,sceKernelGetUIDname
#endif
#ifdef F_SysMemForKernel_0092
	IMPORT_FUNC	"SysMemForKernel",0xF06981C3,SysMemForKernel_39351245
#endif
#ifdef F_SysMemForKernel_0093
	IMPORT_FUNC	"SysMemForKernel",0xF2389185,SysMemForKernel_E0645895
#endif
#ifdef F_SysMemForKernel_0094
	IMPORT_FUNC	"SysMemForKernel",0xF75BE1B2,sceKernelMemset32
#endif
#ifdef F_SysMemForKernel_0095
	IMPORT_FUNC	"SysMemForKernel",0xFD2A7114,sceKernelCreateHeap
#endif
#ifdef F_SysMemForKernel_0096
	IMPORT_FUNC	"SysMemForKernel",0xFD347C2B,sceKernelSetRebootKernel
#endif
