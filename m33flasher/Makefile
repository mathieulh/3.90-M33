TARGET = m33_351update
OBJS = main.o psar.o conf.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR =
LDFLAGS = 
LIBS = -lpsplflash_fatfmt -lpsppower_driver -lpsprtc_driver

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = 3.51 M33UPDATE

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

