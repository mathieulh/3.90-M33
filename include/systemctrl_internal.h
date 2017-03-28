#ifndef __SYSTEMCTRL_INTERNAL_H__
#define __SYSTEMCTRL_INTERNAL_H__

#include <systemctrl.h>
#include <bootinfo.h>

typedef struct InitControl
{
	u32 text_addr;
	BootInfo *bootinfo;
	int  *apitype;
	char **filename;
	int  *mode;
	int  *powerlock_count;
	int (* FreeUnkParamAndModuleInfo)(BootInfo *);
	int (* FreeParamsAndBootInfo)(BootInfo *);
	int (* ProcessCallbacks)(int n);
} InitControl;

typedef int (* KDEC_HANDLER)(u32 *buf, int size, int *retSize, int m);
typedef int (* MDEC_HANDLER)(u32 *tag, u8 *keys, u32 code, u32 *buf, int size, int *retSize, int m, void *unk0, int unk1, int unk2, int unk3, int unk4);
typedef int (* LLE_HANDLER)(SceLibraryStubTable *import); 

int sctrlHENRegisterHomebrewLoader(void *func);
InitControl *sctrlHENGetInitControl();
void sctrlHENTakeInitControl(int (* ictrl)(InitControl *));
KDEC_HANDLER sctrlHENRegisterKDecryptHandler(KDEC_HANDLER handler);
MDEC_HANDLER sctrlHENRegisterMDecryptHandler(MDEC_HANDLER handler);
void sctrlHENRegisterLLEHandler(LLE_HANDLER handler);

#endif

