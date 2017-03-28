#
# stub normaly Kernel to Kernel link
#
	.set noreorder

#include "pspstub.s"

STUB_START "SysMemForKernel",0x40010000,0x00020005 
  STUB_FUNC 0x419DB8F4,SysMemForKernel_419DB8F4 
STUB_END 

STUB_START "scePopsMan_driver",0x40010000,0x00010005 
  STUB_FUNC 0x92E9E38D,scePopsMan_driver_92E9E38D 
STUB_END 

STUB_START "scePspNpDrm_driver",0x40010000,0x00010005 
  STUB_FUNC 0x5667B7B9,scePspNpDrm_driver_5667B7B9 
STUB_END

STUB_START "LoadExecForKernel",0x40010000,0x00010005 
  STUB_FUNC 0x6D302D3D,sceKernelExitVSHKernel 
STUB_END

