PSPSDK=$(shell psp-config --pspsdk-path)
LoadCoreForKernel_OBJS = LoadCoreForKernel_0000.o LoadCoreForKernel_0001.o LoadCoreForKernel_0002.o LoadCoreForKernel_0003.o LoadCoreForKernel_0004.o LoadCoreForKernel_0005.o LoadCoreForKernel_0006.o LoadCoreForKernel_0007.o LoadCoreForKernel_0008.o LoadCoreForKernel_0009.o LoadCoreForKernel_0010.o LoadCoreForKernel_0011.o LoadCoreForKernel_0012.o LoadCoreForKernel_0013.o LoadCoreForKernel_0014.o LoadCoreForKernel_0015.o LoadCoreForKernel_0016.o LoadCoreForKernel_0017.o LoadCoreForKernel_0018.o LoadCoreForKernel_0019.o LoadCoreForKernel_0020.o LoadCoreForKernel_0021.o LoadCoreForKernel_0022.o LoadCoreForKernel_0023.o LoadCoreForKernel_0024.o LoadCoreForKernel_0025.o LoadCoreForKernel_0026.o LoadCoreForKernel_0027.o LoadCoreForKernel_0028.o LoadCoreForKernel_0029.o LoadCoreForKernel_0030.o LoadCoreForKernel_0031.o LoadCoreForKernel_0032.o LoadCoreForKernel_0033.o LoadCoreForKernel_0034.o LoadCoreForKernel_0035.o LoadCoreForKernel_0036.o LoadCoreForKernel_0037.o LoadCoreForKernel_0038.o LoadCoreForKernel_0039.o LoadCoreForKernel_0040.o 
%.o: LoadCoreForKernel.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(LoadCoreForKernel_OBJS)
	psp-ar cru libpsploadcore.a $(LoadCoreForKernel_OBJS)
	psp-ranlib libpsploadcore.a

clean:
	rm -f *.o
	rm -f *.a
