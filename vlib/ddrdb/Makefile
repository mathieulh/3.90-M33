PSPSDK=$(shell psp-config --pspsdk-path)
sceDdrdb_driver_OBJS = sceDdrdb_driver_0000.o sceDdrdb_driver_0001.o sceDdrdb_driver_0002.o sceDdrdb_driver_0003.o sceDdrdb_driver_0004.o sceDdrdb_driver_0005.o sceDdrdb_driver_0006.o sceDdrdb_driver_0007.o sceDdrdb_driver_0008.o sceDdrdb_driver_0009.o 

%.o: sceDdrdb_driver.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(sceDdrdb_driver_OBJS)
	psp-ar cru libpspddrb_driver.a $(sceDdrdb_driver_OBJS)
	psp-ranlib libpspddrb_driver.a

clean:
	rm -f *.o
	rm -f *.a
