PSPSDK=$(shell psp-config --pspsdk-path)
sceUsb_driver_OBJS = sceUsb_driver_0000.o sceUsb_driver_0001.o sceUsb_driver_0002.o sceUsb_driver_0003.o sceUsb_driver_0004.o sceUsb_driver_0005.o sceUsb_driver_0006.o sceUsb_driver_0007.o sceUsb_driver_0008.o sceUsb_driver_0009.o sceUsb_driver_0010.o sceUsb_driver_0011.o sceUsb_driver_0012.o 

%.o: sceUsb_driver.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(sceUsb_driver_OBJS)
	psp-ar cru libpspusb_driver.a $(sceUsb_driver_OBJS)
	psp-ranlib libpspusb_driver.a

clean:
	rm -f *.o
	rm -f *.a
