PSPSDK=$(shell psp-config --pspsdk-path)
LoadExecForKernel_OBJS = LoadExecForKernel_0000.o LoadExecForKernel_0001.o LoadExecForKernel_0002.o LoadExecForKernel_0003.o LoadExecForKernel_0004.o LoadExecForKernel_0005.o LoadExecForKernel_0006.o LoadExecForKernel_0007.o LoadExecForKernel_0008.o LoadExecForKernel_0009.o LoadExecForKernel_0010.o LoadExecForKernel_0011.o LoadExecForKernel_0012.o LoadExecForKernel_0013.o LoadExecForKernel_0014.o LoadExecForKernel_0015.o LoadExecForKernel_0016.o LoadExecForKernel_0017.o LoadExecForKernel_0018.o 
%.o: LoadExecForKernel.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(LoadExecForKernel_OBJS)
	psp-ar cru libpsploadexec_kernel_weak.a $(LoadExecForKernel_OBJS)
	psp-ranlib libpsploadexec_kernel_weak.a

clean:
	rm -f *.o
	rm -f *.a
