release: all
	   psppacker 380I galaxy.prx

TARGET = galaxy
OBJS = main.o csoread.o exports.o

BUILD_PRX=1
PRX_EXPORTS=exports.exp

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS=1

INCDIR = ../include
CFLAGS = -O2 -G0 -Wall -fno-pic 
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR = ../lib
LIBS = -lpspsystemctrl_kernel -lpspsysmem_kernel -lpspumd_kernel -lpspinit

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
