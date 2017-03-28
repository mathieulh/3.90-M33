#include <pspsdk.h>
#include <pspkernel.h>
#include <stdio.h>
#include <string.h>


PSP_MODULE_INFO("DumperMain", 0x1000, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

int sceKernelCheckExecFile;
int a;


#define printf pspDebugScreenPrintf

void aa()
{
	a = sceKernelCheckExecFile;
}


int main() 
{

	pspDebugScreenInit();
	printf("Hola desde kernel.\n");
	

	sceKernelDelayThread(5*1000*1000);
	sceKernelExitGame();

	a = sceKernelCheckExecFile;

	return 0;
}
