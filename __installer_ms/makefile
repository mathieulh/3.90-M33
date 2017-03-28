TARGET = pandora_slim
OBJS = main.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR =
LDFLAGS = 
# -lpspglue -lpsplibc

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Pandora Ext. for Slim 3.60 M33 Installer


PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

