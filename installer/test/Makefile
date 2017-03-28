release : all
	mksfo 'Module Load Test' PARAM.SFO
	psppacker PBG ramsample.prx
	pack-pbp EBOOT.PBP PARAM.SFO NULL NULL NULL NULL NULL ramsample.prx NULL

TARGET = ramsample
OBJS = main.o Partition8Manager.o

INCDIR = ../include
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR = ../lib
LDFLAGS = 
LIBS = -lpspkubridge -lpspsystemctrl_user

PSP_FW_VERSION = 371

BUILD_PRX = 1

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

