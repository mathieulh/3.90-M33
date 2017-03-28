PSPSDK=$(shell psp-config --pspsdk-path)
sceChnnlsv_driver_OBJS = sceChnnlsv_driver_0000.o sceChnnlsv_driver_0001.o sceChnnlsv_driver_0002.o sceChnnlsv_driver_0003.o sceChnnlsv_driver_0004.o sceChnnlsv_driver_0005.o sceChnnlsv_driver_0006.o 
%.o: sceChnnlsv_driver.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(sceChnnlsv_driver_OBJS)
	psp-ar cru libpspchnnlsv_driver_weak.a $(sceChnnlsv_driver_OBJS)
	psp-ranlib libpspchnnlsv_driver_weak.a

clean:
	rm -f *.o
	rm -f *.a
