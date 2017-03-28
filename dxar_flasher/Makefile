TARGET = 351oeflasher
OBJS = main.o dxar.o conf.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR =
LDFLAGS = 
LIBS = -lpspsemaphore -lpsplflash_fatfmt -lpsppower_driver

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = 3.51 OE-A Firmware Install
PSP_EBOOT_ICON = ICON0.PNG

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

