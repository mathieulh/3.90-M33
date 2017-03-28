release: all
	psppacker 371P ipl_update.prx

TARGET = ipl_update
OBJS = main.o 

INCDIR = ../include
CFLAGS = -O2 -G0 -Wall -fno-pic
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBC=1
USE_KERNEL_LIBS=1

LIBDIR = ../lib
LDFLAGS = -mno-crt0 -nostartfiles
LIBS = -lpspnand_driver

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
