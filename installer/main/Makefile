release: all
	pack-pbp EBOOT.PBP UPDATE.SFO ICON2.PNG NULL NULL NULL NULL main.prx NULL

TARGET = main
OBJS = main.o pspDegeneration.o Plutonium.o

INCDIR = ../include
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR = ../lib
LDFLAGS = 
LIBS = -lpsppower -lpspsystemctrl_user -lpspvshbridge

PSP_FW_VERSION = 352

BUILD_PRX = 1
USE_PSPSDK_LIBC = 1

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

