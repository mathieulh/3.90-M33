/*
 * PSPLINK
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPLINK root for details.
 *
 * main.c - PSPLINK Debug/Impose menu
 *
 * Copyright (c) 2006 James F <tyranid@gmail.com>
 *
 * $HeadURL: svn://svn.ps2dev.org/psp/trunk/psplinkusb/tools/debugmenu/main.c $
 * $Id: main.c 2018 2006-10-07 16:54:19Z tyranid $
 */
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspkdebug.h>
#include <pspctrl_kernel.h>
#include <pspdisplay.h>
#include <pspdisplay_kernel.h>
#include <pspsdk.h>
#include <pspctrl.h>
#include <psppower.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PSP_MODULE_INFO("DebugMenu", PSP_MODULE_KERNEL, 1, 1);

#define START_MENU 1
static SceUID g_eventflag = -1;

void psplinkReset(void);

struct MenuOption
{
	const char *text;
	void (*do_option)(void);
};

void menu_exit_to_vsh(void)
{
	sceKernelExitGame();
}

void menu_power_off(void)
{
	scePowerRequestStandby();
}

struct MenuOption options[] = {
	{ "Exit to VSH\n", menu_exit_to_vsh },
	{ "Power Off\n", menu_power_off },
};

int opt_count = sizeof(options)/sizeof(struct MenuOption);

#define TRIGGER (PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER | PSP_CTRL_START | PSP_CTRL_SELECT)

void button_callback(int curr, int last, void *arg)
{
	if((curr & TRIGGER) == TRIGGER)
	{
		sceKernelSetEventFlag(g_eventflag, START_MENU);
	}
}

char str[32];

void redraw_menu(int selected)
{
	int i;

	pspDebugScreenSetXY(0, 0);
	pspDebugScreenSetTextColor(0xFFFFFFFF);
	pspDebugScreenPuts("PSPLINK Debug Menu\n\n\n");
	for(i = 0; i < opt_count; i++)
	{
		if(i == selected)
		{
			pspDebugScreenSetTextColor(0xFF00FF00);
			pspDebugScreenPuts(options[i].text);
		}
		else
		{
			pspDebugScreenSetTextColor(0xFFFFFFFF);
			pspDebugScreenPuts(options[i].text);
		}
	}

	int cpu = scePowerGetCpuClockFrequency();
	int bus = scePowerGetBusClockFrequency();

	sprintf(str, "%d/%d\n", cpu, bus);
	pspDebugScreenPuts(str);
}

void do_menu(void)
{
	SceCtrlData pad;
	int selected = 0;
	unsigned int lastbut = TRIGGER;
	unsigned int curr = 0;

	redraw_menu(selected);
	while(1)
	{
		sceCtrlPeekBufferPositive(&pad, 1);

		curr = pad.Buttons & ~lastbut;
		
		if(curr & PSP_CTRL_UP)
		{
			if(selected > 0)
			{
				selected--;
			}
			else if(selected == 0)
			{
				selected = opt_count-1;
			}
			redraw_menu(selected);
		}
		if(curr & PSP_CTRL_DOWN)
		{
			if(selected < (opt_count-1))
			{
				selected++;
			}
			else if(selected == (opt_count-1))
			{
				selected = 0;
			}
			redraw_menu(selected);
		}
		if(curr & PSP_CTRL_CIRCLE)
		{
			options[selected].do_option();
		}
		else if(curr & PSP_CTRL_CROSS)
		{
			return;
		}
		lastbut = pad.Buttons;
		if(sceDisplayWaitVblankStart() < 0)
		{
			sceKernelExitDeleteThread(0);
		}
	}
}

int main_thread(SceSize args, void *argp)
{
	SceUID block_id;
	void *vram;

	block_id = sceKernelAllocPartitionMemory(4, "debugmenu", PSP_SMEM_Low, 512*272*2, NULL);
	if(block_id < 0)
	{
		Kprintf("Error could not allocate memory buffer 0x%08X\n", block_id);
		goto error;
	}
	 
	vram = (void*) (0xA0000000 | (unsigned int) sceKernelGetBlockHeadAddr(block_id));
	g_eventflag = sceKernelCreateEventFlag("DebugMenuEvent", 0, 0, NULL);
	if(g_eventflag < 0)
	{
		Kprintf("Could not create event flag %08X\n", g_eventflag);
		goto error;
	}

	//sceCtrlRegisterButtonCallback(0, PSP_CTRL_HOME, button_callback, NULL);
	sceCtrlRegisterButtonCallback(3, TRIGGER, button_callback, NULL);
	while(1)
	{
		unsigned int bits;
		if(sceKernelWaitEventFlag(g_eventflag, START_MENU,
					PSP_EVENT_WAITOR | PSP_EVENT_WAITCLEAR, &bits, NULL) < 0)
		{
			break;
		}
		sceCtrlSetButtonMasks(0xFFFF, 1);  // Mask lower 16bits
		sceCtrlSetButtonMasks(0x10000, 2); // Always return HOME key
		sceDisplaySetFrameBufferInternal(0, vram, 512, 0, 1);
		pspDebugScreenInitEx(vram, 0, 0);
		do_menu();
		sceDisplaySetFrameBufferInternal(0, 0, 512, 0, 1);
		sceCtrlSetButtonMasks(0x10000, 0); // Unset HOME key
		sceCtrlSetButtonMasks(0xFFFF, 0);  // Unset mask
		sceKernelClearEventFlag(g_eventflag, ~START_MENU);
	}

error:
	sceKernelExitDeleteThread(0);

	return 0;
}

/* Entry point */
int module_start(SceSize args, void *argp)
{
	int thid;

	thid = sceKernelCreateThread("DebugMenu", main_thread, 15, 0x800, 0, NULL);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, args, argp);
	}
	return 0;
}

/* Module stop entry */
int module_stop(SceSize args, void *argp)
{
	return 0;
}
