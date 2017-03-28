	.set noreorder

#include "pspstub.s"

	STUB_START "reboot150",0x00090000,0x00010005
	STUB_FUNC  0x86E49DF4,reboot_start
	STUB_END
