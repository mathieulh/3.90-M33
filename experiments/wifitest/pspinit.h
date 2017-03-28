#ifndef __PSPINIT_H__

#define __PSPINIT_H__

enum PSPBootFrom
{
	PSP_BOOT_FLASH = 0, /* ? */
	PSP_BOOT_DISC = 0x20,
	PSP_BOOT_MS = 0x40,
};

enum PSPInitApitype
{
	PSP_INIT_APITYPE_DISC = 0x120,
	PSP_INIT_APITYPE_DISC_UPDATER = 0x121,
	PSP_INIT_APITYPE_MS1 = 0x140,
	PSP_INIT_APITYPE_MS2 = 0x141,
	PSP_INIT_APITYPE_MS3 = 0x142,
	PSP_INIT_APITYPE_VSH1 = 0x210, /* ExitGame */
	PSP_INIT_APITYPE_VSH2 = 0x220, /* ExitVSH */
};

/**
 * Gets the api type 
 *
 * @returns the api type in which the system has booted
*/
int sceKernelInitApitype();

/**
 * Gets the filename of the executable to be launched after all modules of the api.
 *
 * @returns filename of executable or NULL if no executable found.
*/
char *sceKernelInitFileName();

/**
 *
 * Gets the device in which the application was launched.
 *
 * @returns the device code, one of PSPBootFrom values.
*/
int sceKernelBootFrom();

#endif

