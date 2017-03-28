PSPSDK=$(shell psp-config --pspsdk-path)
sceSuspendForKernel_OBJS = sceSuspendForKernel_0000.o sceSuspendForKernel_0001.o sceSuspendForKernel_0002.o sceSuspendForKernel_0003.o sceSuspendForKernel_0004.o sceSuspendForKernel_0005.o sceSuspendForKernel_0006.o sceSuspendForKernel_0007.o sceSuspendForKernel_0008.o sceSuspendForKernel_0009.o sceSuspendForKernel_0010.o sceSuspendForKernel_0011.o sceSuspendForKernel_0012.o sceSuspendForKernel_0013.o sceSuspendForKernel_0014.o sceSuspendForKernel_0015.o sceSuspendForKernel_0016.o 
%.o: sceSuspendForKernel.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(sceSuspendForKernel_OBJS)
	psp-ar cru libpspsuspend_kernel.a $(sceSuspendForKernel_OBJS)
	psp-ranlib libpspsuspend_kernel.a

clean:
	rm -f *.o
	rm -f *.a
