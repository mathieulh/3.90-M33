#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <psploadexec_kernel.h>

#include <systemctrl.h>

#include "firmware.h"


PSP_MODULE_INFO("sceWlanFirmVoyager_driver", 0x1006, 1, 0);

int module_start(SceSize args, void *argp)
{
	int (* setfirm)(void *, int, void *, int) = sctrlHENFindFunction("sceWlan_Driver", "sceWlanDrv_driver", 0x20FB9FF7);

	setfirm(part1, sizeof(part1), part2, sizeof(part2));
	
	return 0;
}

int module_stop()
{
	return 0;
}




