#include <pspsdk.h>
#include <pspuser.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <psppaf.h>

#include <systemctrl_se.h>
#include <vshctrl.h>

#include "videoiso.h"

PSP_MODULE_INFO("VshCtrlSatelite", 0, 1, 0);
PSP_MODULE_SDK_VERSION(0x03080110);

#define ALL_ALLOW    (PSP_CTRL_UP|PSP_CTRL_RIGHT|PSP_CTRL_DOWN|PSP_CTRL_LEFT)
#define ALL_BUTTON   (PSP_CTRL_TRIANGLE|PSP_CTRL_CIRCLE|PSP_CTRL_CROSS|PSP_CTRL_SQUARE)
#define ALL_TRIGGER  (PSP_CTRL_LTRIGGER|PSP_CTRL_RTRIGGER)
#define ALL_FUNCTION (PSP_CTRL_SELECT|PSP_CTRL_START|PSP_CTRL_HOME|PSP_CTRL_HOLD|PSP_CTRL_NOTE)
#define ALL_CTRL     (ALL_ALLOW|ALL_BUTTON|ALL_TRIGGER|ALL_FUNCTION)

static SceUID thid;
static int stopit = 0;
static SceCtrlData ctrl_pad;
static u32 cur_buttons = 0;
static u32 button_on  = 0;
static int menu_mode=0;

static SEConfig config;

static char videoiso_sel[128];
static VideoIso *videoisos;
static int nvideoisos;
static int selected=-1;

static int inited = 0;

extern char buf[192];
extern int configchanged;

int CtrlHooked(SceCtrlData *pad_data, int count)
{
	u32 buttons;
	int i;

	paf_memcpy(&ctrl_pad, pad_data, sizeof(ctrl_pad));

	buttons     = ctrl_pad.Buttons;
	button_on   = ~cur_buttons & buttons;
	cur_buttons = buttons;

	for (i = 0; i < count; i++)
	{
		pad_data[i].Buttons &= ~(
		PSP_CTRL_SELECT|PSP_CTRL_START|
		PSP_CTRL_UP|PSP_CTRL_RIGHT|PSP_CTRL_DOWN|PSP_CTRL_LEFT|
		PSP_CTRL_LTRIGGER|PSP_CTRL_RTRIGGER|
		PSP_CTRL_TRIANGLE|PSP_CTRL_CIRCLE|PSP_CTRL_CROSS|PSP_CTRL_SQUARE|
		PSP_CTRL_HOME|PSP_CTRL_NOTE);
	}

	return 0;
}

static void button_func(void)
{
	if (menu_mode == 0)
	{
		if (cur_buttons && (cur_buttons & PSP_CTRL_SELECT) == 0)
		{
			menu_mode = 1;
		}
	}
	else if (menu_mode == 1)
	{
		if (!inited)
		{
			int i;
			
			GetVideoIsos(&videoisos, &nvideoisos);
			
			for (i = 0; i < nvideoisos; i++)
			{
				if (paf_memcmp(videoiso_sel, videoisos[i].filename, paf_strlen(videoisos[i].filename)) == 0)
				{
					selected = i;
					break;
				}
			}

			inited = 1;
		}
		else
		{
			int res = menu_ctrl(cur_buttons, button_on);
			if (res == 0) 
			{
				menu_mode = 2;
			}

			else if (res == -1)
			{
				menu_mode = 3;
			}
		}
	}
	else if (menu_mode == 2)
	{
		if((cur_buttons & ALL_CTRL) == 0)
		{
			menu_mode = 0;
			stopit = 1;
		}
	}

	else if (menu_mode == 3)
	{
		if((cur_buttons & ALL_CTRL) == 0)
		{
			menu_mode = 0;
			stopit = 2;
		}
	}
}

int vshmenu_thread(SceSize args, void *argp)
{
	sceKernelChangeThreadPriority(0, 8);
	//sceKernelChangeThreadPriority(0, 24);

	sctrlSEGetConfig(&config);
	vctrlVSHRegisterVshMenu(CtrlHooked);

	while(1)
	{
		sceDisplayWaitVblankStart();
		
		// stop request
		if(stopit) 
			break;

		if (inited == 1)
		{
			menu_setup(&config, videoisos, nvideoisos, &selected);
			inited = 2;
		}
		else if (inited == 2)
		{
			menu_draw();
			menu_setup(&config, videoisos, nvideoisos, &selected);
		}
		
		button_func();
	}

	if (configchanged)
		sctrlSESetConfig(&config);

	if (stopit == 2)
	{
		scePower_0442D852(0);		
	}

	int  disctype = 0;
	char *p = NULL;

	if (selected != -1)
	{
		paf_snprintf(buf, sizeof(buf), "ms0:/ISO/VIDEO/%s", videoisos[selected].filename);
		p = buf;
		disctype = videoisos[selected].disctype;
	}
	
	vctrlVSHExitVSHMenu(&config, p, disctype);
	return sceKernelExitDeleteThread(0);
}

int module_start(SceSize args, void *argp)
{
	if (args != 0)
	{
		char *str = argp;
		int i;

		for (i = paf_strlen(str)-2; i >= 0; i--)
		{
			if (str[i] == '/')
			{
				paf_strcpy(videoiso_sel, str+i+1);
				break;
			}
		}
	}
	
	thid = sceKernelCreateThread("VshMenu_Thread", vshmenu_thread, 0x10, 0x1000, 0, 0);
	
	if(thid >= 0)
		sceKernelStartThread(thid, 0, 0);
	
	return 0;
}

int module_stop(SceSize args, void *argp)
{
	u32 timeout = 100000;

	stopit = 1;

	if (sceKernelWaitThreadEnd(thid, &timeout) < 0)
		sceKernelTerminateDeleteThread(thid);

	return 0;
}
