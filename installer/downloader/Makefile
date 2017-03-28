TARGET = downloader
OBJS = main.o sceHttp.o 

BUILD_PRX = 1
PSP_LARGE_MEMORY = 1
INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR = ../lib
INCDIR = ../include
LDFLAGS =
LIBS = -lpspgu -lpspgum -lpspsystemctrl_user -lpsppower -lpsprtc
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = DL
USE_PSPSDK_LIBC = 1

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
