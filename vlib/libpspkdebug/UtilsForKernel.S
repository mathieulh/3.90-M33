	.set noreorder

#include "pspimport.s"

// Build List
// UtilsForKernel_0000.o UtilsForKernel_0001.o UtilsForKernel_0002.o UtilsForKernel_0003.o UtilsForKernel_0004.o UtilsForKernel_0005.o UtilsForKernel_0006.o UtilsForKernel_0007.o UtilsForKernel_0008.o UtilsForKernel_0009.o UtilsForKernel_0010.o UtilsForKernel_0011.o UtilsForKernel_0012.o UtilsForKernel_0013.o UtilsForKernel_0014.o UtilsForKernel_0015.o UtilsForKernel_0016.o UtilsForKernel_0017.o UtilsForKernel_0018.o UtilsForKernel_0019.o UtilsForKernel_0020.o UtilsForKernel_0021.o UtilsForKernel_0022.o UtilsForKernel_0023.o UtilsForKernel_0024.o UtilsForKernel_0025.o UtilsForKernel_0026.o UtilsForKernel_0027.o UtilsForKernel_0028.o UtilsForKernel_0029.o UtilsForKernel_0030.o UtilsForKernel_0031.o UtilsForKernel_0032.o UtilsForKernel_0033.o UtilsForKernel_0034.o UtilsForKernel_0035.o UtilsForKernel_0036.o UtilsForKernel_0037.o UtilsForKernel_0038.o UtilsForKernel_0039.o UtilsForKernel_0040.o UtilsForKernel_0041.o UtilsForKernel_0042.o UtilsForKernel_0043.o UtilsForKernel_0044.o UtilsForKernel_0045.o UtilsForKernel_0046.o UtilsForKernel_0047.o UtilsForKernel_0048.o 

#ifdef F_UtilsForKernel_0000
	IMPORT_START	"UtilsForKernel",0x00090011
#endif
#ifdef F_UtilsForKernel_0001
	IMPORT_FUNC	"UtilsForKernel",0x06FB8A63,sceKernelUtilsMt19937UInt
#endif
#ifdef F_UtilsForKernel_0002
	IMPORT_FUNC	"UtilsForKernel",0x136F2419,UtilsForKernel_136F2419
#endif
#ifdef F_UtilsForKernel_0003
	IMPORT_FUNC	"UtilsForKernel",0x16641D70,sceKernelDcacheReadTag
#endif
#ifdef F_UtilsForKernel_0004
	IMPORT_FUNC	"UtilsForKernel",0x193D4036,sceKernelSetGPIMask
#endif
#ifdef F_UtilsForKernel_0005
	IMPORT_FUNC	"UtilsForKernel",0x23A0C5BA,UtilsForKernel_23A0C5BA
#endif
#ifdef F_UtilsForKernel_0006
	IMPORT_FUNC	"UtilsForKernel",0x23FFC828,sceKernelGzipGetCompressedData
#endif
#ifdef F_UtilsForKernel_0007
	IMPORT_FUNC	"UtilsForKernel",0x27CC57F0,sceKernelLibcTime
#endif
#ifdef F_UtilsForKernel_0008
	IMPORT_FUNC	"UtilsForKernel",0x346F6DA8,sceKernelUtilsSha1BlockUpdate
#endif
#ifdef F_UtilsForKernel_0009
	IMPORT_FUNC	"UtilsForKernel",0x34B9FA9E,sceKernelDcacheWritebackInvalidateRange
#endif
#ifdef F_UtilsForKernel_0010
	IMPORT_FUNC	"UtilsForKernel",0x37FB5C42,sceKernelGetGPI
#endif
#ifdef F_UtilsForKernel_0011
	IMPORT_FUNC	"UtilsForKernel",0x39F49610,UtilsForKernel_39F49610
#endif
#ifdef F_UtilsForKernel_0012
	IMPORT_FUNC	"UtilsForKernel",0x3EE30821,sceKernelDcacheWritebackRange
#endif
#ifdef F_UtilsForKernel_0013
	IMPORT_FUNC	"UtilsForKernel",0x41887EF4,UtilsForKernel_41887EF4
#endif
#ifdef F_UtilsForKernel_0014
	IMPORT_FUNC	"UtilsForKernel",0x4FD31C9D,sceKernelIcacheProbe
#endif
#ifdef F_UtilsForKernel_0015
	IMPORT_FUNC	"UtilsForKernel",0x585F1C09,sceKernelUtilsSha1BlockResult
#endif
#ifdef F_UtilsForKernel_0016
	IMPORT_FUNC	"UtilsForKernel",0x61E1E525,sceKernelUtilsMd5BlockUpdate
#endif
#ifdef F_UtilsForKernel_0017
	IMPORT_FUNC	"UtilsForKernel",0x6231A71D,UtilsForKernel_6231A71D
#endif
#ifdef F_UtilsForKernel_0018
	IMPORT_FUNC	"UtilsForKernel",0x6AD345D7,sceKernelSetGPO
#endif
#ifdef F_UtilsForKernel_0019
	IMPORT_FUNC	"UtilsForKernel",0x71EC4271,sceKernelLibcGettimeofday
#endif
#ifdef F_UtilsForKernel_0020
	IMPORT_FUNC	"UtilsForKernel",0x77DFF087,sceKernelDcacheProbeRange
#endif
#ifdef F_UtilsForKernel_0021
	IMPORT_FUNC	"UtilsForKernel",0x78934841,sceKernelGzipDecompress
#endif
#ifdef F_UtilsForKernel_0022
	IMPORT_FUNC	"UtilsForKernel",0x79D1C3FA,sceKernelDcacheWritebackAll
#endif
#ifdef F_UtilsForKernel_0023
	IMPORT_FUNC	"UtilsForKernel",0x7DD07271,sceKernelLzrcDecode
#endif
#ifdef F_UtilsForKernel_0024
	IMPORT_FUNC	"UtilsForKernel",0x80001C4C,sceKernelDcacheProbe
#endif
#ifdef F_UtilsForKernel_0025
	IMPORT_FUNC	"UtilsForKernel",0x840259F1,sceKernelUtilsSha1Digest
#endif
#ifdef F_UtilsForKernel_0026
	IMPORT_FUNC	"UtilsForKernel",0x864A9D72,sceKernelDcacheInvalidateAll
#endif
#ifdef F_UtilsForKernel_0027
	IMPORT_FUNC	"UtilsForKernel",0x87E81561,UtilsForKernel_87E81561
#endif
#ifdef F_UtilsForKernel_0028
	IMPORT_FUNC	"UtilsForKernel",0x8C1FBE04,sceKernelGzipGetComment
#endif
#ifdef F_UtilsForKernel_0029
	IMPORT_FUNC	"UtilsForKernel",0x91E4F6A7,sceKernelLibcClock
#endif
#ifdef F_UtilsForKernel_0030
	IMPORT_FUNC	"UtilsForKernel",0x920F104A,sceKernelIcacheInvalidateAll
#endif
#ifdef F_UtilsForKernel_0031
	IMPORT_FUNC	"UtilsForKernel",0x92282A47,UtilsForKernel_92282A47
#endif
#ifdef F_UtilsForKernel_0032
	IMPORT_FUNC	"UtilsForKernel",0x95035FEF,sceKernelSetGPOMask
#endif
#ifdef F_UtilsForKernel_0033
	IMPORT_FUNC	"UtilsForKernel",0x9E5C5086,sceKernelUtilsMd5BlockInit
#endif
#ifdef F_UtilsForKernel_0034
	IMPORT_FUNC	"UtilsForKernel",0xAF3616C0,UtilsForKernel_AF3616C0
#endif
#ifdef F_UtilsForKernel_0035
	IMPORT_FUNC	"UtilsForKernel",0xB0E9C31F,sceKernelGzipGetInfo
#endif
#ifdef F_UtilsForKernel_0036
	IMPORT_FUNC	"UtilsForKernel",0xB435DEC5,sceKernelDcacheWritebackInvalidateAll
#endif
#ifdef F_UtilsForKernel_0037
	IMPORT_FUNC	"UtilsForKernel",0xB83A1E76,UtilsForKernel_B83A1E76
#endif
#ifdef F_UtilsForKernel_0038
	IMPORT_FUNC	"UtilsForKernel",0xB8D24E78,sceKernelUtilsMd5BlockResult
#endif
#ifdef F_UtilsForKernel_0039
	IMPORT_FUNC	"UtilsForKernel",0xBDBFCA89,UtilsForKernel_BDBFCA89
#endif
#ifdef F_UtilsForKernel_0040
	IMPORT_FUNC	"UtilsForKernel",0xBFA98062,sceKernelDcacheInvalidateRange
#endif
#ifdef F_UtilsForKernel_0041
	IMPORT_FUNC	"UtilsForKernel",0xC2DF770E,sceKernelIcacheInvalidateRange
#endif
#ifdef F_UtilsForKernel_0042
	IMPORT_FUNC	"UtilsForKernel",0xC8186A58,sceKernelUtilsMd5Digest
#endif
#ifdef F_UtilsForKernel_0043
	IMPORT_FUNC	"UtilsForKernel",0xE0CE3E29,sceKernelGzipIsValid
#endif
#ifdef F_UtilsForKernel_0044
	IMPORT_FUNC	"UtilsForKernel",0xE0E6BA96,sceKernelGzipGetName
#endif
#ifdef F_UtilsForKernel_0045
	IMPORT_FUNC	"UtilsForKernel",0xE860E75E,sceKernelUtilsMt19937Init
#endif
#ifdef F_UtilsForKernel_0046
	IMPORT_FUNC	"UtilsForKernel",0xE8DB3CE6,sceKernelDeflateDecompress
#endif
#ifdef F_UtilsForKernel_0047
	IMPORT_FUNC	"UtilsForKernel",0xF8FCD5BA,sceKernelUtilsSha1BlockInit
#endif
#ifdef F_UtilsForKernel_0048
	IMPORT_FUNC	"UtilsForKernel",0xFB05FAD0,sceKernelIcacheReadTag
#endif
