PSPSDK=$(shell psp-config --pspsdk-path)
sceIplUpdate_OBJS = sceIplUpdate_0000.o sceIplUpdate_0001.o sceIplUpdate_0002.o

%.o: sceIplUpdate.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(sceIplUpdate_OBJS)
	psp-ar cru libpspipl_update.a $(sceIplUpdate_OBJS)
	psp-ranlib libpspipl_update.a

clean:
	rm -f *.o
	rm -f *.a
