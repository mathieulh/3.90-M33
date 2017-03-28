#include <pspthreadman.h>
#include <pspmodulemgr.h>

#define O_RDONLY 1
#define O_WRONLY 2
#define O_RDWR   3
#define O_CREAT  0x200
#define O_TRUNC  0x400
#define O_APPEND 0x100

// function args are readily in registers, so just do syscall, seems to work
// syscall numbers from PSPInside's syscalls.txt --abu
int sceIoOpen(char *name, int flags, int mode) {
	asm("syscall 0x208f");
}

void sceIoClose(int fd) {
	asm("syscall 0x208d");
}

int sceIoRead(int fd,char *buf,int sz) {
	asm("syscall 0x2091");
}

int sceIoWrite(SceUID fd, const void *data, SceSize size) {
	asm("syscall 0x2093");
}

int sceKernelExitGame()
{
	asm("syscall 0x20F2");
}

int sceKernelLoadExec()
{
	asm("syscall 0x20F0");
}

SceUID sceKernelLoadModule(const char *path, int flags, SceKernelLMOption *option)
{
	asm("syscall 0x20d2");
}

int sceKernelStartModule(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option)
{
	asm("syscall 0x20d6");
}

// calls kernel load directly
SceUID _sceKernelLoadModule(const char *path, int flags, SceKernelLMOption *option)
{
  __asm__ volatile ("move $16, $27;\n"   // save k1
              		  "move $27, $0;\n"
              			"la $2, 0x8805b5f8\n"
              		  "jalr $2\n"
              			"nop\n"
              			"move $27, $16\n"
              			: : : "$27", "$16", "$2" );
}

int _sceKernelStartModule(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option)
{
	asm("syscall 0x20d6");
}

int sceKernelTerminateDeleteThread(SceUID thid)
{
	register int lret asm("$2");
	register int lthid asm("$4") = thid;

	asm ("syscall 0x2074;"
		: "=r" (lret)
		: "r" (lthid)
		);

	return(lret);
}

int sceKernelCancelCallback(SceUID cbid)
{
	register int lret asm("$2");
	register int lthid asm("$4") = cbid;

	asm ("syscall 0x2010"
		: "=r" (lret)
		: "r" (lthid)
		);

	return(lret);
}

int sceKernelDeleteCallback(SceUID cbid)
{
	register int lret asm("$2");
	register int lthid asm("$4") = cbid;

	asm ("syscall 0x200E;"
		: "=r" (lret)
		: "r" (lthid)
		);

	return(lret);
}

int sceKernelReferCallbackStatus(SceUID cb, SceKernelCallbackInfo *info)
{
	register int lret asm("$2");
	register int lthid asm("$4") = cb;
	register int linfo asm("$5") = info;

	asm("syscall 0x2013"
		: "=r" (lret)
		: "r" (lthid), "r" (linfo)
		);

	return(lret);	
}

int sceKernelReferThreadStatus(int thid, SceKernelThreadInfo *info)
{
	register int lret asm("$2");
	register int lthid asm("$4") = thid;
	register int linfo asm("$5") = info;

	asm("syscall 0x2080"
		: "=r" (lret)
		: "r" (lthid), "r" (linfo)
		);

	return(lret);
}

void sceKernelDelayThread(SceUInt delay)
{
	register unsigned long lsecs asm("$4") = delay;

	asm __volatile__ ("syscall 0x201c;"
		:
	: "r" (lsecs)
		);
}

int sceKernelGetThreadId()
{
	register int lret asm("$2");
	asm("syscall 0x207B"
		: "=r" (lret));

	return(lret);
}

int sceKernelGetThreadmanIdList(enum SceKernelIdListType type, SceUID *readbuf, int readbufsize, int *idcount)
{
	asm("syscall 0x2083");
}

SceUID sceKernelAllocPartitionMemory(SceUID partitionid, const char *name, int type, SceSize size, void *addr)
{
	register SceUID lreturn asm ("$2");
	register SceUID lpart asm ("$4") = partitionid;
	register const char * lname asm ("$5") = name;
	register int ltype asm ("$6") = type;
	register SceSize lsize asm ("$7") = size;
	register void* laddr asm("$8") = addr;

	asm __volatile__ ("syscall 0x2169;"   // fake : exitgame
		: "=r" (lreturn)
		: "r" (lpart), "r" (lname), "r" (ltype), "r" (lsize), "r" (laddr)
	);
	return(lreturn);
}

int sceKernelFreePartitionMemory(SceUID blockid)
{
  register int lreturn asm ("$2");
  register SceUID lblockid asm ("$4") = blockid;

  asm __volatile__ ("syscall 0x20E2;"
                    : "=r" (lreturn)
                    : "r" (lblockid)
                   );
  return(lreturn);
}

void * sceKernelGetBlockHeadAddr(SceUID blockid)
{
	register void *lreturn asm ("$2");
	register SceUID lblockid asm ("$4") = blockid;

	asm __volatile__ ("syscall 0x20e3;"
		: "=r" (lreturn)
		: "r" (lblockid)
		);
	return(lreturn);
}

int sceKernelCreateThread(const char *name, SceKernelThreadEntry entry, int initPriority,
						  int stackSize, SceUInt attr, SceKernelThreadOptParam *option) {
	asm("syscall 0x206d");
}

int sceKernelStartThread(SceUID thid, SceSize args, void *argp) {
	asm("syscall 0x206f");
}

SceSize sceKernelMaxFreeMemSize(void)
{
	register SceSize lreturn asm ("$2") = 0;

	asm __volatile__ ("syscall 0x20DF;"
		: "=r" (lreturn)
		);
	return(lreturn);
}

SceSize sceKernelTotalFreeMemSize(void)
{
	register SceSize lreturn asm ("$2") = 0;

	asm __volatile__ ("syscall 0x20E0;"
		: "=r" (lreturn)
		);
	return(lreturn);
}

int sceKernelDcacheWritebackInvalidateAll() {
	asm("syscall 0x20c7");
}

int sceKernelDcacheWritebackAll() {
	asm("syscall 0x20c6");
}

int sceKernelIcacheInvalidateAll() {
	asm("syscall 0x20cd");
}

int sceKernelDeleteFpl(SceUID fplid)
{
	register int lret asm("$2");
	register int lthid asm("$4") = fplid;

	asm ("syscall 0x204C"
		: "=r" (lret)
		: "r" (lthid)
		);

	return(lret);
}

int sceKernelDeleteSema(SceUID vplid)
{
	register int lret asm("$2");
	register int lthid asm("$4") = vplid;

	asm ("syscall 0x2021"
		: "=r" (lret)
		: "r" (lthid)
		);

	return(lret);
}

int sceKernelDeleteEventFlag(SceUID vplid)
{
	register int lret asm("$2");
	register int lthid asm("$4") = vplid;

	asm ("syscall 0x2029"
		: "=r" (lret)
		: "r" (lthid)
		);

	return(lret);
}

int sceKernelReleaseSubIntrHandler(SceUID intno, SceUID no)
{
  register int lret asm("$2");
  register int lfplid asm("$4") = intno;
  register void* linfo asm("$5") = no;

   asm("syscall 0x2001"
     : "=r" (lret)
     : "r" (lfplid), "r" (linfo)
   );

  return(lret);
}

int sceKernelDisableSubIntr(SceUID intno, SceUID no)
{
  register int lret asm("$2");
  register int lfplid asm("$4") = intno;
  register void* linfo asm("$5") = no;

   asm("syscall 0x2003"
     : "=r" (lret)
     : "r" (lfplid), "r" (linfo)
   );

  return(lret);
}

int sceUmdUnRegisterUMDCallBack() {
	asm("syscall 0x21BF");
}

int scePowerUnregisterCallback() {
	asm("syscall 0x219B");
}

int sceAudioChRelease( SceUID modid )
{
	register int lret asm("$2");
	register int lthid asm("$4") = modid;

	asm ("syscall 0x2135"
		: "=r" (lret)
		: "r" (lthid)
		);

	return(lret);
}

// Hack in some savedata support - because my toolchain needs updating

/** title, savedataTitle, detail: parts of the unencrypted SFO
    data, it contains what the VSH and standard load screen shows */
typedef struct PspUtilitySavedataSFOParam
{
        char title[0x80];
        char savedataTitle[0x80];
        char detail[0x400];
        unsigned char parentalLevel;
        unsigned char unknown[3];
} PspUtilitySavedataSFOParam;

typedef struct PspUtilitySavedataFileData {
        void *buf;
        SceSize bufSize;
        SceSize size;	/* ??? - why are there two sizes? */
        int unknown;
} PspUtilitySavedataFileData;

/** Structure to hold the parameters for the ::sceUtilitySavedataInitStart function.
  */
typedef struct SceUtilitySavedataParam
{
        /** Size of the structure */
        SceSize size;

        int language;

        int buttonSwap;

        int unknown[4];
        int result;
        int unknown2[4];

        /** mode: 0 to load, 1 to save */
        int mode;
        int unknown12;

        /** unknown13 use 0x10 */
        int unknown13;

        /** gameName: name used from the game for saves, equal for all saves */
        char gameName[16];
        /** saveName: name of the particular save, normally a number */
        char saveName[24];
        /** fileName: name of the data file of the game for example DATA.BIN */
        char fileName[16];

        /** pointer to a buffer that will contain data file unencrypted data */
        void *dataBuf;
        /** size of allocated space to dataBuf */
        SceSize dataBufSize;
        SceSize dataSize;

        PspUtilitySavedataSFOParam sfoParam;

        PspUtilitySavedataFileData icon0FileData;
        PspUtilitySavedataFileData icon1FileData;
        PspUtilitySavedataFileData pic1FileData;
        PspUtilitySavedataFileData snd0FileData;

        unsigned char unknown17[4];
} SceUtilitySavedataParam;


/**
 * Saves or Load savedata to/from the passed structure
 * After having called this continue calling sceUtilitySavedataGetStatus to
 * check if the operation is completed
 *
 * @param params - savedata parameters
 * @returns 0 on success
 */
int sceUtilitySavedataInitStart(SceUtilitySavedataParam * params)
{
	register int lret asm("$2");
	register void* lparams asm("$4") = params;

	asm ("syscall 0x2200"
		: "=r" (lret)
		: "r" (lparams)
		);

	return(lret);
}

/**
 * Check the current status of the saving/loading/shutdown process
 * Continue calling this to check current status of the process
 * before calling this call also sceUtilitySavedataUpdate
 * @returns 2 if the process is still being processed.
 * 3 on save/load success, then you can call sceUtilitySavedataShutdownStart.
 * 4 on complete shutdown.
 */
int sceUtilitySavedataGetStatus(void)
{
	register int lret asm("$2");

	asm ("syscall 0x2203"
		: "=r" (lret)
		);

	return(lret);
}


/**
 * Shutdown the savedata utility. after calling this continue calling
 * ::sceUtilitySavedataGetStatus to check when it has shutdown
 *
 * @return 0 on success
 *
 */
int sceUtilitySavedataShutdownStart(void)
{
	register int lret asm("$2");

	asm ("syscall 0x2201"
		: "=r" (lret)
		);

	return(lret);
}


/**
 * Refresh status of the savedata function
 *
 * @param unknown - unknown, pass 1
 */
void sceUtilitySavedataUpdate(int unknown)
{
	register int lparams asm("$4") = unknown;

	asm ("syscall 0x2200"
		:
		: "r" (lparams)
		);
}

