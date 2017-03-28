PSPSDK=$(shell psp-config --pspsdk-path)
sceLed_driver_OBJS = sceLed_driver_0000.o sceLed_driver_0001.o sceLed_driver_0002.o sceLed_driver_0003.o sceLed_driver_0004.o sceLed_driver_0005.o 


%.o: sceLed_driver.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(sceLed_driver_OBJS)
	psp-ar cru libpspled_driver.a $(sceLed_driver_OBJS)
	psp-ranlib libpspled_driver.a

clean:
	rm -f *.o
	rm -f *.a
