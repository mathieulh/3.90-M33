#include <pspsdk.h>
#include <pspsysmem.h>

PSP_MODULE_INFO("lcpatcher", 0x0007, 1, 0);

//PSP_MAIN_THREAD_ATTR(0);

int module_start(SceSize args, void *argp)
{	
	u8 *input = (u8 *)0x88100000;
	u8 *output = (u8 *)0x883e0000;
	int i;

	for (i = 0; i < 0x20000; i++)
	{
		output[i] = input[i];
	}
	
	// 1.50 NoPlainModuleCheckPatch
	_sw(0x340D0001, 0x880152e0);	

	// return 1; // Exit module
	return 0; // Keep module (for systemctrl150 to detect it)
}

