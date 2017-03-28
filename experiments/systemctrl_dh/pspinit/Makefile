PSPSDK=$(shell psp-config --pspsdk-path)
InitForKernel_OBJS = InitForKernel_0000.o InitForKernel_0001.o InitForKernel_0002.o InitForKernel_0003.o InitForKernel_0004.o InitForKernel_0005.o InitForKernel_0006.o InitForKernel_0007.o InitForKernel_0008.o InitForKernel_0009.o InitForKernel_0010.o InitForKernel_0011.o  

%.o: InitForKernel.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(InitForKernel_OBJS)
	psp-ar cru libpspinit.a $(InitForKernel_OBJS)
	psp-ranlib libpspinit.a

clean:
	rm -f *.o
	rm -f *.a
