/*
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * pspnand_driver.h - Definitions and interfaces to the NAND (flash) driver.
 *
 * Copyright (c) 2005 Marcus R. Brown <mrbrown@0xd6.org>
 *
 * $Id: pspnand_driver.h 1211 2005-10-24 06:36:00Z mrbrown $
 */

#ifndef PSPNAND_DRIVER_H
#define PSPNAND_DRIVER_H

#include <pspkerneltypes.h>

#ifdef __cplusplus
extern "C" {
#endif

enum PspNandDriverErrorCodes 
{
	SCE_NAND_ERROR_READ_ECCFAILED = 0x80230003,
	SCE_NAND_ERROR_READEXTRA_ECCFAILED = 0x80230009,
};

int sceNandSetWriteProtect(int protectFlag);

int sceNandLock(int writeFlag);

void sceNandUnlock(void);

int sceNandReadStatus(void);

int sceNandReset(int flag);

int sceNandReadId(void *buf, SceSize size);

int sceNandReadPages(u32 ppn, void *buf, void *buf2, u32 count);

int sceNandReadExtraOnly(u32 ppn, void *buf, u32 count);

int sceNandReadPagesRawAll(u32 ppn, void *buf, void *buf2, u32 count);

int sceNandReadAccess(u32 ppn, void *buf, void *buf2, u32 count, u32 unk);



/*
// sceNandWritePages
// sceNandReadAccess
// sceNandWriteAccess
// sceNandEraseBlock
// sceNandReadExtraOnly
// sceNandCalcEcc
// sceNandVerifyEcc
// sceNandCollectEcc
*/

int sceNandGetPageSize(void);

int sceNandGetPagesPerBlock(void);

int sceNandGetTotalBlocks(void);

/*
// sceNandWriteBlock
// sceNandWriteBlockWithVerify
*/

int sceNandReadBlockWithRetry(u32 ppn, void *buf, void *buf2);

/*
// sceNandVerifyBlockWithRetry
// sceNandEraseBlockWithRetry
*/

int sceNandIsBadBlock(u32 ppn);

/*
// sceNandEraseAllBlock
// sceNandTestBlock
*/

#ifdef __cplusplus
}
#endif

#endif /* PSPNAND_DRIVER_H */
