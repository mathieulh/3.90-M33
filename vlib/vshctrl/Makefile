PSPSDK=$(shell psp-config --pspsdk-path)
VshCtrlLib_OBJS = VshCtrlLib_0000.o VshCtrlLib_0001.o VshCtrlLib_0002.o 
%.o: VshCtrlLib.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(VshCtrlLib_OBJS)
	psp-ar cru libpspvshctrl.a $(VshCtrlLib_OBJS)
	psp-ranlib libpspvshctrl.a

clean:
	rm -f *.o
	rm -f *.a
