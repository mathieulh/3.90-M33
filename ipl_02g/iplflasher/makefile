release: all
	psp-strip flashusb.elf -o extprxs.elf

TARGET = flashusb
OBJS = main.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall -fno-pic
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR =
LDFLAGS = 
LIBS = -lpspusb -lpspusbstor -lpspsemaphore -lpspnand_driver

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Flash USB Encrypted

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

LIBS += -lpsphprm_driver

