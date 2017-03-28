#ifndef __SYSMODPATCHES_H__
#define __SYSMODPATCHES_H__

#include "systemctrl_se.h"

void SetConfig(SEConfig *newconfig);
SEConfig *GetConfig();
void PatchUmdMan(char *buf);
void PatchNandDriver(char *buf);
void PatchInitLoadExecAndMediaSync(char *buf);
void PatchVshMain(char *buf);
void PatchSysconfPlugin(char *buf);
void OnImposeLoad();
void DoNoUmdPatches();
void PatchIsofsDriver(char *buf);
void PatchWlan(char *buf);
void PatchPower(char *buf);


#endif

