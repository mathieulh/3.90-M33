PSPSDK=$(shell psp-config --pspsdk-path)
sceImpose_driver_OBJS = sceImpose_driver_0000.o sceImpose_driver_0001.o sceImpose_driver_0002.o sceImpose_driver_0003.o sceImpose_driver_0004.o sceImpose_driver_0005.o sceImpose_driver_0006.o sceImpose_driver_0007.o sceImpose_driver_0008.o sceImpose_driver_0009.o sceImpose_driver_0010.o sceImpose_driver_0011.o sceImpose_driver_0012.o sceImpose_driver_0013.o sceImpose_driver_0014.o sceImpose_driver_0015.o sceImpose_driver_0016.o sceImpose_driver_0017.o sceImpose_driver_0018.o sceImpose_driver_0019.o sceImpose_driver_0020.o sceImpose_driver_0021.o sceImpose_driver_0022.o sceImpose_driver_0023.o 

%.o: sceImpose_driver.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(sceImpose_driver_OBJS)
	psp-ar cru libpspimpose_driver.a $(sceImpose_driver_OBJS)
	psp-ranlib libpspimpose_driver.a

clean:
	rm -f *.o
	rm -f *.a
