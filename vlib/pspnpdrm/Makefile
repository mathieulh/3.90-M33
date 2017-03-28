PSPSDK=$(shell psp-config --pspsdk-path)
scePspNpDrm_driver_OBJS = scePspNpDrm_driver_0000.o scePspNpDrm_driver_0001.o scePspNpDrm_driver_0002.o scePspNpDrm_driver_0003.o scePspNpDrm_driver_0004.o scePspNpDrm_driver_0005.o scePspNpDrm_driver_0006.o 


%.o: scePspNpDrm_driver.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(scePspNpDrm_driver_OBJS)
	psp-ar cru libpspnpdrm.a $(scePspNpDrm_driver_OBJS)
	psp-ranlib libpspnpdrm.a

clean:
	rm -f *.o
	rm -f *.a
