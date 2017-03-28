PSPSDK=$(shell psp-config --pspsdk-path)
sceRtc_driver_OBJS = sceRtc_driver_0000.o sceRtc_driver_0001.o sceRtc_driver_0002.o sceRtc_driver_0003.o sceRtc_driver_0004.o sceRtc_driver_0005.o sceRtc_driver_0006.o sceRtc_driver_0007.o sceRtc_driver_0008.o sceRtc_driver_0009.o sceRtc_driver_0010.o sceRtc_driver_0011.o sceRtc_driver_0012.o sceRtc_driver_0013.o sceRtc_driver_0014.o sceRtc_driver_0015.o sceRtc_driver_0016.o sceRtc_driver_0017.o sceRtc_driver_0018.o sceRtc_driver_0019.o sceRtc_driver_0020.o sceRtc_driver_0021.o sceRtc_driver_0022.o sceRtc_driver_0023.o sceRtc_driver_0024.o sceRtc_driver_0025.o sceRtc_driver_0026.o sceRtc_driver_0027.o sceRtc_driver_0028.o sceRtc_driver_0029.o sceRtc_driver_0030.o sceRtc_driver_0031.o sceRtc_driver_0032.o sceRtc_driver_0033.o sceRtc_driver_0034.o sceRtc_driver_0035.o  

%.o: sceRtc_driver.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(sceRtc_driver_OBJS)
	psp-ar cru libpsprtc_driver.a $(sceRtc_driver_OBJS)
	psp-ranlib libpsprtc_driver.a

clean:
	rm -f *.o
	rm -f *.a
