PSPSDK=$(shell psp-config --pspsdk-path)
sceUmd_OBJS = sceUmd_0000.o sceUmd_0001.o sceUmd_0002.o sceUmd_0003.o sceUmd_0004.o sceUmd_0005.o sceUmd_0006.o sceUmd_0007.o sceUmd_0008.o sceUmd_0009.o sceUmd_0010.o sceUmd_0011.o sceUmd_0012.o sceUmd_0013.o sceUmd_0014.o sceUmd_0015.o sceUmd_0016.o sceUmd_0017.o sceUmd_0018.o sceUmd_0019.o sceUmd_0020.o sceUmd_0021.o sceUmd_0022.o sceUmd_0023.o sceUmd_0024.o sceUmd_0025.o sceUmd_0026.o sceUmd_0027.o sceUmd_0028.o sceUmd_0029.o sceUmd_0030.o sceUmd_0031.o sceUmd_0032.o sceUmd_0033.o sceUmd_0034.o sceUmd_0035.o 

%.o: sceUmd.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(sceUmd_OBJS)
	psp-ar cru libpspumd_kernel.a $(sceUmd_OBJS)
	psp-ranlib libpspumd_kernel.a

clean:
	rm -f *.o
	rm -f *.a
