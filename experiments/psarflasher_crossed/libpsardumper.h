#ifndef __LIBPSARDUMPER_H__
#define __LIBPSARDUMPER_H__


int pspPSARInit(u8 *dataPSAR, u8 *dataOut, u8 *dataOut2);
int pspPSARGetNextFile(u8 *dataPSAR, int cbFile, u8 *dataOut, u8 *dataOut2, char *name, int *retSize, int *retPos);



#endif
