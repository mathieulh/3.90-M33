PSPSDK=$(shell psp-config --pspsdk-path)
UtilsForKernel_OBJS = UtilsForKernel_0000.o UtilsForKernel_0001.o UtilsForKernel_0002.o UtilsForKernel_0003.o UtilsForKernel_0004.o UtilsForKernel_0005.o UtilsForKernel_0006.o UtilsForKernel_0007.o UtilsForKernel_0008.o UtilsForKernel_0009.o UtilsForKernel_0010.o UtilsForKernel_0011.o UtilsForKernel_0012.o UtilsForKernel_0013.o UtilsForKernel_0014.o UtilsForKernel_0015.o UtilsForKernel_0016.o UtilsForKernel_0017.o UtilsForKernel_0018.o UtilsForKernel_0019.o UtilsForKernel_0020.o UtilsForKernel_0021.o UtilsForKernel_0022.o UtilsForKernel_0023.o UtilsForKernel_0024.o UtilsForKernel_0025.o UtilsForKernel_0026.o UtilsForKernel_0027.o UtilsForKernel_0028.o UtilsForKernel_0029.o UtilsForKernel_0030.o UtilsForKernel_0031.o UtilsForKernel_0032.o UtilsForKernel_0033.o UtilsForKernel_0034.o UtilsForKernel_0035.o UtilsForKernel_0036.o UtilsForKernel_0037.o UtilsForKernel_0038.o UtilsForKernel_0039.o UtilsForKernel_0040.o UtilsForKernel_0041.o UtilsForKernel_0042.o UtilsForKernel_0043.o UtilsForKernel_0044.o UtilsForKernel_0045.o UtilsForKernel_0046.o UtilsForKernel_0047.o UtilsForKernel_0048.o 
%.o: UtilsForKernel.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(UtilsForKernel_OBJS)
	psp-ar cru libpsputils_kernel.a $(UtilsForKernel_OBJS)
	psp-ranlib libpsputils_kernel.a

clean:
	rm -f *.o
	rm -f *.a
