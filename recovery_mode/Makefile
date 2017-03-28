release: all
	psppacker 390I recovery.prx
#release: all
#	pack-pbp EBOOT.PBP PARAM.SFO ICON0.PNG NULL NULL NULL NULL recovery.prx NULL


TARGET = recovery
OBJS = main.o menu.o 

CFLAGS = -O2 -Os -G0 -Wall -fno-pic
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

INCDIR = ../include


USE_PSPSDK_LIBC = 1

LIBS = -lpspusb -lpspusbstor -lpspreg -lpsppower -lpspvshbridge -lpspusbdevice -lpspsystemctrl_user
BUILD_PRX = 1

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
