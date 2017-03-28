release: all
	../psppacker/psppacker 380P popsloader.prx

TARGET = popsloader
OBJS = main.o pspdecrypt.o exports.o

BUILD_PRX=1
PRX_EXPORTS=exports.exp

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS=1
PSP_FW_VERSION=380

INCDIR = ../include
CFLAGS = -O2 -G0 -Wall -fno-pic 
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR = ../lib
LIBS = -lpspinit -lpspmodulemgr_kernel -lpspsystemctrl_kernel -lpspsysmem_kernel -lpspchnnlsv_driver_weak -lpsprtc_driver -lpsputils_kernel -lpspiofilemgr_kernel -lpspmesgd_driver -lpspsemaphore -lpspmemlmd -lpspmesgled_driver -lpspsuspend_kernel

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak

LIBS += -lpspge_driver


