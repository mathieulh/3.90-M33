#ifndef __SYSMODPATCHES_H__
#define __SYSMODPATCHES_H__

#include "systemctrl_se.h"

u32 FindPowerFunction(u32 nid);
void SetConfig(SEConfig *newconfig);
void SetBootFile(int n);
SEConfig *GetConfig();
void PatchUmdMan(u32 text_addr);
void PatchUmdCache(u32 text_addr);
void PatchNandDriver(u32 text_addr);
void PatchInitLoadExecAndMediaSync(u32 text_addr);
void OnImposeLoad(u32 text_addr);
void DoNoUmdPatches();
void PatchIsofsDriver(u32 text_addr);
void PatchWlan(u32 text_addr);
void PatchPower(u32 text_addr);
void OnSystemStatusIdle();

#endif

