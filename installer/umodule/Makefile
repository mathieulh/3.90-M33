TARGET = u235
OBJS = main.o Plutonium.o

INCDIR = ../include
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR = ../lib
LDFLAGS = 
LIBS = -lpspkubridge -lpspsystemctrl_user

PSP_FW_VERSION = 352
USE_PSPSDK_LIBC = 1

BUILD_PRX = 1
USE_USER_LIBS = 1

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

