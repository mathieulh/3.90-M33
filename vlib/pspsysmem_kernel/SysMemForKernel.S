	.set noreorder

#include "pspimport.s"


#ifdef F_SysMemForKernel_0000
	IMPORT_START	"SysMemForKernel",0x00010011
#endif
#ifdef F_SysMemForKernel_0001
	IMPORT_FUNC	"SysMemForKernel",0x002BA296,SysMemForKernel_002BA296
#endif
#ifdef F_SysMemForKernel_0002
	IMPORT_FUNC	"SysMemForKernel",0x02786087,SysMemForKernel_02786087
#endif
#ifdef F_SysMemForKernel_0003
	IMPORT_FUNC	"SysMemForKernel",0x03072750,sceKernelSysMemMaxFreeMemSize
#endif
#ifdef F_SysMemForKernel_0004
	IMPORT_FUNC	"SysMemForKernel",0x03808C51,SysMemForKernel_03808C51
#endif
#ifdef F_SysMemForKernel_0005
	IMPORT_FUNC	"SysMemForKernel",0x049CC735,SysMemForKernel_049CC735
#endif
#ifdef F_SysMemForKernel_0006
	IMPORT_FUNC	"SysMemForKernel",0x071D9804,SysMemForKernel_071D9804
#endif
#ifdef F_SysMemForKernel_0007
	IMPORT_FUNC	"SysMemForKernel",0x0D1D8AC8,sceKernelMemmoveWithFill
#endif
#ifdef F_SysMemForKernel_0008
	IMPORT_FUNC	"SysMemForKernel",0x101C3E59,sceKernelMemoryCloseSize
#endif
#ifdef F_SysMemForKernel_0009
	IMPORT_FUNC	"SysMemForKernel",0x1890BE9C,sceKernelSysMemMemSize
#endif
#ifdef F_SysMemForKernel_0010
	IMPORT_FUNC	"SysMemForKernel",0x1C1FBFE7,sceKernelCreateHeap
#endif
#ifdef F_SysMemForKernel_0011
	IMPORT_FUNC	"SysMemForKernel",0x1C221A08,sceKernelGetUIDname
#endif
#ifdef F_SysMemForKernel_0012
	IMPORT_FUNC	"SysMemForKernel",0x1C4B1713,sceKernelMemmove
#endif
#ifdef F_SysMemForKernel_0013
	IMPORT_FUNC	"SysMemForKernel",0x1F01A9E2,SysMemForKernel_1F01A9E2
#endif
#ifdef F_SysMemForKernel_0014
	IMPORT_FUNC	"SysMemForKernel",0x1F7F7F40,SysMemForKernel_1F7F7F40
#endif
#ifdef F_SysMemForKernel_0015
	IMPORT_FUNC	"SysMemForKernel",0x2269BFA2,SysMemForKernel_2269BFA2
#endif
#ifdef F_SysMemForKernel_0016
	IMPORT_FUNC	"SysMemForKernel",0x237DBD4F,sceKernelAllocPartitionMemory
#endif
#ifdef F_SysMemForKernel_0017
	IMPORT_FUNC	"SysMemForKernel",0x29A5899B,sceKernelSetRebootKernel
#endif
#ifdef F_SysMemForKernel_0018
	IMPORT_FUNC	"SysMemForKernel",0x2A3E5280,sceKernelQueryMemoryInfo
#endif
#ifdef F_SysMemForKernel_0019
	IMPORT_FUNC	"SysMemForKernel",0x2E3402CC,sceKernelRenameUID
#endif
#ifdef F_SysMemForKernel_0020
	IMPORT_FUNC	"SysMemForKernel",0x2F808748,sceKernelMemset32
#endif
#ifdef F_SysMemForKernel_0021
	IMPORT_FUNC	"SysMemForKernel",0x38495D84,SysMemForKernel_38495D84
#endif
#ifdef F_SysMemForKernel_0022
	IMPORT_FUNC	"SysMemForKernel",0x39351245,SysMemForKernel_39351245
#endif
#ifdef F_SysMemForKernel_0023
	IMPORT_FUNC	"SysMemForKernel",0x39357F07,sceKernelGetUIDtype
#endif
#ifdef F_SysMemForKernel_0024
	IMPORT_FUNC	"SysMemForKernel",0x3FC9AE6A,sceKernelDevkitVersion
#endif
#ifdef F_SysMemForKernel_0025
	IMPORT_FUNC	"SysMemForKernel",0x419DB8F4,sceKernelRegisterGetIdFunc
#endif
#ifdef F_SysMemForKernel_0026
	IMPORT_FUNC	"SysMemForKernel",0x41FFC7F9,sceKernelGetUIDcontrolBlockWithType
#endif
#ifdef F_SysMemForKernel_0027
	IMPORT_FUNC	"SysMemForKernel",0x452E3696,sceKernelGetSystemStatus
#endif
#ifdef F_SysMemForKernel_0028
	IMPORT_FUNC	"SysMemForKernel",0x4852F8DD,SysMemForKernel_4852F8DD
#endif
#ifdef F_SysMemForKernel_0029
	IMPORT_FUNC	"SysMemForKernel",0x4EC43DC4,SysMemForKernel_4EC43DC4
#endif
#ifdef F_SysMemForKernel_0030
	IMPORT_FUNC	"SysMemForKernel",0x5367923C,sceKernelCallUIDObjFunction
#endif
#ifdef F_SysMemForKernel_0031
	IMPORT_FUNC	"SysMemForKernel",0x536AD5E1,SysMemForKernel_536AD5E1
#endif
#ifdef F_SysMemForKernel_0032
	IMPORT_FUNC	"SysMemForKernel",0x55A40B2C,sceKernelQueryMemoryPartitionInfo
#endif
#ifdef F_SysMemForKernel_0033
	IMPORT_FUNC	"SysMemForKernel",0x55BFD686,sceKernelSearchUIDbyName
#endif
#ifdef F_SysMemForKernel_0034
	IMPORT_FUNC	"SysMemForKernel",0x55E4719C,SysMemForKernel_55E4719C
#endif
#ifdef F_SysMemForKernel_0035
	IMPORT_FUNC	"SysMemForKernel",0x5EBE73DE,sceKernelJointMemoryBlock
#endif
#ifdef F_SysMemForKernel_0036
	IMPORT_FUNC	"SysMemForKernel",0x636C953B,sceKernelAllocHeapMemory
#endif
#ifdef F_SysMemForKernel_0037
	IMPORT_FUNC	"SysMemForKernel",0x6373995D,sceKernelGetModel
#endif
#ifdef F_SysMemForKernel_0038
	IMPORT_FUNC	"SysMemForKernel",0x6C1DCD41,sceKernelCallUIDFunction
#endif
#ifdef F_SysMemForKernel_0039
	IMPORT_FUNC	"SysMemForKernel",0x6CD838EC,sceKernelLookupUIDFunction
#endif
#ifdef F_SysMemForKernel_0040
	IMPORT_FUNC	"SysMemForKernel",0x6D8E0CDF,SysMemForKernel_6D8E0CDF
#endif
#ifdef F_SysMemForKernel_0041
	IMPORT_FUNC	"SysMemForKernel",0x719792F1,sceKernelMemoryOpenSize
#endif
#ifdef F_SysMemForKernel_0042
	IMPORT_FUNC	"SysMemForKernel",0x7591C7DB,sceKernelSetCompiledSdkVersion
#endif
#ifdef F_SysMemForKernel_0043
	IMPORT_FUNC	"SysMemForKernel",0x7B3E7441,sceKernelMemoryExtendSize
#endif
#ifdef F_SysMemForKernel_0044
	IMPORT_FUNC	"SysMemForKernel",0x7B749390,sceKernelFreeHeapMemory
#endif
#ifdef F_SysMemForKernel_0045
	IMPORT_FUNC	"SysMemForKernel",0x7BE95FA0,sceKernelHoldUID
#endif
#ifdef F_SysMemForKernel_0046
	IMPORT_FUNC	"SysMemForKernel",0x7ECBDBD9,sceKernelGetDNAS
#endif
#ifdef F_SysMemForKernel_0047
	IMPORT_FUNC	"SysMemForKernel",0x811BED79,sceKernelSysMemTotalFreeMemSize
#endif
#ifdef F_SysMemForKernel_0048
	IMPORT_FUNC	"SysMemForKernel",0x82CCA14F,sceKernelQueryMemoryBlockInfo
#endif
#ifdef F_SysMemForKernel_0049
	IMPORT_FUNC	"SysMemForKernel",0x82D3CEE3,SysMemForKernel_82D3CEE3
#endif
#ifdef F_SysMemForKernel_0050
	IMPORT_FUNC	"SysMemForKernel",0x89A74008,sceKernelCreateUID
#endif
#ifdef F_SysMemForKernel_0051
	IMPORT_FUNC	"SysMemForKernel",0x8C0CC205,sceKernelGetFilenameBlock
#endif
#ifdef F_SysMemForKernel_0052
	IMPORT_FUNC	"SysMemForKernel",0x8F20C4C0,sceKernelDeleteUID
#endif
#ifdef F_SysMemForKernel_0053
	IMPORT_FUNC	"SysMemForKernel",0x915EF4AC,SysMemForKernel_915EF4AC
#endif
#ifdef F_SysMemForKernel_0054
	IMPORT_FUNC	"SysMemForKernel",0x9452B542,SysMemForKernel_9452B542
#endif
#ifdef F_SysMemForKernel_0055
	IMPORT_FUNC	"SysMemForKernel",0x95F5E8DA,sceKernelSetSystemStatus
#endif
#ifdef F_SysMemForKernel_0056
	IMPORT_FUNC	"SysMemForKernel",0x960B888C,SysMemForKernel_960B888C
#endif
#ifdef F_SysMemForKernel_0057
	IMPORT_FUNC	"SysMemForKernel",0x9697CD32,sceKernelPartitionTotalFreeMemSize
#endif
#ifdef F_SysMemForKernel_0058
	IMPORT_FUNC	"SysMemForKernel",0x97B18FA8,sceKernelCopyGameInfo
#endif
#ifdef F_SysMemForKernel_0059
	IMPORT_FUNC	"SysMemForKernel",0x9C304ED7,sceKernelSetDNAS
#endif
#ifdef F_SysMemForKernel_0060
	IMPORT_FUNC	"SysMemForKernel",0x9D9A5BA1,sceKernelGetBlockHeadAddr
#endif
#ifdef F_SysMemForKernel_0061
	IMPORT_FUNC	"SysMemForKernel",0xA089ECA4,sceKernelMemset
#endif
#ifdef F_SysMemForKernel_0062
	IMPORT_FUNC	"SysMemForKernel",0xA1ACEA31,sceKernelGetId
#endif
#ifdef F_SysMemForKernel_0063
	IMPORT_FUNC	"SysMemForKernel",0xA262FEF0,SysMemForKernel_A262FEF0
#endif
#ifdef F_SysMemForKernel_0064
	IMPORT_FUNC	"SysMemForKernel",0xA2A65F0E,sceKernelFillFreeBlock
#endif
#ifdef F_SysMemForKernel_0065
	IMPORT_FUNC	"SysMemForKernel",0xA823047E,sceKernelHeapTotalFreeSize
#endif
#ifdef F_SysMemForKernel_0066
	IMPORT_FUNC	"SysMemForKernel",0xAD09C397,sceKernelCreateUIDtypeInherit
#endif
#ifdef F_SysMemForKernel_0067
	IMPORT_FUNC	"SysMemForKernel",0xB2C7AA36,sceKernelSetDdrMemoryProtection
#endif
#ifdef F_SysMemForKernel_0068
	IMPORT_FUNC	"SysMemForKernel",0xB6D61D02,sceKernelFreePartitionMemory
#endif
#ifdef F_SysMemForKernel_0069
	IMPORT_FUNC	"SysMemForKernel",0xB9F8561C,SysMemForKernel_B9F8561C
#endif
#ifdef F_SysMemForKernel_0070
	IMPORT_FUNC	"SysMemForKernel",0xBB90D841,SysMemForKernel_BB90D841
#endif
#ifdef F_SysMemForKernel_0071
	IMPORT_FUNC	"SysMemForKernel",0xC7E57B9C,SysMemForKernel_C7E57B9C
#endif
#ifdef F_SysMemForKernel_0072
	IMPORT_FUNC	"SysMemForKernel",0xC9805775,sceKernelDeleteHeap
#endif
#ifdef F_SysMemForKernel_0073
	IMPORT_FUNC	"SysMemForKernel",0xCBB05241,SysMemForKernel_CBB05241
#endif
#ifdef F_SysMemForKernel_0074
	IMPORT_FUNC	"SysMemForKernel",0xCD617A94,sceKernelGetGameInfo
#endif
#ifdef F_SysMemForKernel_0075
	IMPORT_FUNC	"SysMemForKernel",0xCDA3A2F7,SysMemForKernel_CDA3A2F7
#endif
#ifdef F_SysMemForKernel_0076
	IMPORT_FUNC	"SysMemForKernel",0xCE05CCB7,SysMemForKernel_CE05CCB7
#endif
#ifdef F_SysMemForKernel_0077
	IMPORT_FUNC	"SysMemForKernel",0xCE5544F4,sceKernelResizeMemoryBlock
#endif
#ifdef F_SysMemForKernel_0078
	IMPORT_FUNC	"SysMemForKernel",0xCE8D3DB3,SysMemForKernel_CE8D3DB3
#endif
#ifdef F_SysMemForKernel_0079
	IMPORT_FUNC	"SysMemForKernel",0xCF4DE78C,sceKernelGetUIDcontrolBlock
#endif
#ifdef F_SysMemForKernel_0080
	IMPORT_FUNC	"SysMemForKernel",0xD1BAB054,sceKernelDeleteUIDtype
#endif
#ifdef F_SysMemForKernel_0081
	IMPORT_FUNC	"SysMemForKernel",0xD2E3A399,SysMemForKernel_D2E3A399
#endif
#ifdef F_SysMemForKernel_0082
	IMPORT_FUNC	"SysMemForKernel",0xDEC0A0D8,sceKernelGetCompilerVersion
#endif
#ifdef F_SysMemForKernel_0083
	IMPORT_FUNC	"SysMemForKernel",0xDFAEBD5B,sceKernelIsHold
#endif
#ifdef F_SysMemForKernel_0084
	IMPORT_FUNC	"SysMemForKernel",0xE0058030,sceKernelMemoryShrinkSize
#endif
#ifdef F_SysMemForKernel_0085
	IMPORT_FUNC	"SysMemForKernel",0xE0645895,SysMemForKernel_E0645895
#endif
#ifdef F_SysMemForKernel_0086
	IMPORT_FUNC	"SysMemForKernel",0xE40D3A1D,sceKernelSetUmdData
#endif
#ifdef F_SysMemForKernel_0087
	IMPORT_FUNC	"SysMemForKernel",0xE6581468,sceKernelPartitionMaxFreeMemSize
#endif
#ifdef F_SysMemForKernel_0088
	IMPORT_FUNC	"SysMemForKernel",0xE9E4D851,sceKernelSetFilenameBlock
#endif
#ifdef F_SysMemForKernel_0089
	IMPORT_FUNC	"SysMemForKernel",0xEB4C0E1B,sceKernelQueryBlockSize
#endif
#ifdef F_SysMemForKernel_0090
	IMPORT_FUNC	"SysMemForKernel",0xEB7A74DB,sceKernelAllocHeapMemoryWithOption
#endif
#ifdef F_SysMemForKernel_0091
	IMPORT_FUNC	"SysMemForKernel",0xEE867074,sceKernelSizeLockMemoryBlock
#endif
#ifdef F_SysMemForKernel_0092
	IMPORT_FUNC	"SysMemForKernel",0xEF2EE8C1,SysMemForKernel_EF2EE8C1
#endif
#ifdef F_SysMemForKernel_0093
	IMPORT_FUNC	"SysMemForKernel",0xF4390489,sceKernelRebootKernel
#endif
#ifdef F_SysMemForKernel_0094
	IMPORT_FUNC	"SysMemForKernel",0xF6C10E27,sceKernelGetSysMemoryInfo
#endif
#ifdef F_SysMemForKernel_0095
	IMPORT_FUNC	"SysMemForKernel",0xF77D77CB,sceKernelSetCompilerVersion
#endif
#ifdef F_SysMemForKernel_0096
	IMPORT_FUNC	"SysMemForKernel",0xF91FE6AA,SysMemForKernel_F91FE6AA
#endif
#ifdef F_SysMemForKernel_0097
	IMPORT_FUNC	"SysMemForKernel",0xFC114573,sceKernelGetCompiledSdkVersion
#endif
#ifdef F_SysMemForKernel_0098
	IMPORT_FUNC	"SysMemForKernel",0xFC207849,SysMemForKernel_FC207849
#endif
#ifdef F_SysMemForKernel_0099
	IMPORT_FUNC	"SysMemForKernel",0xFE8DEBE0,sceKernelReleaseUID
#endif
#ifdef F_SysMemForKernel_0100
	IMPORT_FUNC	"SysMemForKernel",0xFEFC8666,sceKernelCreateUIDtype
#endif
