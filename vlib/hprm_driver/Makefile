PSPSDK=$(shell psp-config --pspsdk-path)
sceHprm_driver_OBJS = sceHprm_driver_0000.o sceHprm_driver_0001.o sceHprm_driver_0002.o sceHprm_driver_0003.o sceHprm_driver_0004.o sceHprm_driver_0005.o sceHprm_driver_0006.o sceHprm_driver_0007.o sceHprm_driver_0008.o sceHprm_driver_0009.o sceHprm_driver_0010.o sceHprm_driver_0011.o sceHprm_driver_0012.o sceHprm_driver_0013.o sceHprm_driver_0014.o sceHprm_driver_0015.o sceHprm_driver_0016.o sceHprm_driver_0017.o sceHprm_driver_0018.o sceHprm_driver_0019.o sceHprm_driver_0020.o sceHprm_driver_0021.o 

%.o: sceHprm_driver.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(sceHprm_driver_OBJS)
	psp-ar cru libpsphprm_driver.a $(sceHprm_driver_OBJS)
	psp-ranlib libpsphprm_driver.a

clean:
	rm -f *.o
	rm -f *.a
