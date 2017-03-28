PSPSDK=$(shell psp-config --pspsdk-path)
sceMesgd_driver_OBJS = sceMesgd_driver_0000.o sceMesgd_driver_0001.o sceMesgd_driver_0002.o

%.o: sceMesgd_driver.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(sceMesgd_driver_OBJS)
	psp-ar cru libpspmesgd_driver.a $(sceMesgd_driver_OBJS)
	psp-ranlib libpspmesgd_driver.a

clean:
	rm -f *.o
	rm -f *.a
