TARGET = iplboot
OBJS = \
  crt0.o \
  iplboot.o 

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBS = 
LIBDIR =
LDFLAGS = -nostartfiles -T iplboot.x -nostdlib

PSPSDK=$(shell psp-config --pspsdk-path)
#include $(PSPSDK)/lib/build.mak
include build.mak
