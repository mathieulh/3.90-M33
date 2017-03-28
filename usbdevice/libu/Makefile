PSPSDK=$(shell psp-config --pspsdk-path)
pspUsbDevice_OBJS = pspUsbDevice_0000.o pspUsbDevice_0001.o pspUsbDevice_0002.o 
%.o: pspUsbDevice.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(pspUsbDevice_OBJS)
	psp-ar cru libpspusbdevice.a $(pspUsbDevice_OBJS)
	psp-ranlib libpspusbdevice.a

clean:
	rm -f *.o
	rm -f *.a
