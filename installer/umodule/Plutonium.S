	.set noreorder

#include "pspstub.s"

	STUB_START "Plutonium",0x40090000,0x00030005
	STUB_FUNC  0x340AC1E4,PlutoniumGetModel
	STUB_FUNC  0xF8547F11,PlutoniumStartUpdate
	STUB_FUNC  0xE30ED0F0,PlutoniumRebootPsp
	STUB_END
