PSPSDK=$(shell psp-config --pspsdk-path)
sceDve_driver_OBJS = sceDve_driver_0000.o sceDve_driver_0001.o sceDve_driver_0002.o sceDve_driver_0003.o sceDve_driver_0004.o sceDve_driver_0005.o sceDve_driver_0006.o sceDve_driver_0007.o sceDve_driver_0008.o sceDve_driver_0009.o sceDve_driver_0010.o sceDve_driver_0011.o sceDve_driver_0012.o sceDve_driver_0013.o sceDve_driver_0014.o sceDve_driver_0015.o sceDve_driver_0016.o sceDve_driver_0017.o sceDve_driver_0018.o sceDve_driver_0019.o sceDve_driver_0020.o sceDve_driver_0021.o sceDve_driver_0022.o 

%.o: sceDve_driver.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(sceDve_driver_OBJS)
	psp-ar cru libpspdve_driver.a $(sceDve_driver_OBJS)
	psp-ranlib libpspdve_driver.a

clean:
	rm -f *.o
	rm -f *.a
