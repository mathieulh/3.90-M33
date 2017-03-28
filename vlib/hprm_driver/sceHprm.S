	.set noreorder

#include "pspimport.s"

// Build List
// sceHprm_0000.o sceHprm_0001.o sceHprm_0002.o sceHprm_0003.o sceHprm_0004.o sceHprm_0005.o sceHprm_0006.o sceHprm_0007.o sceHprm_0008.o sceHprm_0009.o sceHprm_0010.o 

#ifdef F_sceHprm_0000
	IMPORT_START	"sceHprm",0x40010011
#endif
#ifdef F_sceHprm_0001
	IMPORT_FUNC	"sceHprm",0x1910B327,sceHprmPeekCurrentKey
#endif
#ifdef F_sceHprm_0002
	IMPORT_FUNC	"sceHprm",0x208DB1BD,sceHprmIsRemoteExist
#endif
#ifdef F_sceHprm_0003
	IMPORT_FUNC	"sceHprm",0x219C58F1,sceHprmIsMicrophoneExist
#endif
#ifdef F_sceHprm_0004
	IMPORT_FUNC	"sceHprm",0x2BCEC83E,sceHprmPeekLatch
#endif
#ifdef F_sceHprm_0005
	IMPORT_FUNC	"sceHprm",0x40D2F9F0,sceHprmReadLatch
#endif
#ifdef F_sceHprm_0006
	IMPORT_FUNC	"sceHprm",0x444ED0B7,sceHprmUnregisterCallback
#endif
#ifdef F_sceHprm_0007
	IMPORT_FUNC	"sceHprm",0x71B5FB67,sceHprm_71B5FB67
#endif
#ifdef F_sceHprm_0008
	IMPORT_FUNC	"sceHprm",0x7E69EDA4,sceHprmIsHeadphoneExist
#endif
#ifdef F_sceHprm_0009
	IMPORT_FUNC	"sceHprm",0xC7154136,sceHprmRegisterCallback
#endif
#ifdef F_sceHprm_0010
	IMPORT_FUNC	"sceHprm",0xFD7DE6CD,sceHprm_FD7DE6CD
#endif
