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

int doMenu(char* picks[], int count, int selected, char* message, int x) 
{	
	int done = 0;

	int y = 5;
	
	while (!done) 
	{
		SceCtrlData pad;
		int onepressed = 0;
		int i;
		
		setXY(0, 1);
		setTextColor(0x000000FF);
		printf("M33 Recovery Menu\n");
		printf("%s", message);

		int j = 0;

		for(i = 0; i < count; i++) 
		{
			setXY(x, y+i+j);

			if(picks[i] == 0) 
				break;

			if (strcmp(picks[i], "Back") == 0)
				j = 1;
			
			if(selected == i) 
			{
				setTextColor(0x000000FF);
				printf(" %s\n", picks[i]);
			} 
			else 
			{
				setTextColor(0x00FFFFFF);
				printf(" %s\n", picks[i]);
			}
		}

		setTextColor(0x000000FF);
		setXY(0, 23);

		printf(" ");
		for (i = 0; i < 67; i++)
		{
			printf("%c", '*');
		}
		printf("\n");
		
		while (!onepressed) 
		{
			sceCtrlReadBufferPositive(&pad, 1);
			
			onepressed = ((pad.Buttons & SELECTBUTTON ) ||
						 (pad.Buttons & CANCELBUTTON ) ||
						 (pad.Buttons & PSP_CTRL_UP  ) ||
						 (pad.Buttons & PSP_CTRL_DOWN));
		}

		if (pad.Buttons & SELECTBUTTON)	{ done = 1; }
		if (pad.Buttons & PSP_CTRL_UP)	{ selected = (selected + count - 1) % count; }
		if (pad.Buttons & PSP_CTRL_DOWN){ selected = (selected+1) % count; }
		if (pad.Buttons & CANCELBUTTON)	{ selected = -1; done = 1; }
		
		while (onepressed) 
		{
			sceCtrlReadBufferPositive(&pad, 1); 
			onepressed = ((pad.Buttons & SELECTBUTTON ) ||
						 (pad.Buttons & CANCELBUTTON ) ||
						 (pad.Buttons & PSP_CTRL_UP  ) ||
						 (pad.Buttons & PSP_CTRL_DOWN));
		}
	}

	return selected;
}
