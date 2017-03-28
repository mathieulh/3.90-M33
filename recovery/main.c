/*****************************************
 * Recovery				 *
 * 			by harleyg :)	 *
 *****************************************/

#include <pspkernel.h>
#include <pspsdk.h>
#include <pspusb.h>
#include <pspusbstor.h>
#include <psploadexec_kernel.h>
#include <pspsuspend.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mydebug.h"
#include "menu.h"
#include "conf.h"
#include <pspreg.h>

PSP_MODULE_INFO("Recovery mode", 0x1000, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

#define printf myDebugScreenPrintf
#define RGB(r, g, b) (0xFF000000 | ((b)<<16) | ((g)<<8) | (r))
#define setTextColor myDebugScreenSetTextColor
#define setBackColor myDebugScreenSetBackColor
#define setXY myDebugScreenSetXY
#define delay sceKernelDelayThread
#define clearScreen myDebugScreenClear

SEConfig config;
int configchanged = 0;

int usbStatus = 0;
int usbModuleStatus = 0;

PspIoDrv *lflash_driver;
PspIoDrv *msstor_driver;

int get_registry_value(const char *dir, const char *name, unsigned int *val)
{
        int ret = 0;
        struct RegParam reg;
        REGHANDLE h;

        memset(&reg, 0, sizeof(reg));
        reg.regtype = 1;
        reg.namelen = strlen("/system");
        reg.unk2 = 1;
        reg.unk3 = 1;
        strcpy(reg.name, "/system");
        if(sceRegOpenRegistry(&reg, 2, &h) == 0)
        {
                REGHANDLE hd;
                if(!sceRegOpenCategory(h, dir, 2, &hd))
                {
                        REGHANDLE hk;
                        unsigned int type, size;

                        if(!sceRegGetKeyInfo(hd, name, &hk, &type, &size))
                        {
                                if(!sceRegGetKeyValue(hd, hk, val, 4))
                                {
                                        ret = 1;
                                        sceRegFlushCategory(hd);
                                }
                        }
                        sceRegCloseCategory(hd);
                }
                sceRegFlushRegistry(h);
                sceRegCloseRegistry(h);
        }

        return ret;
}

int set_registry_value(const char *dir, const char *name, unsigned int val)
{
        int ret = 0;
        struct RegParam reg;
        REGHANDLE h;

        memset(&reg, 0, sizeof(reg));
        reg.regtype = 1;
        reg.namelen = strlen("/system");
        reg.unk2 = 1;
        reg.unk3 = 1;
        strcpy(reg.name, "/system");
        if(sceRegOpenRegistry(&reg, 2, &h) == 0)
        {
                REGHANDLE hd;
                if(!sceRegOpenCategory(h, dir, 2, &hd))
                {
                        if(!sceRegSetKeyValue(hd, name, &val, 4))
                        {
                                ret = 1;
                                sceRegFlushCategory(hd);
                        }
						else
						{
							sceRegCreateKey(hd, name, REG_TYPE_INT, 4);
							sceRegSetKeyValue(hd, name, &val, 4);
							ret = 1;
                            sceRegFlushCategory(hd);
						}
                        sceRegCloseCategory(hd);
                }
                sceRegFlushRegistry(h);
                sceRegCloseRegistry(h);
        }

        return ret;
}

int ReadLine(SceUID fd, char *str)
{
	char ch = 0;
	int n = 0;

	while (1)
	{	
		if (sceIoRead(fd, &ch, 1) != 1)
			return n;

		if (ch < 0x20)
		{
			if (n != 0)
				return n;
		}
		else
		{
			*str++ = ch;
			n++;
		}
	}
	
}

int (* Orig_IoOpen)(PspIoDrvFileArg *arg, char *file, int flags, SceMode mode);
int (* Orig_IoClose)(PspIoDrvFileArg *arg);
int (* Orig_IoRead)(PspIoDrvFileArg *arg, char *data, int len);
int (* Orig_IoWrite)(PspIoDrvFileArg *arg, const char *data, int len);
SceOff(* Orig_IoLseek)(PspIoDrvFileArg *arg, SceOff ofs, int whence);
int (* Orig_IoIoctl)(PspIoDrvFileArg *arg, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen);
int (* Orig_IoDevctl)(PspIoDrvFileArg *arg, const char *devname, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen);

int unit;

/* 1.50 specific function */
PspIoDrv *FindDriver(char *drvname)
{
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceIOFileManager");

	if (!mod)
	{
		return NULL;
	}

	u32 text_addr = *(mod+27);

	u32 *(* GetDevice)(char *) = (void *)(text_addr+0x16D4);
	u32 *u;

	u = GetDevice(drvname);

	if (!u)
	{
		return NULL;
	}

	return (PspIoDrv *)u[1];
}

static int New_IoOpen(PspIoDrvFileArg *arg, char *file, int flags, SceMode mode)
{
	if (!lflash_driver->funcs->IoOpen)
		return -1;

	if (unit == 0)
		file = "0,0";
	else
		file = "0,1";

	return lflash_driver->funcs->IoOpen(arg, file, flags, mode);
}

static int New_IoClose(PspIoDrvFileArg *arg)
{
	if (!lflash_driver->funcs->IoClose)
		return -1;

	return lflash_driver->funcs->IoClose(arg);
}

static int New_IoRead(PspIoDrvFileArg *arg, char *data, int len)
{
	if (!lflash_driver->funcs->IoRead)
		return -1;

	return lflash_driver->funcs->IoRead(arg, data, len);
}
static int New_IoWrite(PspIoDrvFileArg *arg, const char *data, int len)
{
	if (!lflash_driver->funcs->IoWrite)
		return -1;

	return lflash_driver->funcs->IoWrite(arg, data, len);
}

static SceOff New_IoLseek(PspIoDrvFileArg *arg, SceOff ofs, int whence)
{
	if (!lflash_driver->funcs->IoLseek)
		return -1;

	return lflash_driver->funcs->IoLseek(arg, ofs, whence);
}

u8 data_5803[96] = 
{
	0x02, 0x00, 0x08, 0x00, 0x08, 0x00, 0x07, 0x9F, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x21, 0x21, 0x00, 0x00, 0x20, 0x01, 0x08, 0x00, 0x02, 0x00, 0x02, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static int New_IoIoctl(PspIoDrvFileArg *arg, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
	if (cmd == 0x02125008)
	{
		u32 *x = (u32 *)outdata;
		*x = 1; /* Enable writing */
		return 0;
	}
	else if (cmd == 0x02125803)
	{
		memcpy(outdata, data_5803, 96);
		return 0;
	}

	return -1;
}

static int New_IoDevctl(PspIoDrvFileArg *arg, const char *devname, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
	if (cmd == 0x02125801)
	{
		u8 *data8 = (u8 *)outdata;

		data8[0] = 1;
		data8[1] = 0;
		data8[2] = 0,
		data8[3] = 1;
		data8[4] = 0;
				
		return 0;
	}

	return -1;
}

void disableUsb(void) 
{ 
	if(usbStatus) 
	{
		sceUsbDeactivate(0);
		sceUsbStop(PSP_USBSTOR_DRIVERNAME, 0, 0);
		sceUsbStop(PSP_USBBUS_DRIVERNAME, 0, 0);

		msstor_driver->funcs->IoOpen = Orig_IoOpen;
		msstor_driver->funcs->IoClose = Orig_IoClose;
		msstor_driver->funcs->IoRead = Orig_IoRead;
		msstor_driver->funcs->IoWrite = Orig_IoWrite;
		msstor_driver->funcs->IoLseek = Orig_IoLseek;
		msstor_driver->funcs->IoIoctl = Orig_IoIoctl;
		msstor_driver->funcs->IoDevctl = Orig_IoDevctl;

		usbStatus = 0;
	}
}

void enableUsb(int device) 
{
	if (usbStatus)
	{
		disableUsb();
		sceKernelDelayThread(300000);
	}

	if(!usbModuleStatus) 
	{
		pspSdkLoadStartModule("flash0:/kd/semawm.prx", PSP_MEMORY_PARTITION_KERNEL);
		pspSdkLoadStartModule("flash0:/kd/usbstor.prx", PSP_MEMORY_PARTITION_KERNEL);
		pspSdkLoadStartModule("flash0:/kd/usbstormgr.prx", PSP_MEMORY_PARTITION_KERNEL);
		pspSdkLoadStartModule("flash0:/kd/usbstorms.prx", PSP_MEMORY_PARTITION_KERNEL);
		pspSdkLoadStartModule("flash0:/kd/usbstorboot.prx", PSP_MEMORY_PARTITION_KERNEL);
		
		lflash_driver = FindDriver("lflash");
		msstor_driver = FindDriver("msstor");
		
		Orig_IoOpen = msstor_driver->funcs->IoOpen;
		Orig_IoClose = msstor_driver->funcs->IoClose;
		Orig_IoRead = msstor_driver->funcs->IoRead;
		Orig_IoWrite = msstor_driver->funcs->IoWrite;
		Orig_IoLseek = msstor_driver->funcs->IoLseek;
		Orig_IoIoctl = msstor_driver->funcs->IoIoctl;
		Orig_IoDevctl = msstor_driver->funcs->IoDevctl;

		usbModuleStatus = 1;
	}

	if (device != 0)
	{
		unit = device-1;

		msstor_driver->funcs->IoOpen = New_IoOpen;
		msstor_driver->funcs->IoClose = New_IoClose;
		msstor_driver->funcs->IoRead = New_IoRead;
		msstor_driver->funcs->IoWrite = New_IoWrite;
		msstor_driver->funcs->IoLseek = New_IoLseek;
		msstor_driver->funcs->IoIoctl = New_IoIoctl;
		msstor_driver->funcs->IoDevctl = New_IoDevctl;
	}

	sceUsbStart(PSP_USBBUS_DRIVERNAME, 0, 0);
	sceUsbStart(PSP_USBSTOR_DRIVERNAME, 0, 0);
	sceUsbstorBootSetCapacity(0x800000);
	sceUsbActivate(0x1c8);
	usbStatus = 1;
}

#define PROGRAM "ms0:/PSP/GAME/RECOVERY/EBOOT.PBP"

char  hide[64], game[64], bootprog[64], noumd[64], region[64];
char patch[128], bootbin[128], isofs[128];
char plugins[11][64];
char vshspeed[64], umdspeed[64];
char freereg[64];
char *plugins_p[16];
u8	plugcon[15];
char button[50];
int nvsh = 0, ngame = 0, npops = 0;

char *regions[12] =
{
	"Disabled",
	"Japan",
	"America",
	"Europe",
	"",
	"",
	"",
	"Australia/New Zealand",
	"",
	"",
	"Russia",
	""
};

int main(void) {
	myDebugScreenInit();
	char *items[] = { "Toggle USB", "Configuration -> ", "Run program at /PSP/GAME/RECOVERY/EBOOT.PBP", "Advanced -> ", "CPU Speed ->", "Plugins ->", "Registry hacks ->", "Exit", 0 };
	char *mainmsg = "Main menu";
	char *advitems[] = { "Back", "Advanced configuration -> ", "Toggle USB (flash0)", "Toggle USB (flash1)" };
	char *advmsg = "Advanced";
	char *conitems[] = { "Back", "", "", "", "", "", "", "" };
	char *conmsg = "Configuration";
	char *adconitems[] = { "Back", "", "", "" };
	char *speeditems[] = { "Back", "", "" };
	char *regitems[] = { "Back", "", "Activate WMA", "Activate Flash Player" };
	int p = 1, u, o;
	int oldselection;

	//ReassignFlash0();
	//myDebugScreenSetBackColor(RGB(164, 0, 164));
	sceIoUnassign("flash0:");
	sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDWR, NULL, 0);

	myDebugScreenSetBackColor(RGB(0, 138, 138));
	while(p) 
	{
		clearScreen();
		int result;
		result = doMenu(items, 8, 0, mainmsg, 0, 7);
		if(result == 0) {
			if(!usbStatus) {
				printf(" > USB enabled"); enableUsb(0); delay(1000000);
			} else {
				printf(" > USB disabled"); disableUsb(); delay(1000000);
			}
		}
		else if(result == 3) 
		{
			u = 1;
			
			while(u) 
			{
				clearScreen();
				result = doMenu(advitems, 4, 0, advmsg, 2, 7);
				
				if(result == 0) 
				{ 
					printf(" > Back..."); 
					disableUsb();
					delay(1000000); 
					u = 0; 
				}

				else if (result == 3)
				{
					if(!usbStatus) 
					{
						printf(" > USB enabled"); enableUsb(2); delay(1000000);
					} else {
						printf(" > USB disabled"); disableUsb(); delay(1000000);
					}
				}

				else if (result == 2)
				{
					if(!usbStatus) 
					{
						printf(" > USB enabled"); enableUsb(1); delay(1000000);
					} else {
						printf(" > USB disabled"); disableUsb(); delay(1000000);
					}
				}

				else if (result == 1)
				{
					while (1)
					{
						clearScreen();
						
						if (!configchanged)
						{
							SE_GetConfig(&config);
						}

						sprintf(patch, "Plain modules in UMD/ISO (currently: %s)", config.umdactivatedplaincheck ? "Enabled" : "Disabled");
						sprintf(bootbin, "Execute BOOT.BIN in UMD/ISO (currently: %s)", config.executebootbin ? "Enabled" : "Disabled");
						sprintf(isofs, "Use isofs driver also in UMD-inserted mode (currently: %s)", config.useisofsonumdinserted ? "Enabled" : "Disabled");

						adconitems[1] = patch;
						adconitems[2] = bootbin;
						adconitems[3] = isofs;

						result = doMenu(adconitems, 4, 0, "Advanced configuration", 2, 7);

						if (result != 0 && result != -1)
						{
							if (!configchanged)
							{
								configchanged = 1;
							}						
						}

						if(result == 0) { printf(" > Back..."); delay(1000000); break; }

						if (result == 1)
						{
							config.umdactivatedplaincheck = !config.umdactivatedplaincheck;
							printf(" > Plain modules in UMD/ISO: %s", config.umdactivatedplaincheck ? "Enabled" : "Disabled");
							delay(1100000);
						}
						else if (result == 2)
						{
							
							config.executebootbin = !config.executebootbin;
							printf(" > Execute BOOT.BIN in UMD/ISO: %s", config.executebootbin ? "Enabled" : "Disabled");
							delay(1100000);
						}
						else if (result == 3)
						{
							config.useisofsonumdinserted = !config.useisofsonumdinserted;
							printf(" > Use isofs driver also in UMD-inserted mode: %s", config.useisofsonumdinserted ? "Enabled" : "Disabled");
							delay(1100000);
						}

						scePowerTick(0);
					}
				}

				scePowerTick(0);
			}
		}
		else if(result == 1) 
		{
			o = 1;
			oldselection = 0;

			while(o) 
			{
				char skip[96];				

				clearScreen();
				if (!configchanged)
				{
					SE_GetConfig(&config);
				}

				sprintf(skip, "Skip SCE logo (currently: %s)", config.skiplogo ? "Enabled" : "Disabled");
				sprintf(hide, "Hide corrupt icons (currently: %s)", config.hidecorrupt ? "Enabled" : "Disabled");
				sprintf(game, "Game folder homebrew (currently: %s)", config.gamekernel150 ? "1.50 Kernel" : "3.40 Kernel");
				sprintf(bootprog, "Autorun program at /PSP/GAME/BOOT/EBOOT.PBP (currently: %s)", config.startupprog ? "Enabled" : "Disabled");
				sprintf(noumd, "Use NO-UMD (currently: %s)", config.usenoumd ? "Enabled" : "Disabled");
				
				if (config.fakeregion > 10)
					config.fakeregion = 0;
				
				sprintf(region, "Fake region (currently: %s)", regions[config.fakeregion]);
				sprintf(freereg, "Free UMD Region (currently: %s)", config.freeumdregion ? "Enabled" : "Disabled");

				conitems[1] = skip;
				conitems[2] = hide;
				conitems[3] = game;
				conitems[4] = bootprog;
				conitems[5] = noumd;
				conitems[6] = region;
				conitems[7] = freereg;

				result = doMenu(conitems, 8, oldselection, conmsg, 2, 7);
				
				if (result != 0 && result != -1)
				{
					if (!configchanged)
					{
						configchanged = 1;
					}
				}

				if(result == 0) { printf(" > Back..."); delay(1000000); o = 0; }
				else if(result == 1) 
				{ 
					config.skiplogo = !config.skiplogo;
					printf(" > Skip SCE logo: %s", (config.skiplogo) ? "Enabled" : "Disabled");
					delay(1100000); 
				}
				else if(result == 2) 
				{ 
					config.hidecorrupt = !config.hidecorrupt;
					printf(" > Hide corrupt icons: %s", (config.hidecorrupt) ? "Enabled" : "Disabled"); 
					delay(1100000); 
				}
				else if (result == 3)
				{
					config.gamekernel150 = !config.gamekernel150;
					printf(" > Game folder homebrew: %s", (config.gamekernel150) ? "1.50 Kernel" : "3.40 Kernel"); 
					delay(1100000); 
				}
				else if (result == 4)
				{
					config.startupprog = !config.startupprog;
					printf(" > Autorun program at /PSP/GAME/BOOT/EBOOT.PBP: %s", (config.startupprog) ? "Enabled" : "Disabled"); 
					
					u8 *buffer;
					int size;
					SceUID fd; 

					sceKernelVolatileMemLock(0, &buffer, &size);
					fd = sceIoOpen("flash0:/kd/rtc.prx", PSP_O_RDONLY, 0777);
					size = sceIoRead(fd, buffer, size);
					sceIoClose(fd);

					if (config.startupprog)
					{
						_sw(0x10000003, buffer+0x60+0x714);
					}
					else
					{
						_sw(0, buffer+0x60+0x714);
					}

					fd = sceIoOpen("flash0:/kd/rtc.prx", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
					sceIoWrite(fd, buffer, size);
					sceIoClose(fd);

					sceKernelVolatileMemUnlock();
					
					delay(1100000); 
				}
				else if (result == 5)
				{
					config.usenoumd = !config.usenoumd;
					printf(" > Use NO-UMD: %s", (config.usenoumd) ? "Enabled" : "Disabled");
					delay(1100000);
				}
				else if (result == 7)
				{
					config.freeumdregion = !config.freeumdregion;
					printf(" > Free UMD Region: %s", (config.freeumdregion) ? "Enabled" : "Disabled");
					delay(1100000);
				}

				if (result == 6)
				{
					config.fakeregion++;

					if (config.fakeregion == FAKE_REGION_KOREA)
						config.fakeregion = FAKE_REGION_AUSTRALIA;
					else if (config.fakeregion == FAKE_REGION_HONGKONG)
						config.fakeregion = FAKE_REGION_RUSSIA;
					else if (config.fakeregion == FAKE_REGION_CHINA)
						config.fakeregion = FAKE_REGION_DISABLED;

					oldselection = 6;

					delay(300000);
				}
				else if (result != -1)
				{
					oldselection = 0;
				}

				scePowerTick(0);
			}
		}

		else if (result == 2)
		{
			struct SceKernelLoadExecVSHParam param;

			pspSdkLoadStartModule("flash0:/kd/systemctrl.prx", PSP_MEMORY_PARTITION_KERNEL);

			memset(&param, 0, sizeof(param));
			
			param.size = sizeof(param);
			param.args = strlen(PROGRAM)+1;
			param.argp = PROGRAM;
			param.key = "updater";

			sceKernelLoadExecVSHMs1(PROGRAM, &param);
		}

		else if (result == 4)
		{
			oldselection = 0;
			
			while (1)
			{			
				clearScreen();

				/*doMenu(speeditems, 1, 0, "CPU Speed", 2, 7);

				delay(1000000); 
				break; */

				if (!configchanged)
				{
					SE_GetConfig(&config);
				}

				if (config.vshcpuspeed != 333 && config.vshcpuspeed != 266
					&& config.vshcpuspeed != 222 && config.vshcpuspeed != 300 
					&& config.vshcpuspeed != 0)
				{
					config.vshcpuspeed = 0;
					configchanged = 1;
				}

				if (config.vshbusspeed != 166 && config.vshbusspeed != 133 && 
					config.vshbusspeed != 111 && config.vshbusspeed != 150)
				{
					config.vshbusspeed = 0;
					configchanged = 1;
				}

				if (config.umdisocpuspeed != 333 && config.umdisocpuspeed != 266
					&& config.umdisocpuspeed != 222 && config.umdisocpuspeed != 300 
					&& config.umdisocpuspeed != 0)
				{
					config.umdisocpuspeed = 0;
					configchanged = 1;
				}

				if (config.umdisobusspeed != 166 && config.umdisobusspeed != 133 && 
					config.umdisobusspeed != 111 && config.umdisobusspeed != 150)
				{
					config.umdisobusspeed = 0;
					configchanged = 1;
				}

				sprintf(vshspeed, "Speed in XMB (currently: %d)", config.vshcpuspeed);
				sprintf(umdspeed, "Speed in UMD/ISO (currently: %d)", config.umdisocpuspeed);
				
				if (config.vshcpuspeed == 0)
				{
					strcpy(vshspeed+strlen(vshspeed)-2, "Default)");
				}
				
				if (config.umdisocpuspeed == 0)
				{
					strcpy(umdspeed+strlen(umdspeed)-2, "Default)");
				}

				speeditems[1] = vshspeed;
				speeditems[2] = umdspeed;				

				result = doMenu(speeditems, 3, oldselection, "CPU Speed", 2, 7);

				if (result != 0)
				{
					if (!configchanged)
						configchanged = 1;
				}

				if(result == 0)
				{
					printf(" > Back..."); 
					delay(1000000); 
					break; 
				}

				else if (result == 1)
				{
					if (config.vshcpuspeed == 0)
					{
						config.vshcpuspeed = 222;
						config.vshbusspeed = 111;
					}

					else if (config.vshcpuspeed  == 222)
					{
						config.vshcpuspeed = 266;
						config.vshbusspeed = 133;
					}
					else if (config.vshcpuspeed == 266)
					{
						config.vshcpuspeed = 300;
						config.vshbusspeed = 150;
					}
					else if (config.vshcpuspeed  == 300)
					{
						config.vshcpuspeed = 333;
						config.vshbusspeed = 166;
					}
					else if (config.vshcpuspeed  == 333)
					{
						config.vshcpuspeed = 0;
						config.vshbusspeed = 0;
					}

					sprintf(vshspeed, "Speed in XMB: %d", config.vshcpuspeed);
					if (config.vshcpuspeed == 0)
					{
						strcpy(vshspeed+strlen(vshspeed)-1, "Default");
					}
					printf("%s", vshspeed);
					oldselection = 1;
					delay(1100000);
				}
				
				else if (result == 2)
				{
					if (config.umdisocpuspeed == 0)
					{
						config.umdisocpuspeed = 222;
						config.umdisobusspeed = 111;
					}

					else if (config.umdisocpuspeed  == 222)
					{
						config.umdisocpuspeed = 266;
						config.umdisobusspeed = 133;
					}
					else if (config.umdisocpuspeed == 266)
					{
						config.umdisocpuspeed = 300;
						config.umdisobusspeed = 150;
					}
					else if (config.umdisocpuspeed  == 300)
					{
						config.umdisocpuspeed = 333;
						config.umdisobusspeed = 166;
					}
					else if (config.umdisocpuspeed  == 333)
					{
						config.umdisocpuspeed = 0;
						config.umdisobusspeed = 0;
					}

					sprintf(umdspeed, "Speed in UMD/ISO: %d", config.umdisocpuspeed);
					if (config.umdisocpuspeed == 0)
					{
						strcpy(umdspeed+strlen(umdspeed)-1, "Default");
					}
					printf("%s", umdspeed);
					oldselection = 2;
					delay(1100000);
				}

				scePowerTick(0);
			}

		}

		if (result == 5)
		{
			
			while (1)
			{
				clearScreen();

				SceUID fd; 
				int i;
				char *p;

				ngame = 0;
				nvsh = 0;
				npops = 0;
			
				memset(plugcon, 0, 15);

				fd = sceIoOpen("ms0:/seplugins/conf.bin", PSP_O_RDONLY, 0777);
				sceIoRead(fd, plugcon, 15);
				sceIoClose(fd);

				memset(plugins, 0, sizeof(plugins));

				fd= sceIoOpen("ms0:/seplugins/vsh.txt", PSP_O_RDONLY, 0777);
				if (fd >= 0)
				{
					for (i = 0; i < 5; i++)
					{
						if (ReadLine(fd, plugins[i+1]) > 0)
						{
							p = strrchr(plugins[i+1], '/');
							if (p)
							{
								strcpy(plugins[i+1], p+1);
							}
						
							strcat(plugins[i+1], " [VSH]");							
							nvsh++;	
						}
						else
						{
							break;
						}
					}
				
					sceIoClose(fd);
				}

				fd = sceIoOpen("ms0:/seplugins/game.txt", PSP_O_RDONLY, 0777);

				if (fd >= 0)
				{
					for (i = 0; i < 5; i++)
					{
						if (ReadLine(fd, plugins[i+nvsh+1]) > 0)
						{
							p = strrchr(plugins[i+nvsh+1], '/');
							if (p)
							{
								strcpy(plugins[i+nvsh+1], p+1);
							}
						
							strcat(plugins[i+nvsh+1], " [GAME]");
							ngame++;	
						}
						else
						{
							break;
						}
					}

					sceIoClose(fd);
				}
				
				fd = sceIoOpen("ms0:/seplugins/pops.txt", PSP_O_RDONLY, 0777);

				if (fd >= 0)
				{
					for (i = 0; i < 5; i++)
					{
						if (ReadLine(fd, plugins[i+nvsh+ngame+1]) > 0)
						{
							p = strrchr(plugins[i+nvsh+ngame+1], '/');
							if (p)
							{
								strcpy(plugins[i+nvsh+ngame+1], p+1);
							}
						
							strcat(plugins[i+nvsh+ngame+1], " [POPS]");
							npops++;	
						}
						else
						{
							break;
						}
					}

					sceIoClose(fd);
				}

				strcpy(plugins[0], "Back");

				for (i = 0; i < (ngame+nvsh+npops+1); i++)
				{
					if (i != 0)
						strcat(plugins[i], (plugcon[i-1]) ? " (Enabled) " : " (Disabled)");
				
					plugins_p[i] = plugins[i];
				}

				result = doMenu(plugins_p, ngame+nvsh+npops+1, 0, "Plugins", 2, 7);

				if(result == 0) { printf(" > Back..."); delay(1000000); break; }
				else if (result != -1)
				{
					char str[256];

					strcpy(str, plugins_p[result]);
					str[strlen(str)-11] = 0;
					
					plugcon[result-1] = !plugcon[result-1];

					if (plugcon[result-1])
						strcat(str, ": Enabled");
					else
						strcat(str, ": Disabled");

					printf(str);
					SceUID fd = sceIoOpen("ms0:/seplugins/conf.bin", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

					sceIoWrite(fd, plugcon, sizeof(plugcon));
					sceIoClose(fd);
					delay(900000);
				}

				scePowerTick(0);

			}
		}

		else if (result == 6)
		{
			while (1)
			{
				unsigned int value = 0;

				clearScreen();

				strcpy(button, "Button assign (currently ");

				get_registry_value("/CONFIG/SYSTEM/XMB", "button_assign", &value); 

				if (value == 0)
				{
					strcat(button, "O is enter)");
				}
				else
				{
					strcat(button, "X is enter)");
				}

				regitems[1] = button;

				result = doMenu(regitems, 4, 0, "Registry Hacks", 2, 7);

				if(result == 0) 
				{ 
					printf(" > Back..."); 
					delay(800000); 
					break;
				}
				else if (result == 1)
				{
					value = !value;
					set_registry_value("/CONFIG/SYSTEM/XMB", "button_assign", value); 
					
					strcpy(button, " > Button assign: ");
					if (value == 0)
					{
						strcat(button, "O is enter");
					}
					else
					{
						strcat(button, "X is enter");
					}

					printf("%s", button);
					
					delay(1000000);
				}
				else if (result == 2)
				{
					get_registry_value("/CONFIG/MUSIC", "wma_play", &value);
					
					if (value == 1)
					{
						printf("WMA was already activated.");
					}
					else
					{
						printf("Activating WMA...\n");
						set_registry_value("/CONFIG/MUSIC", "wma_play", 1);
					}

					delay(1000000);
				}
				else if (result == 3)
				{
					get_registry_value("/CONFIG/BROWSER", "flash_activated", &value);

					if (value == 1)
					{
						printf("Flash player was already activated.\n");
					}
					else
					{
						printf("Activating Flash Player...");
						set_registry_value("/CONFIG/BROWSER", "flash_activated", 1);
						set_registry_value("/CONFIG/BROWSER", "flash_play", 1);
					}

					delay(1000000);
				}

				scePowerTick(0);
			}
		}

		else if(result == 7) 
		{ 
			printf(" > Exiting recovery"); 
			delay(700000); 
			
			if (configchanged) 
				SE_SetConfig(&config); 			
			
			sceKernelExitVSHVSH(NULL); 
		}

		scePowerTick(0);
	}
	return 0;
}

