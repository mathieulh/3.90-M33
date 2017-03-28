PSPSDK=$(shell psp-config --pspsdk-path)
sceI2c_driver_OBJS = sceI2c_driver_0000.o sceI2c_driver_0001.o sceI2c_driver_0002.o sceI2c_driver_0003.o sceI2c_driver_0004.o sceI2c_driver_0005.o sceI2c_driver_0006.o sceI2c_driver_0007.o sceI2c_driver_0008.o sceI2c_driver_0009.o sceI2c_driver_0010.o sceI2c_driver_0011.o 
%.o: sceI2c_driver.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(sceI2c_driver_OBJS)
	psp-ar cru libpspi2c_driver.a $(sceI2c_driver_OBJS)
	psp-ranlib libpspi2c_driver.a

clean:
	rm -f *.o
	rm -f *.a
