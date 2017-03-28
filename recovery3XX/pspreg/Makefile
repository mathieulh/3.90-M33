PSPSDK=$(shell psp-config --pspsdk-path)
sceReg_OBJS = sceReg_0000.o sceReg_0001.o sceReg_0002.o sceReg_0003.o sceReg_0004.o sceReg_0005.o sceReg_0006.o sceReg_0007.o sceReg_0008.o sceReg_0009.o sceReg_0010.o sceReg_0011.o sceReg_0012.o sceReg_0013.o sceReg_0014.o sceReg_0015.o sceReg_0016.o sceReg_0017.o sceReg_0018.o sceReg_0019.o sceReg_0020.o 
%.o: sceReg.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(sceReg_OBJS)
	psp-ar cru libpspreg.a $(sceReg_OBJS)
	psp-ranlib libpspreg.a

clean:
	rm -f *.o
	rm -f *.a
