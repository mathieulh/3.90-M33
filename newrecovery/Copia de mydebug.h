/*****************************************
 * Recovery - mydebug			 *
 * 			by harleyg :)	 *
 *****************************************/

#ifndef __MYDEBUG_H__
#define __MYDEBUG_H__

#include <psptypes.h>
#include <pspmoduleinfo.h>

#ifdef __cplusplus
extern "C" {
#endif

void myDebugScreenInit(void);

//#define myDebugScreenInit pspDebugScreenInit

void myDebugScreenPrintf(const char *fmt, ...) __attribute__((format(printf,1,2)));
//#define myDebugScreenPrintf pspDebugScreenPrintf

void myDebugScreenSetBackColor(u32 color);
//#define myDebugScreenSetBackColor pspDebugScreenSetBackColor

void myDebugScreenSetTextColor(u32 color);
//#define myDebugScreenSetTextColor pspDebugScreenSetTextColor

void myDebugScreenPutChar(int x, int y, u32 color, u8 ch);
//#define myDebugScreenPutChar pspDebugScreenPutChar

void myDebugScreenSetXY(int x, int y);

//#define myDebugScreenSetXY pspDebugScreenSetXY

void myDebugScreenSetOffset(int offset);
//#define myDebugScreenSetOffset pspDebugScreenSetOffset

int myDebugScreenGetX(void);
//#define myDebugScreenGetX pspDebugScreenGetX

int myDebugScreenGetY(void);
//#define myDebugScreenGetY pspDebugScreenGetY

void myDebugScreenClear(void);
//#define myDebugScreenClear pspDebugScreenClear

int myDebugScreenPrintData(const char *buff, int size);
//#define myDebugSCreenPrintData pspDebugScreenPrintData


#ifdef __cplusplus
}
#endif

#endif
