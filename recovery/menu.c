/*****************************************
 * Recovery - menu			 *
 * 			by harleyg :)	 *
 * 	based on menu example by danzel	 *
 *****************************************/
#include <pspctrl.h>
#include "mydebug.h"

#define SELECTBUTTON PSP_CTRL_CROSS
#define CANCELBUTTON PSP_CTRL_TRIANGLE
#define RGB(r, g, b) (0xFF000000 | ((b)<<16) | ((g)<<8) | (r))
#define printf myDebugScreenPrintf
#define setTextColor myDebugScreenSetTextColor
#define setXY myDebugScreenSetXY
#define clearScreen myDebugScreenClear

int doMenu(char* picks[], int count, int selected, char* message, int x, int y) {
	int done = 0;
	while (!done) {
		SceCtrlData pad;
		int onepressed = 0;
		int i;
		
		setXY(0, 1);
		setTextColor(RGB(0, 0, 128));
		printf(" Recovery mode\n");
		/*setTextColor(RGB(216, 216, 216));
		printf(" by harleyg\n\n");
		setTextColor(RGB(0, 0, 0));*/
		printf(" %s", message);

		for(i=0;i<count;i++) {
			setXY(x, y+i);
			if(picks[i] == 0) break;
			if(selected == i) {
				setTextColor(RGB(0, 0, 128));
				printf(" %s\n", picks[i]);
			} else {
				setTextColor(RGB(190, 190, 190));
				printf(" %s\n", picks[i]);
			}
		}
		setTextColor(RGB(0, 0, 128));
		setXY(0, 20);
		printf(" ------------------------------------------------------------------ \n\n");
			
		while (!onepressed) {
			sceCtrlReadBufferPositive(&pad, 1);
			onepressed = (  (pad.Buttons & SELECTBUTTON ) ||
					(pad.Buttons & CANCELBUTTON ) ||
					(pad.Buttons & PSP_CTRL_UP  ) ||
					(pad.Buttons & PSP_CTRL_DOWN));
		}
		if (pad.Buttons & SELECTBUTTON)	{ done = 1; }
		if (pad.Buttons & PSP_CTRL_UP)	{ selected = (selected + count - 1) % count; }
		if (pad.Buttons & PSP_CTRL_DOWN){ selected = (selected+1) % count; }
		if (pad.Buttons & CANCELBUTTON)	{ done = 1; selected = -1; }
		while (onepressed) {
			sceCtrlReadBufferPositive(&pad, 1); 
			onepressed = (  (pad.Buttons & SELECTBUTTON ) ||
					(pad.Buttons & CANCELBUTTON ) ||
					(pad.Buttons & PSP_CTRL_UP  ) ||
					(pad.Buttons & PSP_CTRL_DOWN));
		}
	}
	return selected;
}
