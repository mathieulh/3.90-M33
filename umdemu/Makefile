release: all
	   ../psppacker/psppacker umdemu.prx

TARGET = umdemu
OBJS = main.o umd9660.o isoread.o csoread.o umdman.o mediaman.o exports.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall -fshort-wchar -fno-pic -mno-check-zero-division 
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1


LIBDIR =
LIBS = -lpspsysmem_kernel2  -lpspsystemctrl_kernel
LDFLAGS =  -nostdlib -nodefaultlibs

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak

