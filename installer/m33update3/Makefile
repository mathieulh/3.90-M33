release: all
	psppacker UPD main.prx
	pack-pbp EBOOT.PBP UPDATE.SFO ICON0.PNG NULL NULL NULL NULL main.prx NULL

TARGET = main
OBJS = main.o 

INCDIR = ../include
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR = ../lib
LDFLAGS = 
LIBS = -lpsppower -lpspsystemctrl_user -lpspvshbridge -lpspkubridge -lz

PSP_FW_VERSION = 371

BUILD_PRX = 1

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

