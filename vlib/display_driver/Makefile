PSPSDK=$(shell psp-config --pspsdk-path)
sceDisplay_driver_OBJS = sceDisplay_driver_0000.o sceDisplay_driver_0001.o sceDisplay_driver_0002.o sceDisplay_driver_0003.o sceDisplay_driver_0004.o sceDisplay_driver_0005.o sceDisplay_driver_0006.o sceDisplay_driver_0007.o sceDisplay_driver_0008.o sceDisplay_driver_0009.o sceDisplay_driver_0010.o sceDisplay_driver_0011.o sceDisplay_driver_0012.o sceDisplay_driver_0013.o sceDisplay_driver_0014.o sceDisplay_driver_0015.o sceDisplay_driver_0016.o sceDisplay_driver_0017.o sceDisplay_driver_0018.o sceDisplay_driver_0019.o sceDisplay_driver_0020.o sceDisplay_driver_0021.o sceDisplay_driver_0022.o sceDisplay_driver_0023.o sceDisplay_driver_0024.o sceDisplay_driver_0025.o sceDisplay_driver_0026.o sceDisplay_driver_0027.o 

%.o: sceDisplay_driver.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(sceDisplay_driver_OBJS)
	psp-ar cru libpspdisplay_driver.a $(sceDisplay_driver_OBJS)
	psp-ranlib libpspdisplay_driver.a

clean:
	rm -f *.o
	rm -f *.a
