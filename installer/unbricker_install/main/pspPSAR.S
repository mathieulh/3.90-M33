	.set noreorder

#include "pspstub.s"

	STUB_START "pspPSAR",0x40090000,0x00020005
	STUB_FUNC  0x56C924E1,pspPSARInit
	STUB_FUNC  0x9C91DD9D,pspPSARGetNextFile
	STUB_END
