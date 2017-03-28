	.set noreorder

#include "pspstub.s"

	STUB_START "Partition8Manager",0x40090000,0x00030005
	STUB_FUNC  0x84265167,Partition8TotalFreeMemsize
	STUB_FUNC  0xEA112DA6,Partition8MaxFreeMemsize
	STUB_FUNC  0x28A5009F,Partition8Malloc
	STUB_END
