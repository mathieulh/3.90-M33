#ifndef __KULIBRARY__

#define __KULIBRARY__

#include <pspsdk.h>
#include <pspkernel.h>

/**
  * Load a module using ModuleMgrForKernel.
  * 
  * @param path - The path to the module to load.
  * @param flags - Unused, always 0 .
  * @param option  - Pointer to a mod_param_t structure. Can be NULL.
  *
  * @returns The UID of the loaded module on success, otherwise one of ::PspKernelErrorCodes.
 */
SceUID kuKernelLoadModule(const char *path, int flags, SceKernelLMOption *option);


/**
  * Load a module with a specific apitype
  * 
  * @param apìtype - The apitype
  * @param path - The path to the module to load.
  * @param flags - Unused, always 0 .
  * @param option  - Pointer to a mod_param_t structure. Can be NULL.
  *
  * @returns The UID of the loaded module on success, otherwise one of ::PspKernelErrorCodes.
  */
SceUID kuKernelLoadModuleWithApitype2(int apitype, const char *path, int flags, SceKernelLMOption *option);

/**
 * Gets the api type 
 *
 * @returns the api type in which the system has booted
*/
int kuKernelInitApitype();

/**
 * Gets the filename of the executable to be launched after all modules of the api.
 *
 * @param initfilename - String where copy the initfilename
 * @returns 0 on success
*/
int kuKernelInitFileName(char *initfilename);

/**
 *
 * Gets the device in which the application was launched.
 *
 * @returns the device code, one of PSPBootFrom values.
*/
int kuKernelBootFrom();

/**
 * Get the user level of the current thread
 *
 * @return The user level, < 0 on error
 */
int kuKernelGetUserLevel(void);

#endif

