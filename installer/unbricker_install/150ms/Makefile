TARGET = ms150
OBJS = main.o pspPSAR.o pspDecrypt.o

INCDIR = ../include
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR = ../lib
LDFLAGS = 
LIBS = -lpsppower -lpspkubridge

PSP_FW_VERSION = 271

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = 150 Ms Installer

BUILD_PRX = 1

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

