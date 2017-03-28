#include <stdlib.h>
#include <stdio.h>

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





// works, but crashes after running out to the street, similar place twice
// does not crash in load menu or cancel load menu in any way
//#define ENTRY_POINT  0x09ed0100

// crashes when trying to use load menu again
// this might be where utility modules get loaded
// maybe should malloc some memory and put it there
//#define ENTRY_POINT  0x087d0100

// loaded fine, but also crashed in load menu like 87d01000
//#define ENTRY_POINT  0x087f0100

// crashed in load menu after cancel
//#define ENTRY_POINT  0x08701100

// good, but crashes if load while thread still running, next heap allocation is at 0x09fb4700
//#define ENTRY_POINT  0x09fb0100

// crashed on loading
//#define ENTRY_POINT  0x08400100

// trying to put myself inside the next allocation
#define ENTRY_POINT  0x09fb5100





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
    0x34840000 + ((ENTRY_POINT-0x100) &amp; 0x0000ffff),    //  ori   a0, a0, $1000 #
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
#define US 1
#define UK 2



main(int argc, char* argv[])
{
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
    FILE* filein = stdin;
    FILE* fileout = stdout;



    //
    // Read file
    //
    char* pszFile1 = "data.org";
    if (argc >= 2)
         pszFile1 = argv[1];
    filein = fopen(pszFile1, "rb");
    if (!filein)
    {
        fprintf(stderr, "Couldn't open input file %s\n", pszFile1);
        exit(1);
    }
    int nBytes = FileSize(pszFile1);
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
        nRegion = UK;



    //
    // Pad for relocator
    //
    int nProgramOffset = nBytes;
    int nPadBytes = 0x100;
    nBytes += nPadBytes;




    //
    // Read file
    //
    char* pszFile2 = "program.bin";
    filein = fopen(pszFile2, "rb");
    if (!filein)
    {
        fprintf(stderr, "Couldn't open program file %s\n", pszFile2);
        exit(1);
    }
    int nProgramBytes = FileSize(pszFile2);
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
    sw(sp + 0x00d0, 0x08B61730 + (nRegion == UK ? 0xC0 : 0));   // s1   global buffer ptr location
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
    assert(nProgramBytes < 0x7fff);
    pcode2[1] |= nProgramBytes;

    // Insert relocator code
    memcpy(pdata + nProgramOffset, pcode2, sizeof(pcode2));










    //
    // Write file
    //  s0=$00018000 /// size of data.bin passed to SavedataInit = $18000 = 98304
    //
    char* pszFileOut = "data.bin";
    if (argc >= 3)
         pszFileOut = argv[2];
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
</PRE></BODY></HTML>
