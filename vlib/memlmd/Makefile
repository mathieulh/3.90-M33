PSPSDK=$(shell psp-config --pspsdk-path)
memlmd_OBJS = memlmd_0000.o memlmd_0001.o memlmd_0002.o memlmd_0003.o memlmd_0004.o memlmd_0005.o memlmd_0006.o memlmd_0007.o memlmd_0008.o memlmd_0009.o 
%.o: memlmd.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(memlmd_OBJS)
	psp-ar cru libpspmemlmd.a $(memlmd_OBJS)
	psp-ranlib libpspmemlmd.a

clean:
	rm -f *.o
	rm -f *.a
