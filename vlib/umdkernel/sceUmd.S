	.set noreorder

#include "pspimport.s"

// Build List
// sceUmd_0000.o sceUmd_0001.o sceUmd_0002.o sceUmd_0003.o sceUmd_0004.o sceUmd_0005.o sceUmd_0006.o sceUmd_0007.o sceUmd_0008.o sceUmd_0009.o sceUmd_0010.o sceUmd_0011.o sceUmd_0012.o sceUmd_0013.o sceUmd_0014.o sceUmd_0015.o sceUmd_0016.o sceUmd_0017.o sceUmd_0018.o sceUmd_0019.o sceUmd_0020.o sceUmd_0021.o sceUmd_0022.o sceUmd_0023.o sceUmd_0024.o sceUmd_0025.o sceUmd_0026.o sceUmd_0027.o sceUmd_0028.o sceUmd_0029.o sceUmd_0030.o sceUmd_0031.o sceUmd_0032.o sceUmd_0033.o sceUmd_0034.o sceUmd_0035.o 

#ifdef F_sceUmd_0000
	IMPORT_START	"sceUmd",0x00010011
#endif
#ifdef F_sceUmd_0001
	IMPORT_FUNC	"sceUmd",0x041CDF0B,sceUmdSetAssignedFlag
#endif
#ifdef F_sceUmd_0002
	IMPORT_FUNC	"sceUmd",0x0FB60AAC,sceUmdGetDriveStatus
#endif
#ifdef F_sceUmd_0003
	IMPORT_FUNC	"sceUmd",0x1030D1D4,sceUmdRegisterUMDCallBack
#endif
#ifdef F_sceUmd_0004
	IMPORT_FUNC	"sceUmd",0x144AEF36,sceUmdRegisterActivateCallBack
#endif
#ifdef F_sceUmd_0005
	IMPORT_FUNC	"sceUmd",0x166A17CC,sceUmdReplaceProhibit
#endif
#ifdef F_sceUmd_0006
	IMPORT_FUNC	"sceUmd",0x1CEBD8F9,sceUmdGetUserEventFlagId
#endif
#ifdef F_sceUmd_0007
	IMPORT_FUNC	"sceUmd",0x29E04881,sceUmdSetSuspendResumeMode
#endif
#ifdef F_sceUmd_0008
	IMPORT_FUNC	"sceUmd",0x4BFC743B,sceUmdUnRegisterGetUMDInfoCallBack
#endif
#ifdef F_sceUmd_0009
	IMPORT_FUNC	"sceUmd",0x4F87AD56,sceUmdRegisterGetUMDInfoCallBack
#endif
#ifdef F_sceUmd_0010
	IMPORT_FUNC	"sceUmd",0x5174CDBE,sceUmdWaitDriveStatCB
#endif
#ifdef F_sceUmd_0011
	IMPORT_FUNC	"sceUmd",0x57E406EE,sceUmdRegisterReplaceCallBack
#endif
#ifdef F_sceUmd_0012
	IMPORT_FUNC	"sceUmd",0x626B1CF3,sceUmdGetErrorStatus
#endif
#ifdef F_sceUmd_0013
	IMPORT_FUNC	"sceUmd",0x64FDF83E,sceUmdReplacePermit
#endif
#ifdef F_sceUmd_0014
	IMPORT_FUNC	"sceUmd",0x6C2F4E69,sceUmd_075F1E0B
#endif
#ifdef F_sceUmd_0015
	IMPORT_FUNC	"sceUmd",0x6CE52E0B,sceUmdRegisterDeactivateCallBack
#endif
#ifdef F_sceUmd_0016
	IMPORT_FUNC	"sceUmd",0x70E69901,sceUmdUnRegisterDeactivateCallBack
#endif
#ifdef F_sceUmd_0017
	IMPORT_FUNC	"sceUmd",0x7C821F7F,sceUmdGetDetectUMDCallBackId
#endif
#ifdef F_sceUmd_0018
	IMPORT_FUNC	"sceUmd",0x7DC8CE8F,sceUmdClearDriveStatus
#endif
#ifdef F_sceUmd_0019
	IMPORT_FUNC	"sceUmd",0x8627B4CB,sceUmd_F8352373
#endif
#ifdef F_sceUmd_0020
	IMPORT_FUNC	"sceUmd",0x8EF08FCE,sceUmdWaitDriveStat
#endif
#ifdef F_sceUmd_0021
	IMPORT_FUNC	"sceUmd",0x90EBF462,sceUmdGetSuspendResumeMode
#endif
#ifdef F_sceUmd_0022
	IMPORT_FUNC	"sceUmd",0x999B8F89,sceUmdUnRegisterActivateCallBack
#endif
#ifdef F_sceUmd_0023
	IMPORT_FUNC	"sceUmd",0xAB82E180,sceUmdCancelWaitDriveStat
#endif
#ifdef F_sceUmd_0024
	IMPORT_FUNC	"sceUmd",0xB5990C6B,sceUmdWaitDriveStatWithTimer
#endif
#ifdef F_sceUmd_0025
	IMPORT_FUNC	"sceUmd",0xB5C92409,sceUmdSetErrorStatus
#endif
#ifdef F_sceUmd_0026
	IMPORT_FUNC	"sceUmd",0xC4F96162,sceUmd_4BA25F4A
#endif
#ifdef F_sceUmd_0027
	IMPORT_FUNC	"sceUmd",0xC6183D47,sceUmdActivate
#endif
#ifdef F_sceUmd_0028
	IMPORT_FUNC	"sceUmd",0xE0CB3442,sceUmdUnRegisterUMDCallBack
#endif
#ifdef F_sceUmd_0029
	IMPORT_FUNC	"sceUmd",0xE6EE2967,sceUmd_659587F7
#endif
#ifdef F_sceUmd_0030
	IMPORT_FUNC	"sceUmd",0xE83742BA,sceUmdDeactivate
#endif
#ifdef F_sceUmd_0031
	IMPORT_FUNC	"sceUmd",0xF2D0EDFB,sceUmdCheckMedium
#endif
#ifdef F_sceUmd_0032
	IMPORT_FUNC	"sceUmd",0xF35A78AE,sceUmdSetDriveStatus
#endif
#ifdef F_sceUmd_0033
	IMPORT_FUNC	"sceUmd",0xF635C9F7,sceUmd_5469DC37
#endif
#ifdef F_sceUmd_0034
	IMPORT_FUNC	"sceUmd",0xF7146B6C,sceUmdGetAssignedFlag
#endif
#ifdef F_sceUmd_0035
	IMPORT_FUNC	"sceUmd",0xF9A3B9B5,sceUmdGetDiscInfo
#endif
