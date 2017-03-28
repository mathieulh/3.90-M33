#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspcrypt.h>
#include <psputilsforkernel.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "libpsardumper.h"

/* most code here from psppet's psardumper */

PSP_MODULE_INFO("pspPSAR_Driver", 0x1006, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

u8 check_keys0[0x10] =
{
	0x71, 0xF6, 0xA8, 0x31, 0x1E, 0xE0, 0xFF, 0x1E,
	0x50, 0xBA, 0x6C, 0xD2, 0x98, 0x2D, 0xD6, 0x2D
}; 

u8 check_keys1[0x10] =
{
	0xAA, 0x85, 0x4D, 0xB0, 0xFF, 0xCA, 0x47, 0xEB,
	0x38, 0x7F, 0xD7, 0xE4, 0x3D, 0x62, 0xB0, 0x10
};

int Encrypt(u32 *buf, int size)
{
	buf[0] = 4;
	buf[1] = buf[2] = 0;
	buf[3] = 0x100;
	buf[4] = size;

	/* Note: this encryption returns different data in each psp,
	   But it always returns the same in a specific psp (even if it has two nands) */
	if (semaphore_4C537C72(buf, size+0x14, buf, size+0x14, 5) < 0)
		return -1;

	return 0;
}

int GenerateSigCheck(u8 *buf)
{
	u8 enc[0xD0+0x14];
	int iXOR, res;

	memcpy(enc+0x14, buf+0x110, 0x40);
	memcpy(enc+0x14+0x40, buf+0x80, 0x90);
	
	for (iXOR = 0; iXOR < 0xD0; iXOR++)
	{
		enc[0x14+iXOR] ^= check_keys0[iXOR&0xF]; 
	}

	if ((res = Encrypt((u32 *)enc, 0xD0)) < 0)
	{
		printf("Encrypt failed.\n");
		return res;
	}

	for (iXOR = 0; iXOR < 0xD0; iXOR++)
	{
		enc[0x14+iXOR] ^= check_keys1[iXOR&0xF];
	}

	memcpy(buf+0x80, enc+0x14, 0xD0);
	return 0;
}

#define OVERHEAD    0x150 /* size of encryption block overhead */
#define SIZE_A      0x110 /* size of uncompressed file entry = 272 bytes */

// for 1.50 and later, they mangled the plaintext parts of the header
void Demangle(const u8* pIn, u8* pOut)
{
    u8 buffer[20+0x130];
	memcpy(buffer+20, pIn, 0x130);
    u32* pl = (u32*)buffer; // first 20 bytes
    pl[0] = 5;
    pl[1] = pl[2] = 0;
    pl[3] = 0x55;
    pl[4] = 0x130;

    semaphore_4C537C72(buffer, 20+0x130, buffer, 20+0x130, PSP_KIRK_SCRAMBLE);
    memcpy(pOut, buffer, 0x130);
}

int DecodeBlock(const u8* pIn, int cbIn, u8* pOut)
{
    // pOut also used as temporary buffer for mangled input
    // assert((((u32)pOut) & 0x3F) == 0); // must be aligned

    memcpy(pOut, pIn, cbIn + 0x10); // copy a little more for $10 page alignment

    int ret;
    int cbOut;
    
    // new style - scrambled PSAR (works with 1.5x and 2.00)
    Demangle(pIn+0x20, pOut+0x20); // demangle the inside $130 bytes
    if (*(u32*)&pOut[0xd0] != 0x0E000000)
    {
		//printf("Demangle failed\n");
        //printf(" PSAR format not supported\n");
        return -4;
	}

	ret = sceMesgd_driver_102DC8AF(pOut, cbIn, &cbOut);
    if (ret != 0)
		;//***printf("Sys_DecodeE returned $%x %d\n", ret, cbOut);
    
    if (ret != 0)
        return ret; // error

	return cbOut;
}

int iBase, cbChunk;

int pspPSARInit(u8 *dataPSAR, u8 *dataOut, u8 *dataOut2)
{
	int k1 = pspSdkSetK1(0);

	if (memcmp(dataPSAR, "PSAR", 4) != 0)
    {
        pspSdkSetK1(k1);
		return -1;
    }

    int cbOut;
	
    // at the start of the PSAR file,
    //   there are one or two special version data chunks
    // printf("Special PSAR records:\n");
    cbOut = DecodeBlock(&dataPSAR[0x10], OVERHEAD+SIZE_A, dataOut);
    if (cbOut <= 0)
    {
        pspSdkSetK1(k1);
        return cbOut;
    }

    if (cbOut != SIZE_A)
    {
        pspSdkSetK1(k1);
        return -1;
    }
   
    iBase = 0x10+OVERHEAD+SIZE_A; // after first entry
            // iBase points to the next block to decode (0x10 aligned)

	// second block
	cbOut = DecodeBlock(&dataPSAR[0x10+OVERHEAD+SIZE_A], OVERHEAD+100, dataOut2);
	if (cbOut <= 0)
	{
		//printf("Performing V2.70 test\n"); // version 2.7 is bigger
		cbOut = DecodeBlock(&dataPSAR[0x10+OVERHEAD+SIZE_A], OVERHEAD+144, dataOut2);
		if (cbOut <= 0)
		{
			pspSdkSetK1(k1);
			return -1;
		}
	}
       
	cbChunk = (cbOut + 15) & 0xFFFFFFF0;
	iBase += OVERHEAD+cbChunk; 

	pspSdkSetK1(k1);
	return 0;
}

int pspPSARGetNextFile(u8 *dataPSAR, int cbFile, u8 *dataOut, u8 *dataOut2, char *name, int *retSize, int *retPos)
{
	int k1 = pspSdkSetK1(0);
	int cbOut;

	if (iBase >= (cbFile-OVERHEAD))
		return 0; // no more files

	cbOut = DecodeBlock(&dataPSAR[iBase], OVERHEAD+SIZE_A, dataOut);
	if (cbOut <= 0)
	{
		pspSdkSetK1(k1);
		return -1;
	}
	if (cbOut != SIZE_A)
	{
		pspSdkSetK1(k1);
		return -1;	
	}

	strcpy(name, (const char*)&dataOut[4]);
	u32* pl = (u32*)&dataOut[0x100];
	int sigcheck = (dataOut[0x10F] == 2);
        
		// pl[0] is 0
		// pl[1] is the PSAR chunk size (including OVERHEAD)
		// pl[2] is true file size (TypeA=272=SIZE_A, TypeB=size when expanded)
		// pl[3] is flags or version?
	if (pl[0] != 0)
	{
		pspSdkSetK1(k1);
		return -1;
	}

	iBase += OVERHEAD + SIZE_A;
	u32 cbDataChunk = pl[1]; // size of next data chunk (including OVERHEAD)
	u32 cbExpanded = pl[2]; // size of file when expanded

	if (cbExpanded > 0)
	{
		cbOut = DecodeBlock(&dataPSAR[iBase], cbDataChunk, dataOut);
		if (cbOut > 10 && dataOut[0] == 0x78 && dataOut[1] == 0x9C)
		{
			// standard Deflate header

			const u8* pbIn = &dataOut[2]; // after header
			u32 pbEnd;
			int ret = sceKernelDeflateDecompress(dataOut2, cbExpanded, pbIn, &pbEnd);
			
			if (ret == cbExpanded)
			{
                if (sigcheck)
				{
					GenerateSigCheck(dataOut2);
				}
				*retSize = ret;				
			}
                    
			else
			{
				pspSdkSetK1(k1);
				return -1;
			}
		}

		else
		{
			return -1;
		}
	}

	else if (cbExpanded == 0)
	{
         // Directory	
	}
	
	else
	{
		pspSdkSetK1(k1);
		return -1;
	}
        
	iBase += cbDataChunk; 
	*retPos = iBase;
	pspSdkSetK1(k1);
	return 1; // morefiles
}

int module_start(SceSize args, void *argp)
{
	return 0;
}

int module_stop(void)
{
	return 0;
}
