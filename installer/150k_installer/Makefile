TARGET = main
OBJS = main.o pspPSAR.o pspDecrypt.o pspIplUpdate.o

INCDIR = ../include
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR = ../lib
LDFLAGS = 
LIBS = -lpsppower -lpspsystemctrl_user -lpspvshbridge -lpspkubridge -lz

PSP_FW_VERSION = 380

BUILD_PRX = 1

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = 1.50 Kernel Addon for 3.90 v2
PSP_EBOOT_ICON = ICON2.PNG

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

