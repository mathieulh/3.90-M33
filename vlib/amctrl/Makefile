PSPSDK=$(shell psp-config --pspsdk-path)
sceAmctrl_driver_OBJS = sceAmctrl_driver_0000.o sceAmctrl_driver_0001.o sceAmctrl_driver_0002.o sceAmctrl_driver_0003.o sceAmctrl_driver_0004.o sceAmctrl_driver_0005.o sceAmctrl_driver_0006.o sceAmctrl_driver_0007.o 

%.o: sceAmctrl_driver.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(sceAmctrl_driver_OBJS)
	psp-ar cru libpspamctrl.a $(sceAmctrl_driver_OBJS)
	psp-ranlib libpspamctrl.a

clean:
	rm -f *.o
	rm -f *.a
