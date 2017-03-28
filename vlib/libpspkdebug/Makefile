PSPSDK=$(shell psp-config --pspsdk-path)
KDebugForKernel_OBJS = KDebugForKernel_0000.o KDebugForKernel_0001.o KDebugForKernel_0002.o KDebugForKernel_0003.o KDebugForKernel_0004.o KDebugForKernel_0005.o KDebugForKernel_0006.o KDebugForKernel_0007.o KDebugForKernel_0008.o KDebugForKernel_0009.o KDebugForKernel_0010.o KDebugForKernel_0011.o KDebugForKernel_0012.o KDebugForKernel_0013.o KDebugForKernel_0014.o KDebugForKernel_0015.o KDebugForKernel_0016.o KDebugForKernel_0017.o KDebugForKernel_0018.o KDebugForKernel_0019.o KDebugForKernel_0020.o KDebugForKernel_0021.o KDebugForKernel_0022.o KDebugForKernel_0023.o KDebugForKernel_0024.o KDebugForKernel_0025.o KDebugForKernel_0026.o KDebugForKernel_0027.o KDebugForKernel_0028.o KDebugForKernel_0029.o 
%.o: KDebugForKernel.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(KDebugForKernel_OBJS)
	psp-ar cru libpspkdebug.a $(KDebugForKernel_OBJS)
	psp-ranlib libpspkdebug.a

clean:
	rm -f *.o
	rm -f *.a
