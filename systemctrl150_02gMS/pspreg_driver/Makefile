PSPSDK=$(shell psp-config --pspsdk-path)
sceReg_driver_OBJS = sceReg_driver_0000.o sceReg_driver_0001.o sceReg_driver_0002.o sceReg_driver_0003.o sceReg_driver_0004.o sceReg_driver_0005.o sceReg_driver_0006.o sceReg_driver_0007.o sceReg_driver_0008.o sceReg_driver_0009.o sceReg_driver_0010.o sceReg_driver_0011.o sceReg_driver_0012.o sceReg_driver_0013.o sceReg_driver_0014.o sceReg_driver_0015.o sceReg_driver_0016.o sceReg_driver_0017.o sceReg_driver_0018.o sceReg_driver_0019.o 
%.o: sceReg_driver.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(sceReg_driver_OBJS)
	psp-ar cru libpspreg_driver.a $(sceReg_driver_OBJS)
	psp-ranlib libpspreg_driver.a

clean:
	rm -f *.o
	rm -f *.a
