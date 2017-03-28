all:
	mksfo 'LibTest' PARAM.SFO
	pack-pbp EBOOT.PBP PARAM.SFO NULL NULL NULL NULL NULL libtest.prx NULL 

TARGET = libtest
OBJS = main.o

INCDIR = 
CFLAGS = -O2 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
#USE_PSPSDK_LIBC = 1

LIBDIR =
LIBS = -lpspsystemctrl_user -lpspkubridge
LDFLAGS = 

#EXTRA_TARGETS = EBOOT.PBP
#PSP_EBOOT_TITLE = LoadTest


PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
