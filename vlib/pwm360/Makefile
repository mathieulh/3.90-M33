PSPSDK=$(shell psp-config --pspsdk-path)
scePwm_driver_OBJS =  scePwm_driver_0000.o scePwm_driver_0001.o scePwm_driver_0002.o scePwm_driver_0003.o scePwm_driver_0004.o scePwm_driver_0005.o scePwm_driver_0006.o scePwm_driver_0007.o scePwm_driver_0008.o scePwm_driver_0009.o 
%.o: scePwm_driver.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(scePwm_driver_OBJS)
	psp-ar cru libpsppwm_driver.a $(scePwm_driver_OBJS)
	psp-ranlib libpsppwm_driver.a

clean:
	rm -f *.o
	rm -f *.a
