release: all
	psppacker 390I systemctrl.prx


TARGET = systemctrl
OBJS = main.o sysmodpatches.o ../systemctrl_common/isofs_driver.o ../systemctrl_common/umd9660_driver.o ../systemctrl_common/isoread.o ../systemctrl_common/csoread.o ../systemctrl_common/malloc.o conf.o ../systemctrl_common/kubridge.o ../systemctrl_common/systemctrl.o ../systemctrl_common/init_patch.o 

INCDIR = ../include
CFLAGS = -Os -G0 -Wall -fshort-wchar -fno-pic -mno-check-zero-division -DIPL_VERSION
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1


PSP_FW_VERSION = 380

LIBDIR = ../lib
LIBS = -lpsploadcore_kernel380 -lpspinit -lpspmodulemgr_kernel380 -lpspsysmem_kernel380  -lpsploadexec_kernel380 -lpsputils_kernel -lpspinterruptmanager_kernel380
LDFLAGS =  -nostdlib -nodefaultlibs

PSPSDK=$(shell psp-config --pspsdk-path)
include build.mak
