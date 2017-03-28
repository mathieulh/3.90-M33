	.set noreorder

#include "pspimport.s"

// Build List
// sceReg_0000.o sceReg_0001.o sceReg_0002.o sceReg_0003.o sceReg_0004.o sceReg_0005.o sceReg_0006.o sceReg_0007.o sceReg_0008.o sceReg_0009.o sceReg_0010.o sceReg_0011.o sceReg_0012.o sceReg_0013.o sceReg_0014.o sceReg_0015.o sceReg_0016.o sceReg_0017.o sceReg_0018.o sceReg_0019.o sceReg_0020.o 

#ifdef F_sceReg_0000
	IMPORT_START	"sceReg",0x40010011
#endif
#ifdef F_sceReg_0001
	IMPORT_FUNC	"sceReg",0x0CAE832B,sceRegCloseCategory
#endif
#ifdef F_sceReg_0002
	IMPORT_FUNC	"sceReg",0x0D69BF40,sceRegFlushCategory
#endif
#ifdef F_sceReg_0003
	IMPORT_FUNC	"sceReg",0x17768E14,sceRegSetKeyValue
#endif
#ifdef F_sceReg_0004
	IMPORT_FUNC	"sceReg",0x1D8A762E,sceRegOpenCategory
#endif
#ifdef F_sceReg_0005
	IMPORT_FUNC	"sceReg",0x28A8E98A,sceRegGetKeyValue
#endif
#ifdef F_sceReg_0006
	IMPORT_FUNC	"sceReg",0x2C0DB9DD,sceRegGetKeysNum
#endif
#ifdef F_sceReg_0007
	IMPORT_FUNC	"sceReg",0x2D211135,sceRegGetKeys
#endif
#ifdef F_sceReg_0008
	IMPORT_FUNC	"sceReg",0x30BE0259,sceRegGetKeyValueByName
#endif
#ifdef F_sceReg_0009
	IMPORT_FUNC	"sceReg",0x3615BC87,sceRegRemoveKey
#endif
#ifdef F_sceReg_0010
	IMPORT_FUNC	"sceReg",0x39461B4D,sceRegFlushRegistry
#endif
#ifdef F_sceReg_0011
	IMPORT_FUNC	"sceReg",0x4CA16893,sceRegRemoveCategory
#endif
#ifdef F_sceReg_0012
	IMPORT_FUNC	"sceReg",0x57641A81,sceRegCreateKey
#endif
#ifdef F_sceReg_0013
	IMPORT_FUNC	"sceReg",0x835ECE6F,sceReg_835ECE6F
#endif
#ifdef F_sceReg_0014
	IMPORT_FUNC	"sceReg",0x92E41280,sceRegOpenRegistry
#endif
#ifdef F_sceReg_0015
	IMPORT_FUNC	"sceReg",0x9B25EDF1,sceRegExit
#endif
#ifdef F_sceReg_0016
	IMPORT_FUNC	"sceReg",0xBE8C1263,sceReg_BE8C1263
#endif
#ifdef F_sceReg_0017
	IMPORT_FUNC	"sceReg",0xC5768D02,sceRegGetKeyInfoByName
#endif
#ifdef F_sceReg_0018
	IMPORT_FUNC	"sceReg",0xD4475AA8,sceRegGetKeyInfo
#endif
#ifdef F_sceReg_0019
	IMPORT_FUNC	"sceReg",0xDEDA92BF,sceRegRemoveRegistry
#endif
#ifdef F_sceReg_0020
	IMPORT_FUNC	"sceReg",0xFA8A5739,sceRegCloseRegistry
#endif
