TARGET = M33update
OBJS = main.o conf.o table.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR =
LDFLAGS = 
LIBS = 

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = M33 Update
PSP_EBOOT_ICON = icon.png
PSP_EBOOT_PIC1 = pic1.png
PSP_EBOOT_PSAR = signature.bin

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

