#ifndef __SCTRLLIBRARY_SE_H__
#define __SCTRLLIBRARY_SE_H__

/**
 * These functions are only available in SE-C and later, 
 * and they are not in HEN 
*/

typedef struct
{
	int magic; /* 0x47434553 */
	int hidecorrupt;
	int	skiplogo;
	int umdactivatedplaincheck;
	int gamekernel150;
	int executebootbin;
	int startupprog;
	int usenoumd;
	int useisofsonumdinserted;
	int	vshcpuspeed; /* not available yet */
	int	vshbusspeed; /* not available yet */
	int	umdisocpuspeed; /* not available yet */
	int	umdisobusspeed; /* not available yet */
	int fakeregion;
	int freeumdregion;
} SEConfig;


/**
 * Gets the SE/OE version
 *
 * @returns the SE version
 *
 * 3.03 OE-A: 0x00000500
*/
int sctrlSEGetVersion();

/**
 * Gets the SE configuration
 *
 * @param config - pointer to a SEConfig structure that receives the SE configuration
 * @returns 0 on success
*/
int sctrlSEGetConfig(SEConfig *config);

/**
 * Sets the SE configuration
 *
 * @param config - pointer to a SEConfig structure that has the SE configuration to set
 * @returns 0 on success
*/
int sctrlSESetConfig(SEConfig *config);

/**
 * Initiates the emulation of a disc from an ISO9660/CSO file.
 *
 * @param file - The path of the 
 * @param noumd - Wether use noumd or not
 * @param isofs - Wether use the custom SE isofs driver or not
 * 
 * @returns 0 on success
 *
 * @Note - When setting noumd to 1, isofs should also be set to 1,
 * otherwise the umd would be still required.
 *
 * @Note 2 - The function doesn't check if the file is valid or even if it exists
 * and it may return success on those cases
 *
 * @Note 3 - This function is not available in SE for devhook
 * @Example:
 *
 * SEConfig config;
 *
 * sctrlSEGetConfig(&config);
 *
 * if (config.usenoumd)
 * {
 *		sctrlSEMountUmdFromFile("ms0:/ISO/mydisc.iso", 1, 1);
 * }
 * else
 * {
 *		sctrlSEMountUmdFromFile("ms0:/ISO/mydisc.iso", 0, config.useisofsonumdinserted);
 * }
*/
int sctrlSEMountUmdFromFile(char *file, int noumd, int isofs);



#endif
