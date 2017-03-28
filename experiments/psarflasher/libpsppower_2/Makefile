PSPSDK=$(shell psp-config --pspsdk-path)
scePower_OBJS = scePower_0000.o scePower_0001.o scePower_0002.o scePower_0003.o scePower_0004.o scePower_0005.o scePower_0006.o scePower_0007.o scePower_0008.o scePower_0009.o scePower_0010.o scePower_0011.o scePower_0012.o scePower_0013.o scePower_0014.o scePower_0015.o scePower_0016.o scePower_0017.o scePower_0018.o scePower_0019.o scePower_0020.o scePower_0021.o scePower_0022.o scePower_0023.o scePower_0024.o scePower_0025.o scePower_0026.o scePower_0027.o scePower_0028.o scePower_0029.o scePower_0030.o scePower_0031.o scePower_0032.o scePower_0033.o scePower_0034.o scePower_0035.o scePower_0036.o scePower_0037.o scePower_0038.o scePower_0039.o scePower_0040.o scePower_0041.o scePower_0042.o scePower_0043.o scePower_0044.o scePower_0045.o scePower_0046.o scePower_0047.o 


%.o: scePower.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(scePower_OBJS)
	psp-ar cru libpsppower_2.a $(scePower_OBJS)
	psp-ranlib libpsppower_2.a

clean:
	rm -f *.o
	rm -f *.a
