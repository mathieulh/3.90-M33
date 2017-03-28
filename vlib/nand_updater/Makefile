PSPSDK=$(shell psp-config --pspsdk-path)
sceNand_updater_driver_OBJS = sceNand_updater_driver_0000.o sceNand_updater_driver_0001.o 

%.o: sceNand_updater_driver.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(sceNand_updater_driver_OBJS)
	psp-ar cru libpspnandupdater_driver.a $(sceNand_updater_driver_OBJS)
	psp-ranlib libpspnandupdater_driver.a

clean:
	rm -f *.o
	rm -f *.a
