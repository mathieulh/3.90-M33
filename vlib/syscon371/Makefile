PSPSDK=$(shell psp-config --pspsdk-path)
sceSyscon_driver_OBJS = sceSyscon_driver_0000.o sceSyscon_driver_0001.o sceSyscon_driver_0002.o sceSyscon_driver_0003.o 

%.o: sceSyscon_driver.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(sceSyscon_driver_OBJS)
	psp-ar cru libpspsyscon_driver.a $(sceSyscon_driver_OBJS)
	psp-ranlib libpspsyscon_driver.a

clean:
	rm -f *.o
	rm -f *.a
