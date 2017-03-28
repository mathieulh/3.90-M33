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

void myDebugScreenPrintf(const char *fmt, ...) __attribute__((format(printf,1,2)));

void myDebugScreenSetBackColor(u32 color);

void myDebugScreenSetTextColor(u32 color);

void myDebugScreenPutChar(int x, int y, u32 color, u8 ch);

void myDebugScreenSetXY(int x, int y);

void myDebugScreenSetOffset(int offset);

int myDebugScreenGetX(void);

int myDebugScreenGetY(void);

void myDebugScreenClear(void);

int myDebugScreenPrintData(const char *buff, int size);


#ifdef __cplusplus
}
#endif

#endif
