#include <pspsdk.h>
#include <pspkernel.h>
#include <pspinit.h>
#include <pspcrypt.h>
#include <psploadexec_kernel.h>
#include <psputilsforkernel.h>
#include <psppower.h>
#include <pspreg.h>
#include <pspmediaman.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "sysmodpatches.h"
#include "main.h"
#include "reboot.h"
#include <umd9660_driver.h>
#include <isofs_driver.h>
//#include "virtualpbpmgr.h"
#include "systemctrl_se.h"
#include "malloc.h"

SEConfig config;
u32 vshmain_args[0x0400/4];

void SetConfig(SEConfig *newconfig)
{
	memcpy(&config, newconfig, sizeof(SEConfig));
}

/*SEConfig *GetConfig()
{
	return &config;
}*/

int LoadStartModule(char *module)
{
	SceUID mod = sceKernelLoadModule(module, 0, NULL);

	if (mod < 0)
		return mod;

	return sceKernelStartModule(mod, strlen(module)+1, module, NULL, NULL);
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

u32 FindPowerFunction(u32 nid)
{
	return FindProc("scePower_Service", "scePower", nid);
}

u32 FindPowerDriverFunction(u32 nid)
{
	return FindProc("scePower_Service", "scePower_driver", nid);
}

int (* scePowerSetCpuClockFrequency_k)(int cpufreq);
int (* scePowerSetBusClockFrequency_k)(int busfreq);
int (* scePowerSetClockFrequency_k)(int cpufreq, int ramfreq, int busfreq);

int (* MemlmdDecrypt)(u8 *buf, int size, int *ret, int u);

/* Decryption stuff, to avoid the odd error of 3.10 kernel modules not decrypting
   after recovering from sleep */

u8 keys_310k[0x10] = 
{
	0xa2, 0x41, 0xe8, 0x39, 0x66, 0x5b, 0xfa, 0xbb,
	0x1b, 0x2d, 0x6e, 0x0e, 0x33, 0xe5, 0xd7, 0x3f
};

int Scramble(u32 *buf, u32 size, u32 code)
{
	buf[0] = 5;
	buf[1] = buf[2] = 0;
	buf[3] = code;
	buf[4] = size;

	if (semaphore_4C537C72(buf, size+0x14, buf, size+0x14, 7) < 0)
	{
		return -1;
	}

	return 0;
}

int DecryptPRX(u8 *buf, int size, int *ret)
{
	u8	tmp1[0x150], tmp2[0x90+0x14], tmp3[0x60+0x14];
	*ret = *(int *)&buf[0xB0];
	
	memset(tmp1, 0, 0x150);
	memset(tmp2, 0, 0x90+0x14);
	memset(tmp3, 0, 0x60+0x14);

	if (*((u32 *)buf) != 0x5053507E) // "~PSP"
	{
		//printf("Error: unknown signature.\n");
		return -1;
	}

	if (size < 0x160) 
	{
		//printf("Buffer not big enough.\n");
		return -1;
	}

	if (((u32)buf & 0x3F)) 
	{
		//printf("Buffer not aligned to 64 bytes.\n");
		return -1;
	}

	if ((size - 0x150) < *ret)
	{
		//printf("No enough data.\n");
		return -1;
	}

	memcpy(tmp1, buf, 0x150);

	int i, j;
	u8 *p = tmp2+0x14;

	for (i = 0; i < 9; i++)
	{
		for (j = 0; j < 0x10; j++)
		{
			p[(i << 4) + j] = keys_310k[j]; 			
		}

		p[(i << 4)] = i;
	}	

	if (Scramble((u32 *)tmp2, 0x90, 0x62) < 0)
	{
		//printf("Error in Scramble #1.\n");
		return -1;
	}

	memcpy(buf, tmp1+0xD0, 0x5C);
	memcpy(buf+0x5C, tmp1+0x140, 0x10);
	memcpy(buf+0x6C, tmp1+0x12C, 0x14);
	memcpy(buf+0x80, tmp1+0x080, 0x30);
	memcpy(buf+0xB0, tmp1+0x0C0, 0x10);
	memcpy(buf+0xC0, tmp1+0x0B0, 0x10);
	memcpy(buf+0xD0, tmp1+0x000, 0x80);

	memcpy(tmp3+0x14, buf+0x5C, 0x60);	

	if (Scramble((u32 *)tmp3, 0x60, 0x62) < 0)
	{
		//printf("Error in Scramble #2.\n");
		return -1;
	}

	memcpy(buf+0x5C, tmp3, 0x60);
	memcpy(tmp3, buf+0x6C, 0x14);
	memcpy(buf+0x70, buf+0x5C, 0x10);
	memset(buf+0x18, 0, 0x58);
	memcpy(buf+0x04, buf, 0x04);

	*((u32 *)buf) = 0x014C;
	memcpy(buf+0x08, tmp2, 0x10);	

	// sha-1 
	if (semaphore_4C537C72(buf, 3000000, buf, 3000000, 0x0B) < 0)
	{
		//printf("Error in semaphore2  #1.\n");
		return -1;
	}	

	if (memcmp(buf, tmp3, 0x14) != 0)
	{
		//printf("SHA-1 incorrect.\n");
		return -1;
	}

	int iXOR;

	for (iXOR = 0; iXOR < 0x40; iXOR++)
	{
		tmp3[iXOR+0x14] = buf[iXOR+0x80] ^ tmp2[iXOR+0x10];
	}

	if (Scramble((u32 *)tmp3, 0x40, 0x62) != 0)
	{
		//printf("Error in Scramble #2.\n");
		return -1;
	}
	
	for (iXOR = 0x3F; iXOR >= 0; iXOR--)
	{
		buf[iXOR+0x40] = tmp3[iXOR] ^ tmp2[iXOR+0x50]; // uns 8
	}

	memset(buf+0x80, 0, 0x30);
	*(u32 *)&buf[0xA0] = 1;

	memcpy(buf+0xB0, buf+0xC0, 0x10);
	memset(buf+0xC0, 0, 0x10);

	// The real decryption
	if (semaphore_4C537C72(buf, size, buf+0x40, size-0x40, 0x1) < 0)
	{
		//printf("Error in semaphore2  #2.\n");
		return -1;
	}

	if (*ret < 0x150)
	{
		// Fill with 0
		memset(buf+*ret, 0, 0x150-*ret);		
	}

	return 0;
}

int MemlmdDecryptPatched(u8 *buf, int size, int *ret, int u)
{
	if (buf && ret)
	{	
		u32 tag = *(u32 *)&buf[0xD0];

		//printf("tag %08X\n", tag);

		if (tag == 0xcfef09f0)
		{
			return DecryptPRX(buf, size, ret);
		}

	}

	return MemlmdDecrypt(buf, size, ret, u);		
}

void PatchMemlmd()
{
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceMemlmd");
	u32 text_addr = *(mod+27);

	MAKE_CALL(text_addr+0xE2C, MemlmdDecryptPatched);	
	MemlmdDecrypt = (void *)(text_addr+0x134);
	ClearCaches();
}

int sceKernelStartModulePatched(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option)
{
	SceModule *mod = sceKernelFindModuleByUID(modid);
	if (mod != NULL)
	{		
		Kprintf(mod->modname);
		if (strcmp(mod->modname, "vsh_module") == 0)
		{
			if (sceKernelInitKeyConfig() == PSP_INIT_KEYCONFIG_VSH)
			{
				if (argsize == 0)
				{
					if (config.skiplogo)
					{
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

		else if (strcmp(mod->modname, "sceMediaSync") == 0)
		{
			u8 plugcon[15];
			char plugin[64];
			int	nvsh = 0, ngame = 0;
			int keyconfig, i;
			SceUID fd;

			keyconfig = sceKernelInitKeyConfig();

			memset(plugcon, 0, 15);

			for (i = 0; i < 0x10; i++)
			{
				fd = sceIoOpen("ms0:/seplugins/conf.bin", PSP_O_RDONLY, 0777);
				
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

			fd = sceIoOpen("ms0:/seplugins/vsh.txt", PSP_O_RDONLY, 0777);
			if (fd >= 0)
			{
				for (i = 0; i < 5; i++)
				{
					memset(plugin, 0, sizeof(plugin));
					if (ReadLine(fd, plugin) > 0)
					{
						nvsh++;
						if (keyconfig == PSP_INIT_KEYCONFIG_VSH)
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

			fd= sceIoOpen("ms0:/seplugins/game.txt", PSP_O_RDONLY, 0777);
			if (fd >= 0)
			{
				for (i = 0; i < 5; i++)
				{
					memset(plugin, 0, sizeof(plugin));
					if (ReadLine(fd, plugin) > 0)
					{
						ngame++;
						if (keyconfig == PSP_INIT_KEYCONFIG_GAME)
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

			if (keyconfig == PSP_INIT_KEYCONFIG_POPS)
			{
				fd = sceIoOpen("ms0:/seplugins/pops.txt", PSP_O_RDONLY, 0777);
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

	return sceKernelStartModule(modid, argsize, argp, status, option);	
}

void PatchInit(u32 text_addr)
{
	// Patch StartModule 
	MAKE_JUMP(text_addr+0x1684, sceKernelStartModulePatched);	
}

void PatchNandDriver(char *buf)
{
	int intr = sceKernelCpuSuspendIntr();
	u32 text_addr = (u32)buf+0xA0;

	_sh(0xac60, text_addr+0x0e4c);

	sceKernelCpuResumeIntr(intr);
	ClearCaches();

	if (mallocinit() < 0)
	{
		while (1) _sw(0, 0);
	}	
}

//////////////////////////////////////////////////////////////

void PatchUmdMan(char *buf)
{
	int intr = sceKernelCpuSuspendIntr();
	u32 text_addr = (u32)buf+0xA0;

	if (sceKernelBootFrom() == PSP_BOOT_MS)
	{
		// Replace call to sceKernelBootFrom with return PSP_BOOT_DISC
		_sw(0x24020020, text_addr+0x13F4);
	}

	sceKernelCpuResumeIntr(intr);
	ClearCaches();
}

//////////////////////////////////////////////////////////////

#define EBOOT_BIN "disc0:/PSP_GAME/SYSDIR/EBOOT.BIN"
#define BOOT_BIN  "disc0:/PSP_GAME/SYSDIR/BOOT.BIN"

int (* UtilsForKernel_7dd07271)(u8 *dest, u32 destSize, const u8 *src, void *unk, void *unk2);

//int reboot150 = 0;

int LoadRebootex(u8 *dest, u32 destSize, const u8 *src, void *unk, void *unk2)
{
	u8 *output = (u8 *)0x88fc0000;
	char *theiso;
	int i;

	theiso = GetUmdFile();
	if (theiso)
	{	
		strcpy((void *)0x88fb0000, theiso);
	}

	memcpy((void *)0x88fb0050, &config, sizeof(SEConfig));

	for (i = 0; i < (sizeof(rebootex)-0x10); i++)
	{
		output[i] = rebootex[i+0x10];		
	}

	return UtilsForKernel_7dd07271(dest, destSize, src, unk, unk2);	
}

int Reboot303_1()
{
	LoadStartModule("flash0:/kd/reboot303.prx");
	return sceKernelCheckExitCallback();
}

int Reboot303_2()
{
	LoadStartModule("flash0:/kd/reboot303.prx");
	return sceKernelIsIntrContext();
}

int Reboot303_3()
{
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceLoadExec");
	u32 text_addr = *(mod+27);
	int (* func)(void *, void *) = (void *)(text_addr+0x11D4);
	
	LoadStartModule("flash0:/kd/reboot303.prx");
	return sceKernelIsIntrContext();
}

void PatchLoadExec(u32 text_addr)
{
	_sw(0x3C0188fc, text_addr+0x16E4);
	MAKE_CALL(text_addr+0x16A8, LoadRebootex);
	UtilsForKernel_7dd07271 = (void *)(text_addr+0x1BCC);

	// Allow LoadExecVSH in whatever user level
	_sw(0x1000000b, text_addr+0x0F38);
	_sw(0, text_addr+0x0F78);

	// Allow ExitVSHVSH in whatever user level
	_sw(0x10000008, text_addr+0x0720);
	_sw(0, text_addr+0x0754);	

	MAKE_CALL(text_addr+0x063C, Reboot303_1);
	MAKE_CALL(text_addr+0x0704, Reboot303_2);
	MAKE_CALL(text_addr+0x0B40, Reboot303_3);
}

void PatchInitLoadExecAndMediaSync(char *buf)
{
	int intr = sceKernelCpuSuspendIntr();	
	
	char *filename = sceKernelInitFileName();
	u32 text_addr = (u32)buf+0xA0;

	if (filename)
	{
		if (strstr(filename, ".PBP"))
		{
			// Patch mediasync (avoid error 0x80120005 sfo error) 
			// Make return 0
			_sw(0x00008021, text_addr+0x5B8);
			ClearCaches();
			
			SetUmdFile("");
		}

		else if (strstr(filename, "disc") == filename)
		{
			if (!config.umdactivatedplaincheck)
				UndoSuperNoPlainModuleCheckPatch();

			char *theiso = GetUmdFile();

			if (theiso)
			{
				if (strncmp(theiso, "ms0:/", 5) == 0)
				{
					if (!config.usenoumd && !config.useisofsonumdinserted)
					{
						DoAnyUmd();
					}
					else
					{
						sceKernelCpuResumeIntr(intr);
						sceIoDelDrv("umd");
						sceIoAddDrv(getumd9660_driver());
						intr = sceKernelCpuSuspendIntr();
						//DoAnyUmd();
					}

					if (config.usenoumd)
					{
						// Patch the error of no disc
						_sw(0x34020000, text_addr+0xEC);
					}
					else
					{
						// Patch the error of diferent sfo?
						_sw(0x00008021, text_addr+0xF8);
					}
				}
				else
				{
					SetUmdFile("");
				}
			}
		}
		else
		{
			SetUmdFile("");
		}
	}
	else
	{
		SetUmdFile("");
	}

	u32 *mod = (u32 *)sceKernelFindModuleByName("sceInit");
	if (mod)
	{
		PatchInit(*(mod+27));
	}

	mod = (u32 *)sceKernelFindModuleByName("sceLoadExec");
	if (mod)
	{
		PatchLoadExec(*(mod+27));
	}	

	sceKernelCpuResumeIntr(intr);
	ClearCaches();	
	
	int (* sceClockgen_driver_5F8328FD) (void) = (void *)FindProc("sceClockgen_Driver", "sceClockgen_driver", 0x5F8328FD);
	
	sceClockgen_driver_5F8328FD();
}

//////////////////////////////////////////////////

void *block;
int drivestat = SCE_UMD_READY | SCE_UMD_MEDIA_IN;
SceUID umdcallback;

void UmdCallback()
{
	if (umdcallback >= 0)
	{
		sceKernelNotifyCallback(umdcallback, drivestat);
	}
}

int sceUmdActivatePatched(const int mode, const char *aliasname)
{
	int k1 = pspSdkSetK1(0);
	//sceIoAssign(aliasname, block, block+6, IOASSIGN_RDONLY, NULL, 0);
	sceIoAssign(aliasname, "umd0:", "isofs0:", IOASSIGN_RDONLY, NULL, 0);
	pspSdkSetK1(k1);	
	
	drivestat = SCE_UMD_READABLE | SCE_UMD_MEDIA_IN;	
	UmdCallback();
	return 0;
}

int sceUmdDeactivatePatched(const int mode, const char *aliasname)
{
	sceIoUnassign(aliasname);
	drivestat = SCE_UMD_MEDIA_IN | SCE_UMD_READY;
	
	UmdCallback();
	return 0;
}

int sceUmdGetDiscInfoPatched(SceUmdDiscInfo *disc_info)
{
	disc_info->uiSize = 8; 
	disc_info->uiMediaType = SCE_UMD_FMT_GAME; 

	return 0;
}

int sceUmdRegisterUMDCallBackPatched(SceUID cbid)
{
	umdcallback = cbid;
	UmdCallback();

	return 0;
}

int sceUmdUnRegisterUMDCallBackPatched(SceUID cbid)
{
	umdcallback = -1;
	return 0;
}

int sceUmdGetDriveStatPatched()
{
	return drivestat;
}

u32 FindUmdUserFunction(u32 nid)
{
	return FindProc("sceUmd_driver", "sceUmdUser", nid);
}

void PatchIsofsDriver(char *buf)
{
	// Patch StopModule to avoid crash at exit...
	//u32 *mod = (u32 *)sceKernelFindModuleByName("sceIsofs_driver");
	//u32 text_addr = *(mod+27);
	u32 text_addr = (u32)(buf+0xA0);

	char *iso = GetUmdFile();

	if (iso)
	{
		if (strstr(iso, "ms0:/") == iso)
		{
			if (config.usenoumd || config.useisofsonumdinserted)
			{
				// make module exit inmediately 
				_sw(0x03E00008, text_addr+0x4238);
				_sw(0x24020001, text_addr+0x423C);				

				ClearCaches();
			}
		}
	}

	/*_sw(0x03e00008, text_addr+0x4a2c);
	_sw(0x34020000, text_addr+0x4a30);*/
}

void PatchPower(char *buf)
{
	u32 text_addr = (u32)(buf+0xA0);

	_sw(0, text_addr+0xB24);
	ClearCaches();
}

void PatchWlan(char *buf)
{
	u32 text_addr = (u32)(buf+0xA0);

	_sw(0, text_addr+0x2520);
	ClearCaches();
}

void DoNoUmdPatches()
{
	REDIRECT_FUNCTION(FindUmdUserFunction(0xC6183D47), sceUmdActivatePatched);
	REDIRECT_FUNCTION(FindUmdUserFunction(0xE83742BA), sceUmdDeactivatePatched);
	REDIRECT_FUNCTION(FindUmdUserFunction(0x340B7686), sceUmdGetDiscInfoPatched);
	REDIRECT_FUNCTION(FindUmdUserFunction(0xAEE7404D), sceUmdRegisterUMDCallBackPatched);
	REDIRECT_FUNCTION(FindUmdUserFunction(0xBD2BDE07), sceUmdUnRegisterUMDCallBackPatched);
	MAKE_DUMMY_FUNCTION1(FindUmdUserFunction(0x46EBB729)); // sceUmdCheckMedium
	MAKE_DUMMY_FUNCTION0(FindUmdUserFunction(0x8EF08FCE)); // sceUmdWaitDriveStat
	MAKE_DUMMY_FUNCTION0(FindUmdUserFunction(0x56202973)); // sceUmdWaitDriveStatWithTimer
	MAKE_DUMMY_FUNCTION0(FindUmdUserFunction(0x4A9E5E29)); // sceUmdWaitDriveStatCB
	REDIRECT_FUNCTION(FindUmdUserFunction(0x6B4A146C), sceUmdGetDriveStatPatched);
	MAKE_DUMMY_FUNCTION0(FindUmdUserFunction(0x20628E6F)); // sceUmdGetErrorStat

	ClearCaches();
}

int sceChkregGetPsCodePatched(u8 *pscode)
{
	pscode[0] = 0x01;
	pscode[1] = 0x00;
	pscode[2] = config.fakeregion + 2;

	if (pscode[2] == 2)
		pscode[2] = 3;

	pscode[3] = 0x00;
	pscode[4] = 0x01;
	pscode[5] = 0x00;
	pscode[6] = 0x01;
	pscode[7] = 0x00;

	return 0;
}

void PatchChkreg()
{
	u32 pscode = FindProc("sceChkreg", "sceChkreg_driver", 0x59F8491D);
	int (* sceChkregGetPsCode)(u8 *);
	u8 code[8];

	int intr = sceKernelCpuSuspendIntr();

	if (pscode)
	{
		if (config.fakeregion)
		{
			REDIRECT_FUNCTION(pscode, sceChkregGetPsCodePatched);			
		}
		else
		{
			sceChkregGetPsCode = (void *)pscode;

			memset(code, 0, 8);
			sceChkregGetPsCode(code);

			if (code[2] == 0x06 || code[2] == 0x0A || code[2] == 0x0B
				|| code[2] == 0x0D)
			{
				REDIRECT_FUNCTION(pscode, sceChkregGetPsCodePatched);
			}
		}
	}

	/*u32 checkregion = FindProc("sceChkreg", "sceChkreg_driver", 0x54495B19);

	if (checkregion)
	{
		if (config.freeumdregion)
		{
			MAKE_DUMMY_FUNCTION1(checkregion);
		}
	}*/

	sceKernelCpuResumeIntr(intr);
	ClearCaches();
}

void SetSpeed(int cpu, int bus)
{
	scePowerSetClockFrequency_k = (void *)FindPowerDriverFunction(0x545A7F3C);
	scePowerSetClockFrequency_k(cpu, cpu, bus);
	
	int intr = sceKernelCpuSuspendIntr();

	MAKE_DUMMY_FUNCTION0((u32)scePowerSetClockFrequency_k);
	MAKE_DUMMY_FUNCTION0((u32)FindPowerDriverFunction(0x737486F2));
	MAKE_DUMMY_FUNCTION0((u32)FindPowerDriverFunction(0xB8D7B3FB));
	MAKE_DUMMY_FUNCTION0((u32)FindPowerDriverFunction(0x843FBF43));

	sceKernelCpuResumeIntr(intr);
	ClearCaches();
}

#define NAND_STATUS (*((volatile unsigned *)0xBD101004))
#define NAND_COMMAND (*((volatile unsigned *)0xBD101008))
#define NAND_ADDRESS (*((volatile unsigned *)0xBD10100C))
#define NAND_READDATA (*((volatile unsigned *)0xBD101300))
#define NAND_ENDTRANS (*((volatile unsigned *)0xBD101014))

u32 commands[20] =
{
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 
	0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01,
	0x00, 0x00, 0x01, 0xFF 
};

u8 commands_2[20] =
{
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01,
	0x01, 0x01, 0x00, 0xFF
};

u8 unk()
{
	int i;
	u8 read; // read?

	commands_2[19] = 1; 

	for (i = 0; i < 20; i++)
	{
		NAND_COMMAND = commands_2[i];
	}

	read = (u8)NAND_READDATA;

	commands_2[19] = 0; 

	for (i = 0; i < 20; i++)
	{
		NAND_COMMAND = commands_2[i];
	}

	NAND_ENDTRANS = 1;

	return read & 0xFF;
}

void SetActiveNand(u32 unit)
{
	// unit 0 -> UP, unit 1 -> internal nand
	int i;
	
	commands[19] = unit;

	for (i = 0; i < 20; i++)
	{
		NAND_COMMAND = commands[i];
		NAND_ADDRESS = 0;
		NAND_ENDTRANS = 1;
	}
}

#define BUS_CLOCK_ENABLE *(volatile u32 *)0xbc100050
#define IO_ENABLE	*(volatile u32 *)0xbc100078

int SysEventHandler(int event)
{
	if (event == 0x10004)
	{
		BUS_CLOCK_ENABLE |= (1 << 13);
		IO_ENABLE |= (1 << 0);
			
		SetActiveNand(1);
		unk();		
	}
	
	return 0;
}

u32 event_handler[] =
{
	0x40,
	"SleepRecovery",
	0x00FFFF00,
	(u32)SysEventHandler
};

void OnImposeLoad()
{
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceChkreg");

	if (mod)
	{
		sctrlSEGetConfig(&config);
		PatchChkreg();
	}
	
	if (sceKernelInitKeyConfig() == PSP_INIT_KEYCONFIG_GAME)
	{
		char *theiso = GetUmdFile();

		if (theiso)
		{
			if (strncmp(theiso, "ms0:/", 5) == 0)
			{
				if (config.usenoumd || config.useisofsonumdinserted)
				{
					sceIoDelDrv("isofs");
					sceIoAddDrv(getisofs_driver());
					
					if (config.usenoumd)
					{
						DoNoUmdPatches();
					}

					sceIoAssign("disc0:", "umd0:", "isofs0:", IOASSIGN_RDONLY, NULL, 0);
				}
			}			
		}		
	}

	if (sceKernelInitApitype() == PSP_INIT_APITYPE_DISC)
	{
		if (config.umdisocpuspeed == 333 || config.umdisocpuspeed == 300 ||
			config.umdisocpuspeed == 266 || config.umdisocpuspeed == 222)
			SetSpeed(config.umdisocpuspeed, config.umdisobusspeed);
	}	

	PatchMemlmd();
	sceKernelRegisterSysEventHandler(event_handler);
}





