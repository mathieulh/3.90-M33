release: all
	pack-pbp EBOOT.PBP PARAM.SFO ICON0.PNG NULL NULL NULL NULL updateflasher.prx NULL


TARGET = updateflasher
OBJS = main.o pspPSAR.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR = ./
LDFLAGS =
LIBS = -lpspvshbridge_2 -lpspipl_update -lpsppower_2

BUILD_PRX = 1

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
