	.set noreorder

#include "pspimport.s"

// Build List
// semaphore_0000.o semaphore_0001.o semaphore_0002.o semaphore_0003.o semaphore_0004.o 

#ifdef F_semaphore_0000
	IMPORT_START	"semaphore",0x00010011
#endif
#ifdef F_semaphore_0001
	IMPORT_FUNC	"semaphore",0x00EEC06A,sceUtilsBufferCopy
#endif
#ifdef F_semaphore_0002
	IMPORT_FUNC	"semaphore",0x4C537C72,sceUtilsBufferCopyWithRange
#endif
#ifdef F_semaphore_0003
	IMPORT_FUNC	"semaphore",0x77E97079,sceUtilsBufferCopyByPollingWithRange
#endif
#ifdef F_semaphore_0004
	IMPORT_FUNC	"semaphore",0x8EEB7BF2,sceUtilsBufferCopyByPolling
#endif
