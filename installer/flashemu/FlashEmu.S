	.set noreorder

#include "pspstub.s"

	STUB_START "FlashEmu",0x00090000,0x00020005
	STUB_FUNC  0xD780E25C,InstallFlashEmu
	STUB_FUNC  0xDE3E3022,UninstallFlashEmu
	STUB_END
