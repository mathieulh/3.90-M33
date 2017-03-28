#include <pspsdk.h>
#include <pspkernel.h>
#include <systemctrl.h>
#include <systemctrl_se.h>
#include <systemctrl_internal.h>


#include <stdio.h>
#include <string.h>
#include <oe_malloc.h>

#include <main.h>

static int plugindone = 0;
extern SEConfig config;
InitControl initcontrol;

int LoadStartModule(char *module)
{
	SceUID mod = sceKernelLoadModule(module, 0, NULL);	
	if (mod < 0)
	{
		return mod;
	}

	return sceKernelStartModule(mod, strlen(module)+1, module, NULL, NULL);
}

int ReadLine(SceUID fd, char *str)
{
	char ch = 0;
	int n = 0;

	while (1)
	{	
		if (sceIoRead(fd, &ch, 1) != 1)
			break;

		if (ch < 0x20)
		{
			if (n != 0)
				break;
		}
		else
		{
			*str++ = ch;
			n++;
		}
	}

	return n;
}

int sceKernelStartModulePatched(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option)
{
	SceModule2 *mod = sceKernelFindModuleByUID(modid);
	u32 *vshmain_args = NULL;

	if (mod != NULL)
	{		
		//Kprintf(mod->modname);
		if (strcmp(mod->modname, "vsh_module") == 0)
		{
			if (sceKernelInitMode() == PSP_INIT_MODE_VSH)
			{
				if (argsize == 0)
				{
					if (config.skiplogo)
					{
						vshmain_args = (u32 *)oe_malloc(0x0400);
						
						memset(vshmain_args, 0, 0x0400);
						vshmain_args[0/4] = 0x0400;
						vshmain_args[4/4] = 0x20;
						vshmain_args[0x40/4] = 1;
						vshmain_args[0x280/4] = 1;
						vshmain_args[0x284/4] = 0x40003;

						argsize = 0x0400;
						argp = vshmain_args;						
					}
				}				
			}			
		}
		
		else if (!plugindone)
		{
			char *waitmodule;
			
			if (sceKernelDiscImage() != 0)
			{
				waitmodule = "sceMeCodecWrapper";
			}	
			else
			{
				waitmodule = "sceMediaSync";
			}

			if (sceKernelFindModuleByName(waitmodule))
			{			
				u8 plugcon[15];
				char plugin[64];
				int	nvsh = 0, ngame = 0;
				int initmode, i;
				SceUID fd;

				memset(plugcon, 0, 15);	
				plugindone = 1;
				initmode = sceKernelInitMode();			

				for (i = 0; i < 0x10; i++)
				{
					fd = sceIoOpen("ms0:/seplugins/conf.bin", PSP_O_RDONLY, 0);
				
					if (fd >= 0)
					{
						break;	
					}

					sceKernelDelayThread(20000);
				}

				if (fd >= 0)
				{
					sceIoRead(fd, plugcon, 15);
					sceIoClose(fd);
				}			

				fd = sceIoOpen("ms0:/seplugins/vsh.txt", PSP_O_RDONLY, 0);
				if (fd >= 0)
				{
					for (i = 0; i < 5; i++)
					{
						memset(plugin, 0, sizeof(plugin));
						if (ReadLine(fd, plugin) > 0)
						{
							nvsh++;
							if (initmode == PSP_INIT_MODE_VSH && sceKernelFindModuleByName("scePspNpDrm_Driver"))
							{
								if (plugcon[i])
								{
									LoadStartModule(plugin);									
								}
							}
						}
						else
						{
							break;
						}
					}

					sceIoClose(fd);
				}

				fd= sceIoOpen("ms0:/seplugins/game.txt", PSP_O_RDONLY, 0);
				if (fd >= 0)
				{
					for (i = 0; i < 5; i++)
					{
						memset(plugin, 0, sizeof(plugin));
						if (ReadLine(fd, plugin) > 0)
						{
							ngame++;
							if (initmode == PSP_INIT_MODE_GAME)
							{
								if (plugcon[i+nvsh])
								{
									LoadStartModule(plugin);									
								}
							}
						}
						else
						{
							break;
						}
					}

					sceIoClose(fd);
				}

				if (initmode == PSP_INIT_MODE_POPS)
				{
					fd = sceIoOpen("ms0:/seplugins/pops.txt", PSP_O_RDONLY, 0);
					if (fd >= 0)
					{
						for (i = 0; i < 5; i++)
						{
							memset(plugin, 0, sizeof(plugin));

							if (ReadLine(fd, plugin) > 0)
							{							
								if (plugcon[i+nvsh+ngame])
								{
									LoadStartModule(plugin);								
								}
							}
							else
							{
								break;
							}
						}
					
						sceIoClose(fd);	
					}				
				}
			}
		}
	}

	int res = sceKernelStartModule(modid, argsize, argp, status, option);	
	
	if (vshmain_args)
	{
		oe_free(vshmain_args);		
	}	

	return res;
}

int sctrlKernelSetInitApitype(int apitype)
{
	int k1 = pspSdkSetK1(0);
	int prev = sceKernelInitApitype();

	*initcontrol.apitype = apitype;	

	pspSdkSetK1(k1);
	return prev;
}

int sctrlKernelSetInitFileName(char *filename)
{
	int k1 = pspSdkSetK1(0);

	*initcontrol.filename = filename;

	pspSdkSetK1(k1);
	return 0;
}

int sctrlKernelSetInitMode(int mode)
{
	int k1 = pspSdkSetK1(0);
	int prev = sceKernelInitMode();

	*initcontrol.mode = mode;

	pspSdkSetK1(k1);
	return prev;
}

int sctrlHENRegisterHomebrewLoader(void *func)
{
	MAKE_JUMP(initcontrol.text_addr+0x1AE0, (u32)func);	
	ClearCaches();

	return 0;
}

InitControl *sctrlHENGetInitControl()
{
	return &initcontrol;
}

void sctrlHENTakeInitControl(int (* ictrl)(InitControl *))
{
	u16 high, low;
	u32 addr;

	addr = initcontrol.text_addr+0xC90;
	high = addr >> 16;
	low = addr & 0xFFFF;

	// lui ra, high
	_sw(0x3c1f0000 | high, initcontrol.text_addr+0xC08);
	// ori ra, ra, low
	_sw(0x37ff0000 | low, initcontrol.text_addr+0xC0C);

	high = ((u32)&initcontrol) >> 16;
	low  = ((u32)&initcontrol) & 0xFFFF;

	// lui a0, high
	_sw(0x3c040000 | high, initcontrol.text_addr+0xC10);
	MAKE_JUMP(initcontrol.text_addr+0xC14, ictrl);
	// ori a0, a0, low
	_sw(0x34840000 | low, initcontrol.text_addr+0xC18);

	initcontrol.bootinfo->nextmodule++;

	ClearCaches();
}

int PatchInit(int (* module_bootstart)(SceSize, BootInfo *), BootInfo *bootinfo)
{
	u32 text_addr = ((u32)module_bootstart)-0xA8;

	memset(&initcontrol, 0, sizeof(InitControl));

	initcontrol.text_addr = text_addr;
	initcontrol.bootinfo = bootinfo;
	initcontrol.apitype = (int *)(text_addr+0x22B0);
	initcontrol.filename = (char **)(text_addr+0x22D4);
	initcontrol.mode = (int *)(text_addr+0x2424);
	initcontrol.powerlock_count = (int *)(text_addr+0x2428);
	initcontrol.FreeUnkParamAndModuleInfo = (void *)(text_addr+0x1524);
	initcontrol.FreeParamsAndBootInfo = (void *)(text_addr+0x1624);
	initcontrol.ProcessCallbacks = (void *)(text_addr+0x13AC);
	
	MAKE_JUMP(text_addr+0x1AB8, sceKernelStartModulePatched);
	ClearCaches();
	
	return module_bootstart(4, bootinfo);
	
}

