	.set noreorder

#include "pspimport.s"

// Build files
// SystemCtrlForKernel_0000.o SystemCtrlForKernel_0001.o SystemCtrlForKernel_0002.o SystemCtrlForKernel_0003.o SystemCtrlForKernel_0004.o SystemCtrlForKernel_0005.o SystemCtrlForKernel_0006.o SystemCtrlForKernel_0007.o SystemCtrlForKernel_0008.o SystemCtrlForKernel_0009.o SystemCtrlForKernel_0010.o SystemCtrlForKernel_0011.o SystemCtrlForKernel_0012.o SystemCtrlForKernel_0013.o SystemCtrlForKernel_0014.o SystemCtrlForKernel_0015.o SystemCtrlForKernel_0016.o SystemCtrlForKernel_0017.o SystemCtrlForKernel_0018.o SystemCtrlForKernel_0019.o SystemCtrlForKernel_0020.o SystemCtrlForKernel_0021.o SystemCtrlForKernel_0022.o SystemCtrlForKernel_0023.o SystemCtrlForKernel_0024.o SystemCtrlForKernel_0025.o SystemCtrlForKernel_0026.o SystemCtrlForKernel_0027.o SystemCtrlForKernel_0028.o SystemCtrlForKernel_0029.o SystemCtrlForKernel_0030.o SystemCtrlForKernel_0031.o SystemCtrlForKernel_0032.o SystemCtrlForKernel_0033.o SystemCtrlForKernel_0034.o SystemCtrlForKernel_0035.o SystemCtrlForKernel_0036.o SystemCtrlForKernel_0037.o 

#ifdef F_SystemCtrlForKernel_0000
	IMPORT_START "SystemCtrlForKernel",0x00090000
#endif
#ifdef F_SystemCtrlForKernel_0001
	IMPORT_FUNC  "SystemCtrlForKernel",0x8D5BE1F0,sctrlKernelSetInitApitype
#endif
#ifdef F_SystemCtrlForKernel_0002
	IMPORT_FUNC  "SystemCtrlForKernel",0x128112C3,sctrlKernelSetInitFileName
#endif
#ifdef F_SystemCtrlForKernel_0003
	IMPORT_FUNC  "SystemCtrlForKernel",0xCB76B778,sctrlKernelSetInitKeyConfig
#endif
#ifdef F_SystemCtrlForKernel_0004
	IMPORT_FUNC  "SystemCtrlForKernel",0xEB74FE45,sctrlKernelSetUserLevel
#endif
#ifdef F_SystemCtrlForKernel_0005
	IMPORT_FUNC  "SystemCtrlForKernel",0xD8FF9B99,sctrlKernelSetDevkitVersion
#endif
#ifdef F_SystemCtrlForKernel_0006
	IMPORT_FUNC  "SystemCtrlForKernel",0xD339E2E9,sctrlHENIsSE
#endif
#ifdef F_SystemCtrlForKernel_0007
	IMPORT_FUNC  "SystemCtrlForKernel",0x2E2935EF,sctrlHENIsDevhook
#endif
#ifdef F_SystemCtrlForKernel_0008
	IMPORT_FUNC  "SystemCtrlForKernel",0x1090A2E1,sctrlHENGetVersion
#endif
#ifdef F_SystemCtrlForKernel_0009
	IMPORT_FUNC  "SystemCtrlForKernel",0x78E46415,sctrlHENFindDriver
#endif
#ifdef F_SystemCtrlForKernel_0010
	IMPORT_FUNC  "SystemCtrlForKernel",0x159AF5CC,sctrlHENFindFunction
#endif
#ifdef F_SystemCtrlForKernel_0011
	IMPORT_FUNC  "SystemCtrlForKernel",0xB47C9D77,sctrlSEGetVersion
#endif
#ifdef F_SystemCtrlForKernel_0012
	IMPORT_FUNC  "SystemCtrlForKernel",0x2794CCF4,sctrlKernelExitVSH
#endif
#ifdef F_SystemCtrlForKernel_0013
	IMPORT_FUNC  "SystemCtrlForKernel",0x577AF198,sctrlKernelLoadExecVSHDisc
#endif
#ifdef F_SystemCtrlForKernel_0014
	IMPORT_FUNC  "SystemCtrlForKernel",0x94FE5E4B,sctrlKernelLoadExecVSHDiscUpdater
#endif
#ifdef F_SystemCtrlForKernel_0015
	IMPORT_FUNC  "SystemCtrlForKernel",0x75643FCA,sctrlKernelLoadExecVSHMs1
#endif
#ifdef F_SystemCtrlForKernel_0016
	IMPORT_FUNC  "SystemCtrlForKernel",0xABA7F1B0,sctrlKernelLoadExecVSHMs2
#endif
#ifdef F_SystemCtrlForKernel_0017
	IMPORT_FUNC  "SystemCtrlForKernel",0x7B369596,sctrlKernelLoadExecVSHMs3
#endif
#ifdef F_SystemCtrlForKernel_0018
	IMPORT_FUNC  "SystemCtrlForKernel",0xD690750F,sctrlKernelLoadExecVSHMs4
#endif
#ifdef F_SystemCtrlForKernel_0019
	IMPORT_FUNC  "SystemCtrlForKernel",0x2D10FB28,sctrlKernelLoadExecVSHWithApitype
#endif
#ifdef F_SystemCtrlForKernel_0020
	IMPORT_FUNC  "SystemCtrlForKernel",0x1DDDAD0C,sctrlSESetConfig
#endif
#ifdef F_SystemCtrlForKernel_0021
	IMPORT_FUNC  "SystemCtrlForKernel",0x16C3B7EE,sctrlSEGetConfig
#endif
#ifdef F_SystemCtrlForKernel_0022
	IMPORT_FUNC  "SystemCtrlForKernel",0x85B520C6,sctrlSEMountUmdFromFile
#endif
#ifdef F_SystemCtrlForKernel_0023
	IMPORT_FUNC  "SystemCtrlForKernel",0x4EB249E4,sctrlHENSetOnApplyPspRelSectionEvent
#endif
#ifdef F_SystemCtrlForKernel_0024
	IMPORT_FUNC  "SystemCtrlForKernel",0xF9584CAD,oe_malloc
#endif
#ifdef F_SystemCtrlForKernel_0025
	IMPORT_FUNC  "SystemCtrlForKernel",0xA65E8BC4,oe_free
#endif
#ifdef F_SystemCtrlForKernel_0026
	IMPORT_FUNC  "SystemCtrlForKernel",0xAC56B90B,GetUmdFile
#endif
#ifdef F_SystemCtrlForKernel_0027
	IMPORT_FUNC  "SystemCtrlForKernel",0xB64186D0,SetUmdFile
#endif
#ifdef F_SystemCtrlForKernel_0028
	IMPORT_FUNC  "SystemCtrlForKernel",0x260CA420,isofs_init
#endif
#ifdef F_SystemCtrlForKernel_0029
	IMPORT_FUNC  "SystemCtrlForKernel",0xB078D9A0,isofs_exit
#endif
#ifdef F_SystemCtrlForKernel_0030
	IMPORT_FUNC  "SystemCtrlForKernel",0xE82E932B,isofs_open
#endif
#ifdef F_SystemCtrlForKernel_0031
	IMPORT_FUNC  "SystemCtrlForKernel",0x324FB7B1,isofs_read
#endif
#ifdef F_SystemCtrlForKernel_0032
	IMPORT_FUNC  "SystemCtrlForKernel",0xCAE6A8E1,isofs_close
#endif
#ifdef F_SystemCtrlForKernel_0033
	IMPORT_FUNC  "SystemCtrlForKernel",0x3BC8E648,isofs_lseek
#endif
#ifdef F_SystemCtrlForKernel_0034
	IMPORT_FUNC  "SystemCtrlForKernel",0xDC974FF8,isofs_getstat
#endif
#ifdef F_SystemCtrlForKernel_0035
	IMPORT_FUNC  "SystemCtrlForKernel",0x7E6F2BBA,isofs_fastinit
#endif
#ifdef F_SystemCtrlForKernel_0036
	IMPORT_FUNC  "SystemCtrlForKernel",0x02BFCB5F,PatchSyscall
#endif
#ifdef F_SystemCtrlForKernel_0037
	IMPORT_FUNC  "SystemCtrlForKernel",0x98012538,SetSpeed
#endif
