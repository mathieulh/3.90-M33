	.set noreorder

#include "pspimport.s"

// Build List
// sceWlanDrv_driver_0000.o sceWlanDrv_driver_0001.o sceWlanDrv_driver_0002.o sceWlanDrv_driver_0003.o sceWlanDrv_driver_0004.o sceWlanDrv_driver_0005.o sceWlanDrv_driver_0006.o sceWlanDrv_driver_0007.o sceWlanDrv_driver_0008.o sceWlanDrv_driver_0009.o sceWlanDrv_driver_0010.o sceWlanDrv_driver_0011.o sceWlanDrv_driver_0012.o sceWlanDrv_driver_0013.o sceWlanDrv_driver_0014.o sceWlanDrv_driver_0015.o sceWlanDrv_driver_0016.o sceWlanDrv_driver_0017.o sceWlanDrv_driver_0018.o sceWlanDrv_driver_0019.o sceWlanDrv_driver_0020.o sceWlanDrv_driver_0021.o sceWlanDrv_driver_0022.o sceWlanDrv_driver_0023.o sceWlanDrv_driver_0024.o sceWlanDrv_driver_0025.o sceWlanDrv_driver_0026.o sceWlanDrv_driver_0027.o sceWlanDrv_driver_0028.o 

#ifdef F_sceWlanDrv_driver_0000
	IMPORT_START	"sceWlanDrv_driver",0x00010011
#endif
#ifdef F_sceWlanDrv_driver_0001
	IMPORT_FUNC	"sceWlanDrv_driver",0x05FE320C,sceWlanDevGetStateGPIO
#endif
#ifdef F_sceWlanDrv_driver_0002
	IMPORT_FUNC	"sceWlanDrv_driver",0x19E51F54,sceWlanDevGetMacAddr
#endif
#ifdef F_sceWlanDrv_driver_0003
	IMPORT_FUNC	"sceWlanDrv_driver",0x2D0FAE4E,sceWlanDrv_driver_2D0FAE4E
#endif
#ifdef F_sceWlanDrv_driver_0004
	IMPORT_FUNC	"sceWlanDrv_driver",0x40B0AA4A,sceWlanDrv_driver_40B0AA4A
#endif
#ifdef F_sceWlanDrv_driver_0005
	IMPORT_FUNC	"sceWlanDrv_driver",0x44A99F9D,sceWlanGetDropRate
#endif
#ifdef F_sceWlanDrv_driver_0006
	IMPORT_FUNC	"sceWlanDrv_driver",0x482CAE9A,sceWlanDevAttach
#endif
#ifdef F_sceWlanDrv_driver_0007
	IMPORT_FUNC	"sceWlanDrv_driver",0x4C14BACA,sceWlanDrv_driver_4C14BACA
#endif
#ifdef F_sceWlanDrv_driver_0008
	IMPORT_FUNC	"sceWlanDrv_driver",0x4C7F62E0,sceWlanGPUnRegisterCallback
#endif
#ifdef F_sceWlanDrv_driver_0009
	IMPORT_FUNC	"sceWlanDrv_driver",0x56F467CA,sceWlanDrv_driver_56F467CA
#endif
#ifdef F_sceWlanDrv_driver_0010
	IMPORT_FUNC	"sceWlanDrv_driver",0x5BAA1FE5,sceWlanDrv_driver_5BAA1FE5
#endif
#ifdef F_sceWlanDrv_driver_0011
	IMPORT_FUNC	"sceWlanDrv_driver",0x5E7C8D94,sceWlanDevIsGameMode
#endif
#ifdef F_sceWlanDrv_driver_0012
	IMPORT_FUNC	"sceWlanDrv_driver",0x5ED4049A,sceWlanGPPrevEstablishActive
#endif
#ifdef F_sceWlanDrv_driver_0013
	IMPORT_FUNC	"sceWlanDrv_driver",0x6D5C839E,sceWlanDrv_driver_6D5C839E
#endif
#ifdef F_sceWlanDrv_driver_0014
	IMPORT_FUNC	"sceWlanDrv_driver",0x6D89822C,sceWlanDevInit
#endif
#ifdef F_sceWlanDrv_driver_0015
	IMPORT_FUNC	"sceWlanDrv_driver",0x749B813A,sceWlanSetHostDiscover
#endif
#ifdef F_sceWlanDrv_driver_0016
	IMPORT_FUNC	"sceWlanDrv_driver",0x7FF54BD2,sceWlanDevSetGPIO
#endif
#ifdef F_sceWlanDrv_driver_0017
	IMPORT_FUNC	"sceWlanDrv_driver",0x81579D36,sceWlanDrv_driver_81579D36
#endif
#ifdef F_sceWlanDrv_driver_0018
	IMPORT_FUNC	"sceWlanDrv_driver",0x8D5F551B,sceWlanDrv_driver_8D5F551B
#endif
#ifdef F_sceWlanDrv_driver_0019
	IMPORT_FUNC	"sceWlanDrv_driver",0x93440B11,sceWlanDevIsPowerOn
#endif
#ifdef F_sceWlanDrv_driver_0020
	IMPORT_FUNC	"sceWlanDrv_driver",0x9658C9F7,sceWlanGPRegisterCallback
#endif
#ifdef F_sceWlanDrv_driver_0021
	IMPORT_FUNC	"sceWlanDrv_driver",0xA447103A,sceWlanGPRecv
#endif
#ifdef F_sceWlanDrv_driver_0022
	IMPORT_FUNC	"sceWlanDrv_driver",0xB4D7CB74,sceWlanGPSend
#endif
#ifdef F_sceWlanDrv_driver_0023
	IMPORT_FUNC	"sceWlanDrv_driver",0xC9A8CAB7,sceWlanDevDetach
#endif
#ifdef F_sceWlanDrv_driver_0024
	IMPORT_FUNC	"sceWlanDrv_driver",0xE50312AB,sceWlanSetFirm
#endif
#ifdef F_sceWlanDrv_driver_0025
	IMPORT_FUNC	"sceWlanDrv_driver",0xEB3235B5,sceWlanSetDropRate
#endif
#ifdef F_sceWlanDrv_driver_0026
	IMPORT_FUNC	"sceWlanDrv_driver",0xEC9232F0,sceWlanDevEnd
#endif
#ifdef F_sceWlanDrv_driver_0027
	IMPORT_FUNC	"sceWlanDrv_driver",0xFBD385FB,sceWlanDropCheck
#endif
#ifdef F_sceWlanDrv_driver_0028
	IMPORT_FUNC	"sceWlanDrv_driver",0xFE8A0B46,sceWlanSetWakeUp
#endif
