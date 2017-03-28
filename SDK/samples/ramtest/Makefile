TARGET = ramsample
OBJS = main.o 

INCDIR = ../../include
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR = ../../lib
LDFLAGS = 
LIBS = -lpspkubridge -lpspsystemctrl_user

PSP_FW_VERSION = 371
PSP_LARGE_MEMORY = 1
BUILD_PRX = 1

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Psp Slim Large Memory Sample

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

