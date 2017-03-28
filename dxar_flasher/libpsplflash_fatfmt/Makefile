PSPSDK=$(shell psp-config --pspsdk-path)
FATFMT_OBJS = LflashFatfmt_0000.o LflashFatfmt_0001.o

%.o: LflashFatfmt.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(FATFMT_OBJS)
	psp-ar cru libpsplflash_fatfmt.a $(FATFMT_OBJS)
	psp-ranlib libpsplflash_fatfmt.a

clean:
	rm -f *.o
	rm -f *.a
