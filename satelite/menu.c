#include <pspsdk.h>
#include <pspuser.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <psppaf.h>

#include <systemctrl_se.h>

#include "videoiso.h"

#include "blit.h"

#define TMENU_MAX			7
#define MAX_CPU_SPEEDS		9
#define ISO_VIDEO_MOUNT		4

static const char *top_menu_list[TMENU_MAX] =
{
	"CPU CLOCK XMB  ",
	"CPU CLOCK GAME ",
	"USB DEVICE     ",
	"UMD ISO MODE   ",
	"ISO VIDEO MOUNT",
	"RESET DEVICE",
	"EXIT"
};

static const char *item_str[TMENU_MAX];
static int item_fcolor[TMENU_MAX];

static SEConfig *config;
static int nvideoisos, *selected;

static char freq_buf_vsh[3+3+2];
static char freq_buf_umd[3+3+2];
static char usb_device[13];
static char umdmode[12];

static int menu_sel = 0;
int configchanged = 0;

extern int pwidth;

extern char buf[192];

static int cpu_speeds[] =
{
	0, 20, 75, 100, 133, 222, 266, 300, 333
};

static int bus_speeds[] =
{
	0, 10, 37,  50,  66, 111, 133, 150, 166
};

static const char* umd_modes[] =
{
	NULL, "OE isofs", "M33 driver", "Sony NP9660"
};

static int normal_coords[] =
{
	22*8,
	6*8,
	27*8,
	23*8,
	17*8,
	8,
	17
};

static int tvout_coords[] =
{
	19*8,
	6*8,
	24*8,
	20*8,
	14*8,
	8,
	14
};

static int GetCpuIndex(int cpu)
{
	int i;

	for (i = 0; i < MAX_CPU_SPEEDS; i++)
		if (cpu_speeds[i] == cpu)
			return i;

	return 0;
}

static int GetBusIndex(int bus)
{
	int i;

	for (i = 0; i < MAX_CPU_SPEEDS; i++)
		if (bus_speeds[i] == bus)
			return i;

	return 0;
}

int menu_setup(SEConfig *conf, VideoIso *isos, int nisos, int *sel)
{
	int i;

	config = conf;
	nvideoisos = nisos;
	selected = sel;
	
	for(i = 0;i < TMENU_MAX; i++)
	{
		item_str[i] = NULL;
		item_fcolor[i] = RGB(255,255,255);
	}

	if (GetCpuIndex(config->vshcpuspeed) && GetBusIndex(config->vshbusspeed))
	{
		paf_sprintf(freq_buf_vsh, "%d/%d", config->vshcpuspeed, config->vshbusspeed);
	}
	else
	{
		paf_strcpy(freq_buf_vsh, "Default");
	}

	item_str[0] = freq_buf_vsh;

	if (GetCpuIndex(config->umdisocpuspeed) && GetBusIndex(config->umdisobusspeed))
	{
		paf_sprintf(freq_buf_umd, "%d/%d", config->umdisocpuspeed, config->umdisobusspeed);
	}
	else
	{
		paf_strcpy(freq_buf_umd, "Default");
	}

	item_str[1] = freq_buf_umd;

	if (config->usbdevice >= 1 && config->usbdevice <= 4)
	{
		paf_sprintf(usb_device, "Flash %d", config->usbdevice-1);
	}
	else if (config->usbdevice == 5)
	{
		paf_strcpy(usb_device, "UMD Disc");
	}
	else
	{
		if (config->usbdevice != 0)
		{
			config->usbdevice = 0;
			configchanged = 1;
		}
		
		paf_strcpy(usb_device, "Memory Stick");
	}

	item_str[2] = usb_device;

	if (config->umdmode >= MODE_OE_LEGACY && config->umdmode <= MODE_NP9660)
	{
		paf_strcpy(umdmode, umd_modes[config->umdmode]);
	}
	else
	{
		if (config->umdmode != MODE_UMD)
		{
			config->umdmode = MODE_UMD;
			configchanged = 1;
		}

		paf_strcpy(umdmode, "Normal");
	}

	item_str[3] = umdmode;

	if (*sel == -1)
	{
		paf_strcpy(buf, "NONE");
	}
	else
	{
		paf_strcpy(buf, isos[*sel].filename);
	}

	item_str[4] = buf;

	return 0;
}

int menu_draw(void)
{
	u32 fc,bc;
	const char *msg;
	int menu_len;
	int max_menu;
	int *coords;
	
	if(blit_setup() < 0) return -1;

	if (pwidth == 720)
		coords = tvout_coords;
	else
		coords = normal_coords;

	blit_set_color(0xffffff,0x8000ff00);
	blit_string(coords[0], coords[1], "M33 VSH MENU");

	// menu title & list
	menu_len  = 15; 
	for(max_menu = 0;max_menu < TMENU_MAX; max_menu++)
	{
		fc = 0xffffff;
		bc = (max_menu==menu_sel) ? 0xff8080 : 0xc00000ff;
		blit_set_color(fc, bc);

		msg = top_menu_list[max_menu];
		if(msg)
		{
			int x;

			if (max_menu == (TMENU_MAX-1))
			{
				x = coords[2];
			}
			
			else if (max_menu == (TMENU_MAX-2))
			{
				x = coords[3];
			}

			else
			{
				x = coords[4];
			}

			blit_string(x,(coords[5]+max_menu)*8,msg);			

			msg = item_str[max_menu];
			if(msg)
			{
				blit_set_color(item_fcolor[max_menu],bc);
				blit_string((coords[6]+menu_len+1)*8,(coords[5]+max_menu)*8,msg);
			}
		}
	}

	blit_set_color(0x00ffffff,0x00000000);

	return 0;
}

static int limit(int val,int min,int max)
{
	if(val<min) val = max;
	if(val>max) val = min;
	return val;
}

int menu_ctrl(u32 buttons,u32 button_on)
{
	int direction;
	int index;
	//
	// check buttons 
	//
	button_on &=
		PSP_CTRL_SELECT|PSP_CTRL_START|
		PSP_CTRL_UP|PSP_CTRL_RIGHT|PSP_CTRL_DOWN|PSP_CTRL_LEFT|
		PSP_CTRL_LTRIGGER|PSP_CTRL_RTRIGGER|
		PSP_CTRL_TRIANGLE|PSP_CTRL_CIRCLE|PSP_CTRL_CROSS|PSP_CTRL_SQUARE|
		PSP_CTRL_HOME|PSP_CTRL_NOTE; // PSP_CTRL_HOLD

	// change menu select
	direction = 0;
	if(button_on&PSP_CTRL_DOWN) direction++;
	if(button_on&PSP_CTRL_UP) direction--;
	do
	{
		menu_sel = limit(menu_sel+direction, 0, TMENU_MAX-1);
	}while(top_menu_list[menu_sel]==NULL);

	// LEFT & RIGHT
	direction = -2;
	if (button_on & PSP_CTRL_LEFT)   direction = -1;
	if (button_on & PSP_CTRL_CROSS) direction = 0;
	if (button_on & PSP_CTRL_RIGHT)  direction = 1;
	if (button_on & (PSP_CTRL_SELECT | PSP_CTRL_HOME))
	{
		menu_sel = TMENU_MAX-1;
		direction = 0;
	}

	if(direction > -2)
	{
		if (!configchanged && direction && menu_sel != 4)
			configchanged = 1;
		
		switch(menu_sel)
		{
			case 0:
				
				if (direction)
				{
					index = limit(GetCpuIndex(config->vshcpuspeed)+direction, 0, MAX_CPU_SPEEDS-1);
					config->vshcpuspeed = cpu_speeds[index];
					config->vshbusspeed = bus_speeds[index];
				}

			break;

			case 1:
				
				if (direction)
				{
					index = limit(GetCpuIndex(config->umdisocpuspeed)+direction, 0, MAX_CPU_SPEEDS-1);
					config->umdisocpuspeed = cpu_speeds[index];
					config->umdisobusspeed = bus_speeds[index];
				}
				
			break;

			case 2:
				
				if (direction)
				{
					config->usbdevice = limit(config->usbdevice+direction, 0, 5);
				}

			break;

			case 3:
				
				if (direction > 0)
				{
					if (config->umdmode == MODE_UMD)
						config->umdmode = MODE_MARCH33;
					else if (config->umdmode == MODE_MARCH33)
						config->umdmode = MODE_NP9660;
					else if (config->umdmode == MODE_NP9660)
						config->umdmode = MODE_OE_LEGACY;
					else
						config->umdmode = MODE_UMD;
				}
				else if (direction < 0)
				{
					if (config->umdmode == MODE_UMD)
						config->umdmode = MODE_OE_LEGACY;
					else if (config->umdmode == MODE_OE_LEGACY)
						config->umdmode = MODE_NP9660;
					else if (config->umdmode == MODE_NP9660)
						config->umdmode = MODE_MARCH33;
					else
						config->umdmode = MODE_UMD;
				}

			break;

			case 4:

				if (direction)
					*selected = limit(*selected+direction, -1, nvideoisos-1);

			break;

			case 5:
				return -1;
			break;

			case (TMENU_MAX-1):

				if(direction == 0) 
				{
					return 0; // finish
				}

			break;
		}
	}

	return 1; // continue
}

