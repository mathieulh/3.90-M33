TARGET = systemctrl_dh4se
OBJS = main.o sysmodpatches.o conf.o kubridge.o systemctrl.o exports.o 


INCDIR = 
CFLAGS = -O2 -G0 -Wall -fshort-wchar
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PRX_EXPORTS = export.exp

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1


LIBDIR =
LIBS = -lpspinit -lpspmodulemgr_kernel2 -lpspsemaphore


PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
