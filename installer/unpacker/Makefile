release: all
#	psppacker UPD main.prx	
	pack-pbp EBOOT.PBP UPDATE.SFO ICON3.PNG NULL NULL NULL NULL main.prx NULL

TARGET = main
OBJS = main.o pspIplUpdate.o

INCDIR = ../include
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR = ../lib
LDFLAGS = 
LIBS = -lpsppower -lpspsystemctrl_user -lpspvshbridge -lpspkubridge

PSP_FW_VERSION = 352

BUILD_PRX = 1
USE_PSPSDK_LIBC = 1
USE_USER_LIBS = 1

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

