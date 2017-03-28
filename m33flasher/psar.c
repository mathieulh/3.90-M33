#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <psputilsforkernel.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "psar.h"

#define OVERHEAD    0 /* size of encryption block overhead */
#define SIZE_A      0x110 /* size of uncompressed file entry = 272 bytes */

int DecodeBlock(const u8* pIn, int cbIn, u8* pOut)
{
    if (pIn != pOut)
	{
		memcpy(pOut, pIn, cbIn);
	}

	return cbIn;
}

int iBase;

int pspPSARInit(u8 *dataPSAR, u8 *dataOut, u8 *dataOut2)
{
	if (memcmp(dataPSAR, "PSAR", 4) != 0)
    {
        return -1;
    }

    int cbOut;
	
    // at the start of the PSAR file,
    //   there are one or two special version data chunks
    // printf("Special PSAR records:\n");
    cbOut = DecodeBlock(&dataPSAR[0x10], OVERHEAD+SIZE_A, dataOut);
    if (cbOut <= 0)
    {
        return cbOut;
    }

    if (cbOut != SIZE_A)
    {
        return -1;
    }
   
    iBase = 0x10+OVERHEAD+SIZE_A; // after first entry
            // iBase points to the next block to decode (0x10 aligned)

	// second block
	cbOut = DecodeBlock(&dataPSAR[0x10+OVERHEAD+SIZE_A], *(u32 *)&dataOut[0x90], dataOut2);
	if (cbOut <= 0)
	{
		return -1;		
	}
       
	iBase += OVERHEAD+cbOut; 
	return 0;
}

int pspPSARGetNextFile(u8 *dataPSAR, int cbFile, u8* dataOut, u8 *dataOut2, char *name, int *retSize, int *retPos)
{
	int cbOut;
	
	if (iBase >= (cbFile-OVERHEAD))
		return 0; // no more files

	cbOut = DecodeBlock(&dataPSAR[iBase], OVERHEAD+SIZE_A, dataOut);
	if (cbOut <= 0)
	{
		return -1;
	}
	if (cbOut != SIZE_A)
	{
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
					printf("No.\n");
				}
				*retSize = ret;				
			}
                    
			else
			{			
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
		return -1;
	}
        
	iBase += cbDataChunk; 
	*retPos = iBase;

	return 1; // morefiles
}

