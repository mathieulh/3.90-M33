#ifndef __SCTRLLIBRARY_H__
#define __SCTRLLIBRARY_H__

#include <pspsdk.h>
#include <pspkernel.h>
#include <psploadexec_kernel.h>
#include <pspinit.h>

/** 
 * All of these functions can be accessed in both, SE-C (or later) and
 * HEN-D (or later) in both, normal and devhook
 *
 * All of the functions can be used in both, user or kernel mode.
*/

/**
 * Restart the vsh.
 *
 * @param param - Pointer to a ::SceKernelLoadExecVSHParam structure, or NULL
 *
 * @returns < 0 on some errors.
 *
*/
int sctrlKernelExitVSH(struct SceKernelLoadExecVSHParam *param);

/**
 * Executes a new executable from a disc.
 * It is the function used by the firmware to execute the EBOOT.BIN from a disc.
 *
 * @param file - The file to execute.
 * @param param - Pointer to a ::SceKernelLoadExecVSHParam structure, or NULL.
 *
 * @returns < 0 on some errors. 
*/
int sctrlKernelLoadExecVSHDisc(const char *file, struct SceKernelLoadExecVSHParam *param);

/**
 * Executes a new executable from a disc.
 * It is the function used by the firmware to execute an updater from a disc.
 *
 * @param file - The file to execute.
 * @param param - Pointer to a ::SceKernelLoadExecVSHParam structure, or NULL.
 *
 * @returns < 0 on some errors. 
*/
int sctrlKernelLoadExecVSHDiscUpdater(const char *file, struct SceKernelLoadExecVSHParam *param);

/**
 * Executes a new executable from a memory stick.
 * It is the function used by the firmware to execute an updater from a memory stick.
 *
 * @param file - The file to execute.
 * @param param - Pointer to a ::SceKernelLoadExecVSHParam structure, or NULL.
 *
 * @returns < 0 on some errors. 
*/
int sctrlKernelLoadExecVSHMs1(const char *file, struct SceKernelLoadExecVSHParam *param);

/**
 * Executes a new executable from a memory stick.
 * It is the function used by the firmware to execute games (and homebrew :P) from a memory stick.
 *
 * @param file - The file to execute.
 * @param param - Pointer to a ::SceKernelLoadExecVSHParam structure, or NULL.
 *
 * @returns < 0 on some errors. 
*/
int sctrlKernelLoadExecVSHMs2(const char *file, struct SceKernelLoadExecVSHParam *param);

/**
 * Executes a new executable from a memory stick.
 * It is the function used by the firmware to execute ... ?
 *
 * @param file - The file to execute.
 * @param param - Pointer to a ::SceKernelLoadExecVSHParam structure, or NULL.
 *
 * @returns < 0 on some errors. 
*/
int sctrlKernelLoadExecVSHMs3(const char *file, struct SceKernelLoadExecVSHParam *param);

/**
 * Executes a new executable with the specified apitype
 *
 * @param apitype - The apitype
 * @param file - The file to execute.
 * @param param - Pointer to a ::SceKernelLoadExecVSHParam structure, or NULL.
 *
 * @returns < 0 on some errors. 
*/
int sctrlKernelLoadExecVSHWithApitype(int apitype, const char *file, struct SceKernelLoadExecVSHParam *param);

/**
 * Sets the api type 
 *
 * @param apitype - The apitype to set
 * @returns 0 on success
 *
 * @Note - this will modify also the value of sceKernelBootFrom, since the value of
 * bootfrom is calculated from the apitype
*/
int sctrlKernelSetInitApitype(int apitype);

/**
 * Sets the filename of the launched executable.
 *
 * @param filename - The filename to set
 * @returns 0 on success
*/
int sctrlKernelSetInitFileName(char *filename);

/**
 * Sets the init key config
 *
 * @param key - The key code
 * @returns 0 on success
*/
int sctrlKernelSetInitKeyConfig(int key);

/**
 * Sets the user level of the current thread
 *
 * @param level - The user level
 * @return the previous user level on success
 */
int sctrlKernelSetUserLevel(int level);

/**
 * Sets the devkit version
 * 
 * @param version - The devkit version to set
 * @return 0 on success
 * 
*/
int sctrlKernelSetDevkitVersion(int version);

/**
 * Checks if we are in SE.
 *
 * @returns 1 if we are in SE-C or later, 0 if we are in HEN-D or later,
 * and < 0 (a kernel error code) in any other case
*/
int	sctrlHENIsSE();

/**
 * Checks if we are in Devhook.
 *
 * @returns 1 if we are in SE-C/HEN-D for devhook  or later, 0 if we are in normal SE-C/HEN-D or later,
 * and < 0 (a kernel error code) in any other case
*/
int	sctrlHENIsDevhook();

/**
 * Gets the HEN version
 *
 * @returns - The HEN version
 *
 * HEN D / SE-C :  0x00000400
 */
int sctrlHENGetVersion();

/**
 * Finds a driver
 *
 * @param drvname - The name of the driver (without ":" or numbers)
 *
 * @returns the driver if found, NULL otherwise
 *
 */
PspIoDrv *sctrlHENFindDriver(char *drvname);

/** 
 * Finds a function.
 *
 * @param modname - The module where to search the function
 * @param libname - The library name
 * @nid - The nid of the function
 *
 * @returns - The function address or 0 if not found
 *
*/
u32 sctrlHENFindFunction(char *modname, char *libname, u32 nid);

typedef int (* APRS_EVENT)(char *modname, u8 *modbuf);

/**
 * Sets a function to be called after sceKernelApplyPspRelSection is called (useful for patching purposes)
 *
 * @param func - The function, that will receive the module name and the module buffer
 *
 * @returns - The previous set function (NULL if none);
*/
APRS_EVENT sctrlHENSetOnApplyPspRelSectionEvent(APRS_EVENT func);

#endif



