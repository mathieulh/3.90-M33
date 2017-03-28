	.set noreorder

#include "pspstub.s"

	STUB_START "NpegLib",0x40090000,0x00030005
	STUB_FUNC  0x015E58A2,NpegOpen
	STUB_FUNC  0xD6927204,NpegReadBlock
	STUB_FUNC  0x2C8395BB,NpegClose
	STUB_END
