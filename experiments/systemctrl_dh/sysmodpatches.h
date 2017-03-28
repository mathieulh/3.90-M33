#ifndef __SYSMODPATCHES_H__
#define __SYSMODPATCHES_H__

#include "systemctrl_se.h"

void SetConfig(SEConfig *newconfig);
void PatchUmdMan(char *buf);
void PatchInitLoadExecAndMediaSync(char *buf);
void PatchVshMain(char *buf);
void PatchSysconfPlugin(char *buf);
void OnImposeLoad();
void DoNoUmdPatches();
void PatchIsofsDriver();

#endif

