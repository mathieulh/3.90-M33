PSPSDK=$(shell psp-config --pspsdk-path)
pspUsbDevice_driver_OBJS = pspUsbDevice_driver_0000.o pspUsbDevice_driver_0001.o pspUsbDevice_driver_0002.o 
%.o: pspUsbDevice_driver.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(pspUsbDevice_driver_OBJS)
	psp-ar cru libpspusbdevice_driver.a $(pspUsbDevice_driver_OBJS)
	psp-ranlib libpspusbdevice_driver.a

clean:
	rm -f *.o
	rm -f *.a
