PSPSDK=$(shell psp-config --pspsdk-path)
sceWlanDrv_driver_OBJS =  sceWlanDrv_driver_0000.o sceWlanDrv_driver_0001.o sceWlanDrv_driver_0002.o sceWlanDrv_driver_0003.o sceWlanDrv_driver_0004.o sceWlanDrv_driver_0005.o sceWlanDrv_driver_0006.o sceWlanDrv_driver_0007.o sceWlanDrv_driver_0008.o sceWlanDrv_driver_0009.o sceWlanDrv_driver_0010.o sceWlanDrv_driver_0011.o sceWlanDrv_driver_0012.o sceWlanDrv_driver_0013.o sceWlanDrv_driver_0014.o sceWlanDrv_driver_0015.o sceWlanDrv_driver_0016.o sceWlanDrv_driver_0017.o sceWlanDrv_driver_0018.o sceWlanDrv_driver_0019.o sceWlanDrv_driver_0020.o sceWlanDrv_driver_0021.o sceWlanDrv_driver_0022.o sceWlanDrv_driver_0023.o sceWlanDrv_driver_0024.o sceWlanDrv_driver_0025.o sceWlanDrv_driver_0026.o sceWlanDrv_driver_0027.o sceWlanDrv_driver_0028.o 

%.o: sceWlanDrv_driver.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(sceWlanDrv_driver_OBJS)
	psp-ar cru libpspwlan_driver.a $(sceWlanDrv_driver_OBJS)
	psp-ranlib libpspwlan_driver.a

clean:
	rm -f *.o
	rm -f *.a
