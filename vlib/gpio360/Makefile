PSPSDK=$(shell psp-config --pspsdk-path)
sceGpio_driver_OBJS = sceGpio_driver_0000.o sceGpio_driver_0001.o sceGpio_driver_0002.o sceGpio_driver_0003.o sceGpio_driver_0004.o sceGpio_driver_0005.o sceGpio_driver_0006.o sceGpio_driver_0007.o sceGpio_driver_0008.o sceGpio_driver_0009.o sceGpio_driver_0010.o sceGpio_driver_0011.o sceGpio_driver_0012.o sceGpio_driver_0013.o sceGpio_driver_0014.o sceGpio_driver_0015.o sceGpio_driver_0016.o sceGpio_driver_0017.o sceGpio_driver_0018.o sceGpio_driver_0019.o sceGpio_driver_0020.o 

%.o: sceGpio_driver.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(sceGpio_driver_OBJS)
	psp-ar cru libpspgpio_driver.a $(sceGpio_driver_OBJS)
	psp-ranlib libpspgpio_driver.a

clean:
	rm -f *.o
	rm -f *.a
