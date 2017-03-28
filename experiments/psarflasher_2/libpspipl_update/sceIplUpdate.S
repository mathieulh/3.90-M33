	.set noreorder

#include "pspimport.s"

#ifdef F_sceIplUpdate_0000
	IMPORT_START	"sceIplUpdate",0x00090000
#endif
#ifdef F_sceIplUpdate_0001
	IMPORT_FUNC	"sceIplUpdate",0xEE7EB563,sceIplUpdateSetIpl
#endif
#ifdef F_sceIplUpdate_0002
	IMPORT_FUNC	"sceIplUpdate",0x26093B04,sceIplUpdateClearIpl
#endif

