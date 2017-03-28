release: all
	mksfo 'Reflash' PARAM.SFO
	pack-pbp EBOOT.PBP PARAM.SFO NULL NULL NULL NULL NULL test.prx NULL


TARGET = test
OBJS = main.o 

INCDIR = 
CFLAGS = -O2 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1

LIBDIR = .
LIBS = 
LDFLAGS =

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
