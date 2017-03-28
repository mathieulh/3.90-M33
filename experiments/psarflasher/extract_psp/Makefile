#Note: this file needs to be in a pbp, otherwise it will be rejected by VSHMs functions

release: all
	mksfo 'Extract PRXs' PARAM.SFO
	pack-pbp extprxs.pbp PARAM.SFO NULL NULL NULL NULL NULL extprxs.prx NULL


TARGET = extprxs
OBJS = main.o PRXDecrypter.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS = -lpspvshbridge

BUILD_PRX = 1

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
