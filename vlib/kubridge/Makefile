PSPSDK=$(shell psp-config --pspsdk-path)
KUBridge_OBJS = KUBridge_0000.o KUBridge_0001.o KUBridge_0002.o KUBridge_0003.o KUBridge_0004.o KUBridge_0005.o KUBridge_0006.o KUBridge_0007.o KUBridge_0008.o KUBridge_0009.o 

%.o: KUBridge.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(KUBridge_OBJS)
	psp-ar cru libpspkubridge.a $(KUBridge_OBJS)
	psp-ranlib libpspkubridge.a

clean:
	rm -f *.o
	rm -f *.a
