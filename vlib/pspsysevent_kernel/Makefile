PSPSDK=$(shell psp-config --pspsdk-path)
sceSysEventForKernel_OBJS = sceSysEventForKernel_0000.o sceSysEventForKernel_0001.o sceSysEventForKernel_0002.o sceSysEventForKernel_0003.o sceSysEventForKernel_0004.o sceSysEventForKernel_0005.o 

%.o: sceSysEventForKernel.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(sceSysEventForKernel_OBJS)
	psp-ar cru libpspsysevent_kernel.a $(sceSysEventForKernel_OBJS)
	psp-ranlib libpspsysevent_kernel.a

clean:
	rm -f *.o
	rm -f *.a
