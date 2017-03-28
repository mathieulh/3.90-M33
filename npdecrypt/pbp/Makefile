TARGET = main
OBJS = main.o NpegLib.o

INCDIR = ../../include
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR = ../../lib
LDFLAGS = 
LIBS = -lpspkubridge -lpsppower -lpsprtc
PSP_FW_VERSION = 380

BUILD_PRX = 1

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = NPEG-RV

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

