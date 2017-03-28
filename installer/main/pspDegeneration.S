	.set noreorder

#include "pspstub.s"

	STUB_START "pspDegeneration",0x40090000,0x00020005
	STUB_FUNC  0x843F274A,pspDegCheckDegenerationState
	STUB_FUNC  0x9FA0B53E,pspDegCorrectDegeneration
	STUB_END
