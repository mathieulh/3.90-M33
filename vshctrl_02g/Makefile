release: all
	psppacker 390IS vshctrl_02g.prx


TARGET = vshctrl_02g
OBJS = main.o virtualpbpmgr.o exports.o

INCDIR = ../include
CFLAGS = -O2 -Os -G0 -Wall -fshort-wchar -fno-pic -mno-check-zero-division -DIPL_VERSION -DPSP_SLIM
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1

PSP_FW_VERSION = 371

LIBDIR = ../lib
LIBS = -lpsploadexec_kernel -lpspsystemctrl_kernel  -lpspreg_driver -lpsprtc_driver -lpspusb_driver -lpspusbdevice_driver 

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
