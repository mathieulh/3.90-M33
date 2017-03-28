release: all
	psp-strip resurrection.elf 

TARGET = resurrection
OBJS = main.o ipl.o pspPSAR.o pspDecrypt.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall -fno-pic
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR =
LDFLAGS = 
LIBS = -lpspnand_driver -lpsplflash_fatfmt -lpspled_driver -lpspsysreg_driver -lpspsyscon_driver -lpspdisplay_driver

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak


