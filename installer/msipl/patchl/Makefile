TARGET = patch
OBJS = \
  crt0.o \
  patchl.o \
  fat.o \
  string.o
  

INCDIR = 
CFLAGS = -O2 -G0 -Wall -mno-check-zero-division 
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBS = 
LIBDIR =
LDFLAGS = -nostartfiles -T patchl.x -nostdlib

PSPSDK=$(shell psp-config --pspsdk-path)
#include $(PSPSDK)/lib/build.mak
include build.mak
