TARGET = rebootex
OBJS = crt0.o main.o string.o psp_uart.o ms.o fat.o syscon.o sysreg.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall -fno-pic
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti 
ASFLAGS = $(CFLAGS)

LIBS = -lc
LIBDIR =
LDFLAGS = -nostartfiles -T rebootex.x

PSPSDK=$(shell psp-config --pspsdk-path)
#include $(PSPSDK)/lib/build.mak
include build.mak
