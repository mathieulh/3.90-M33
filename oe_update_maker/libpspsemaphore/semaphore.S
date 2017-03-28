	.set noreorder

#include "pspimport.s"

#ifdef F_semaphore_0000
	IMPORT_START	"semaphore",0x00090000
#endif
#ifdef F_semaphore_0001
	IMPORT_FUNC	"semaphore",0x00EEC06A,sceUtilsBufferCopy
#endif
#ifdef F_semaphore_0002
	IMPORT_FUNC	"semaphore",0x8EEB7BF2,semaphore_8EEB7BF2
#endif
#ifdef F_semaphore_0003
	IMPORT_FUNC	"semaphore",0x4C537C72,semaphore_4C537C72
#endif
#ifdef F_semaphore_0004
	IMPORT_FUNC	"semaphore",0x77E97079,semaphore_77E97079
#endif
