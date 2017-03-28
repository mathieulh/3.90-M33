TARGET = updateflasher
OBJS = main.o pspPSAR.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR = ./
LDFLAGS =
LIBS = -lpspvshbridge_2 -lpspipl_update -lpsppower_2 -lpspnand_driver

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Crossed PSAR flasher


PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
