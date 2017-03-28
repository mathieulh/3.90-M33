TARGET = sysreset
OBJS = main.o gpio.o me.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS += -lpspnand_driver


EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Sysreset

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

LIBS += -lpsphprm_driver

