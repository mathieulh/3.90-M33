TARGET = m33create
OBJS = main.o psar.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR =
LDFLAGS = 
LIBS = -lpspsemaphore -lpspmesgd_driver -lz -lpsppower_driver

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = 3.52 M33 CREATOR
PSP_EBOOT_ICON = ICON0.PNG

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

