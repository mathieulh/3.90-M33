TARGET = ramsample
OBJS = main.o 

INCDIR = ../include
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR = ../lib
LDFLAGS = 
LIBS = -lpspkubridge

PSP_FW_VERSION = 371

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Psp Slim Ram Sample 2

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

