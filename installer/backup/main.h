//**************************************************************************
//		PSP Project: 'PSPDMPR' - main.h
//**************************************************************************

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspnand_driver.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define NAND_STATUS (*((volatile unsigned *)0xBD101004))
#define NAND_COMMAND (*((volatile unsigned *)0xBD101008))
#define NAND_ADDRESS (*((volatile unsigned *)0xBD10100C))
#define NAND_READDATA (*((volatile unsigned *)0xBD101300))
#define NAND_ENDTRANS (*((volatile unsigned *)0xBD101014))

#define printf pspDebugScreenPrintf

//#define USE_API_METHOD
#define USE_LOWLEVEL_METHOD

int flashLocked = 0;
int currentY = 0;

int main(int argc, char **argv);

void nandAPI_readPageSpare(unsigned n, void *s);
void nandAPI_readPageData(unsigned n, void *s);
void nandLow_readPageSpare2(unsigned n, void *s);
void nandLow_readPageData2(unsigned n, void *b);

void LockFlash(void);
void UnlockFlash(void);

void DrawLayout(void);
void DrawProgress(int p);

