	.set noreorder

#include "pspstub.s"

	STUB_START "pspDegeneration_driver",0x00090000,0x00020005
	STUB_FUNC  0x843F274A,pspDegCheckDegenerationState
	STUB_FUNC  0x9FA0B53E,pspDegCorrectDegeneration
	STUB_END
