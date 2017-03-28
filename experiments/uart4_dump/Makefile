TARGET = uart4
OBJS = main.o 

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LIBS = 
LDFLAGS = -mno-crt0 -nostartfiles

BUILD_PRX = 1

USE_PSPSDK_LIBC=1
USE_KERNEL_LIBS=1

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
