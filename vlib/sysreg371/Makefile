PSPSDK=$(shell psp-config --pspsdk-path)
sceSysreg_driver_OBJS = sceSysreg_driver_0000.o sceSysreg_driver_0001.o sceSysreg_driver_0002.o sceSysreg_driver_0003.o sceSysreg_driver_0004.o sceSysreg_driver_0005.o sceSysreg_driver_0006.o sceSysreg_driver_0007.o 

%.o: sceSysreg_driver.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(sceSysreg_driver_OBJS)
	psp-ar cru libpspsysreg_driver.a $(sceSysreg_driver_OBJS)
	psp-ranlib libpspsysreg_driver.a

clean:
	rm -f *.o
	rm -f *.a
