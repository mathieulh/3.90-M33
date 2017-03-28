TARGET = oeupdmaker
OBJS = main.o dxar.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR =
LDFLAGS = 
LIBS = -lpspsemaphore -lpspmesgd_driver -lz -lpsppower_driver

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = 3.51 OE-A Update Maker

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

