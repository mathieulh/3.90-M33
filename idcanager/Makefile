release: all
	psppacker 380I idcanager.prx

TARGET = idcanager
OBJS = main.o exports.o

BUILD_PRX=1
PRX_EXPORTS=exports.exp

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS=1

INCDIR = ../include
CFLAGS = -O2 -G0 -Wall -fno-pic
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR = ../lib
LIBS = -lpspsysmem_kernel -lpspnpdrm -lpsploadexec_kernel

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
