/**
 * File for native encrypt/decrypt functions
*/

#include <pspsdk.h>

/** Commands for KIRK encryption/decryption engine. */
enum PspKirkCommands 
{
	PSP_KIRK_DECRYPT = 1,
	PSP_KIRK_ENCRYPT = 4,
	PSP_KIRK_ENCRYPT2 = 5,
	PSP_KIRK_SCRAMBLE = 7,
	PSP_KIRK_ENCRYPT3 = 8,
	PSP_KIRK_SHA1 = 0x0B,
};

/**
 * Sends a command to the KIRK encryption/decryption engine.
 *
 * @param inbuf - The input buffer
 * @param insize - The size of input buffer
 * @param outbuf - The output buffer
 * @param outsize - The size of output buffer
 * @param cmd - The commands to send to KIRK engine. (one of PspKirkCommands)
 *
 * @returns < 0 on error
 */
int semaphore_4C537C72(void *inbuf, SceSize insize, void *outbuf, int outsize, int cmd);

/**
 * Used for PSAR decoding (1.00 bogus)
 *
 * @param buf - The in/out buffer to decode.
 * @param bufsize - The size of the buffer pointed by buf
 * @param retSize - Pointer to an integer that receives the size of 
 * the decoded data.
 * 
 * @returns < 0 on error
*/
int sceNwman_driver_9555D68D(void* buf, SceSize bufsize, int* retSize);

/**
 * Used for PSAR decoding 
 *
 * @param buf - The in/out buffer to decode.
 * @param bufsize - The size of the buffer pointed by buf
 * @param retSize - Pointer to an integer that receives the size of 
 * the decoded data.
 * 
 * @returns < 0 on error
*/
int sceMesgd_driver_102DC8AF(void* buf, SceSize bufsize, int* retSize);
