#include <stdio.h>
#include <string.h>

#define DUMP_START  0x08400000
#define DUMP_END    0x0A000000
#define ELF_START   0x08804000

void sw(char* p, int n)
{
    *((int*)p) = n;
}

int lw(char* p)
{
    return (*((int*)p));
}

void sh(char* p, int n)
{
    *((short*)p) = n;
}

int lh(char* p)
{
    return (*((short*)p));
}




//
// Bootstrap loader
//  limit 10 instructions max
//
//  global buffer ptr is at 0x08B61730
//
unsigned int pcode[] =
{
    0x8e250000,   //  lw          a1, $0000(s1)            #  global buffer pointer location is in s1
    0x00b02821,   //  addu        a1, a1, s0               #  offset into savefile is in s0
    0x00a00008,   //  jr          a1
    0x00000000,   //  nop
};




#define ENTRY_POINT  0x08556100





//
// Bootstrap relocator - with check that not reloading
//
unsigned int pcode2[] =
{
    // a0 = dest
    // a1 = src
    // a2 = length

    0x3c040000 + 0x4000 + ((ENTRY_POINT-0x100) >> 16),  //  lui   a0, $4870       #  0x40000000 = don't cache, needs fixup
    0x34060000,                                         //  ori   a2, zero, $0000 #  size of code, needs fixup
    0x34840000 + ((ENTRY_POINT-0x100) & 0x0000ffff),    //  ori   a0, a0, $1000 #
    0x8c880000,     //  lw              t0, $0000(a0)
    0x8ca90000,     //  lw              t1, $0000(a1)
    0x15090003,     //  bne             t0, t1, +3
    0x00000000,     //  nop
    0x0a258932,     //  j    $089624c8  // Resume, same for US and UK versions
    0x34020001,     //  ori             v0, zero, $0001
    0x8ca70000,     //  lw              a3, $0000(a1)
    0xac870000,     //  sw              a3, $0000(a0)
    0x24a50004,     //  addiu           a1, a1, $0004
    0x24840004,     //  addiu           a0, a0, $0004
    0x24c6fffc,     //  addiu           a2, a2, $fffc
    0x14c0fffa,     //  bne             a2, zero, __back5
    0x00000000,     //  nop
    0x08000000 + (ENTRY_POINT-0x80) / 4,                //  j                     #  jump to code at 0x08701180
    0x00000000,     //  nop
};







int nRegion = 0;
int nRegionOffset[] =
{
  0,  // dummy
  0,  // US
  0xC0, // UK
  0xC0,  // DE
  0   // JP
};
#define US 1
#define UK 2
#define DE 3
#define JP 4

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
      printf("not enough args\n");
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


    //
    // Region
    //
    nRegion = US;
    if (strstr(pszFile1, "uk") || strstr(pszFile1, "UK"))
    {
      nRegion = UK;
    }
    else if (strstr(pszFile1, "de") || strstr(pszFile1, "DE"))
    {
      nRegion = DE;
    }
    else if (strstr(pszFile1, "jp") || strstr(pszFile1, "JP"))
    {
      nRegion = JP;
    }

    printf("Assuming this is region %d\n", nRegion);



    //
    // Pad for relocator
    //
    int nProgramOffset = nBytes;
    int nPadBytes = 0x100;
    nBytes += nPadBytes;




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
    int nBytesReadProgram = fread(pdata + nBytes, 1, nProgramBytes, filein);
    printf("reading %s, %d bytes\n", pszFile2, nBytesReadProgram);
    if (nBytesReadProgram != nProgramBytes)
    {
        fprintf(stderr, "fread failed\n");
        exit(3);
    }
    fclose(filein);

    nBytes += nProgramBytes;
    nProgramBytes += nPadBytes;






    //
    // In LoadParser
    //
    char* sp = pdata + 8 - 0x10;






    ///////////////////////////
    //
    // Bootstrap loader
    //

    //
    // Extend SIMP section
    //  first 12 bytes is LoadParser's s0, s1, and ra
    //  next 8 bytes is LoadParser's unused area
    //  next 0x20 bytes are caller's unused area
    //
    int nExtend = 12 + sizeof(pcode);
    if (nExtend > 12 + 0x20)
    {
        printf("Error: pcode is more than 10 instructions\n");
        exit(1);
    }
    sw(pdata + 4, lw(pdata + 4) + nExtend);
    memmove(pdata + 0x00c4 + nExtend, pdata + 0x00c4, nBytes - 0x00c4);
    memset(pdata + 0x00c4, 0, nExtend);
    nBytes += nExtend;
    nProgramOffset += nExtend;


    //
    // lw  s0, $00cc(sp)
    // lw  s1, $00d0(sp)
    // lw  ra, $00d4(sp)
    //  global buffer ptr is at 0x08B61730
    //


    sw(sp + 0x00cc, nProgramOffset);   // s0   offset to program in savefile in global buffer
    sw(sp + 0x00d0, 0x08B61730 + (nRegionOffset[nRegion]));   // s1   global buffer ptr location
    sw(sp + 0x00d4, 0x09FF6E28);       // ra   sp + 0x00d8 (load) or sp + 0x00d8 (init)



    // load sp = 0x09FF6D50
    // init sp = 0x09FF6D90
    // global buffer ptr at 0x08B61730




    // ELF_START   0x08804000




    // Insert bootstrap code for load case
    memcpy(sp + 0x00d8, pcode, sizeof(pcode));

    // Insert bootstrap code for init case into unused area (0x84-0xac) at 0x88
    memcpy(sp + 0x00d8-0x40, pcode, sizeof(pcode));




    // Fixup relocator
    pcode2[1] |= nProgramBytes;

    // Insert relocator code
    memcpy(pdata + nProgramOffset, pcode2, sizeof(pcode2));










    //
    // Write file
    //  s0=$00018000 /// size of data.bin passed to SavedataInit = $18000 = 98304
    //
    char* pszFileOut = argv[3];
    fileout = fopen(pszFileOut, "wb");
    if (!fileout)
    {
        printf("Failed to open out file");
        exit(4);
    }
    printf("%d bytes written to %s\n", fwrite(pdata, 1, nBytes, fileout), pszFileOut);
    printf("program bytes limit is %d\n", 98304 - nProgramOffset - 0x100);
    fclose(fileout);



    free(pdata);
    return (0);
}
