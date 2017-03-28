PSPSDK=$(shell psp-config --pspsdk-path)
sceMesgLed_driver_OBJS = sceMesgLed_driver_0000.o sceMesgLed_driver_0001.o sceMesgLed_driver_0002.o sceMesgLed_driver_0003.o sceMesgLed_driver_0004.o sceMesgLed_driver_0005.o sceMesgLed_driver_0006.o sceMesgLed_driver_0007.o sceMesgLed_driver_0008.o sceMesgLed_driver_0009.o sceMesgLed_driver_0010.o sceMesgLed_driver_0011.o sceMesgLed_driver_0012.o sceMesgLed_driver_0013.o sceMesgLed_driver_0014.o sceMesgLed_driver_0015.o sceMesgLed_driver_0016.o sceMesgLed_driver_0017.o sceMesgLed_driver_0018.o sceMesgLed_driver_0019.o sceMesgLed_driver_0020.o sceMesgLed_driver_0021.o sceMesgLed_driver_0022.o sceMesgLed_driver_0023.o 

%.o: sceMesgLed_driver.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(sceMesgLed_driver_OBJS)
	psp-ar cru libpspmesgled_driver.a $(sceMesgLed_driver_OBJS)
	psp-ranlib libpspmesgled_driver.a

clean:
	rm -f *.o
	rm -f *.a
