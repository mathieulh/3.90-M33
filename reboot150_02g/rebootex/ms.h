#ifndef __MS_H__
#define __MS_H__

void pspMsInit(void);
int  pspMsReadSector(int sector, void *addr);
int  pspMsWriteSector(int sector, void *addr);


#endif

