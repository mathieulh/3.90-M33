#include <pspsdk.h>
#include <pspuser.h>
#include <pspctrl.h>
#include <kubridge.h>
#include <systemctrl.h>
#include <systemctrl_se.h>
#include <string.h>

PSP_MODULE_INFO("BootLoader", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

#define	printf	pspDebugScreenPrintf

int main()
{
	pspDebugScreenInit();

	printf("Hola");

	printf("loadexec: 0x%08X\n", sctrlKernelLoadExecVSHWithApitype(0x141, "ms0:/PSP/GAME271/usbstorage/EBOOT.PBP", NULL));
	
	sceKernelDelayThread(10*1000*1000);
	sceKernelExitGame();
	
	return 0;
}

