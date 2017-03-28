TARGET = recovery
OBJS = main.o menu.o mydebug.o conf.o

CFLAGS = -O2 -Os -G0 -Wall -fno-pic
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBS = -lpspusb -lpspusbstor -lpspreg_driver -lpsppower_driver
BUILD_PRX = 1

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
