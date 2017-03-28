.set noreorder

#include "pspstub.s"

   STUB_START   "sceGpio_driver",0x00010000,0x00120005
   STUB_FUNC   0xEABDB328,sceGpioInit
   STUB_FUNC   0x4A992B24,sceGpioEnd
   STUB_FUNC   0x17DAA8C2,sceGpioSuspend
   STUB_FUNC   0x64CD4536,sceGpioResume
   STUB_FUNC   0x4250D44A,sceGpioPortRead
   STUB_FUNC   0x310F0CCF,sceGpioPortSet
   STUB_FUNC   0x103C3EB2,sceGpioPortClear
   STUB_FUNC   0x95135905,sceGpio_driver_95135905
   STUB_FUNC   0x317D9D2C,sceGpioSetPortMode
   STUB_FUNC   0xCA8BE2EA,sceGpioGetPortMode
   STUB_FUNC   0x37C8DADC,sceGpioSetIntrMode
   STUB_FUNC   0xF856CE46,sceGpioGetIntrMode
   STUB_FUNC   0x785206CD,sceGpioEnableIntr
   STUB_FUNC   0x95D7F3B8,sceGpioDisableIntr
   STUB_FUNC   0x31F34AE6,sceGpioQueryIntr
   STUB_FUNC   0xBE77D1D0,sceGpioAcquireIntr
   STUB_FUNC   0xC6928224,sceGpio_driver_C6928224
   STUB_FUNC   0x6B38B826,sceGpio_driver_6B38B826
   STUB_END

