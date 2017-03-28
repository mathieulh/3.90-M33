	.set noreorder

#include "pspstub.s"

	STUB_START "PRXDecrypter",0x40090000,0x00010005
	STUB_FUNC  0xBB5D4E66,decrypt_prx
	STUB_END
