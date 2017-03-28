/*
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 *  pspchnnlsv_driver.h - Include for the pspChnnlsv_driver library.
 *
 * Copyright (c) 2005 Jim Paris <jim@jtan.com>
 * Copyright (c) 2005 psp123
 *
 * $Id: pspchnnlsv_driver.h 1559 2005-12-10 01:10:11Z jim $
 */
#ifndef __PSPCHNNLSV_DRIVER_H__
#define __PSPCHNNLSV_DRIVER_H__

/* The descriptions are mostly speculation. */

/** @defgroup Chnnlsv_driver Chnnlsv_driver Library
  * Library imports for the vsh chnnlsv_driver library.
  */

#ifdef __cplusplus
extern "C" {
#endif

#include <psptypes.h>

/** @addtogroup Chnnlsv_driver Chnnlsv_driver Library */
/*@{*/

typedef struct _pspChnnlsvContext1 {
	/** Cipher mode */
	int	mode;

	/** Context data */
	char	buffer1[0x10];
	char    buffer2[0x10];
	int	unknown;
} pspChnnlsvContext1;

typedef struct _pspChnnlsvContext2 {
	/** Context data */
	char    unknown[0x100];
} pspChnnlsvContext2;

/**
 * Initialize context
 *
 * @param ctx - Context
 * @param mode - Cipher mode
 * @returns < 0 on error
 */
int sceChnnlsv_driver_E7833020(pspChnnlsvContext1 *ctx, int mode);
	
/**
 * Process data
 *
 * @param ctx - Context
 * @param data - Data (aligned to 0x10)
 * @param len - Length (aligned to 0x10)
 * @returns < 0 on error
 */
int sceChnnlsv_driver_F21A1FCA(pspChnnlsvContext1 *ctx, unsigned char *data, int len);

/**
 * Finalize hash
 *
 * @param ctx - Context
 * @param hash - Hash output (aligned to 0x10, 0x10 bytes long)
 * @param cryptkey - Crypt key or NULL.
 * @returns < 0 on error
 */
int sceChnnlsv_driver_C4C494F8(pspChnnlsvContext1 *ctx, 
			unsigned char *hash, unsigned char *cryptkey);

/**
 * Prepare a key, and set up integrity check
 *
 * @param ctx - Context
 * @param mode1 - Cipher mode
 * @param mode2 - Encrypt mode (1 = encrypting, 2 = decrypting)
 * @param hashkey - Key out
 * @param cipherkey - Key in
 * @returns < 0 on error
 */
int sceChnnlsv_driver_ABFDFC8B(pspChnnlsvContext2 *ctx, int mode1, int mode2,
			unsigned char *hashkey, unsigned char *cipherkey);

/**
 * Process data for integrity check
 *
 * @param ctx - Context
 * @param data - Data (aligned to 0x10)
 * @param len - Length (aligned to 0x10)
 * @returns < 0 on error
 */
int sceChnnlsv_driver_850A7FA1(pspChnnlsvContext2 *ctx, unsigned char *data, int len);

/**
 * Check integrity
 *
 * @param ctx - Context
 * @returns < 0 on error
 */
int sceChnnlsv_driver_21BE78B4(pspChnnlsvContext2 *ctx);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif

