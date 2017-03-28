PSPSDK=$(shell psp-config --pspsdk-path)
sceCtrl_driver_OBJS = sceCtrl_driver_0000.o sceCtrl_driver_0001.o sceCtrl_driver_0002.o sceCtrl_driver_0003.o sceCtrl_driver_0004.o sceCtrl_driver_0005.o sceCtrl_driver_0006.o sceCtrl_driver_0007.o sceCtrl_driver_0008.o sceCtrl_driver_0009.o sceCtrl_driver_0010.o sceCtrl_driver_0011.o sceCtrl_driver_0012.o sceCtrl_driver_0013.o sceCtrl_driver_0014.o sceCtrl_driver_0015.o sceCtrl_driver_0016.o sceCtrl_driver_0017.o sceCtrl_driver_0018.o sceCtrl_driver_0019.o sceCtrl_driver_0020.o sceCtrl_driver_0021.o sceCtrl_driver_0022.o sceCtrl_driver_0023.o sceCtrl_driver_0024.o sceCtrl_driver_0025.o sceCtrl_driver_0026.o sceCtrl_driver_0027.o 

%.o: sceCtrl_driver.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(sceCtrl_driver_OBJS)
	psp-ar cru libpspctrl_driver.a $(sceCtrl_driver_OBJS)
	psp-ranlib libpspctrl_driver.a

clean:
	rm -f *.o
	rm -f *.a
