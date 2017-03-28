PSPSDK=$(shell psp-config --pspsdk-path)
scePower_driver_OBJS = scePower_driver_0000.o scePower_driver_0001.o scePower_driver_0002.o scePower_driver_0003.o scePower_driver_0004.o scePower_driver_0005.o scePower_driver_0006.o scePower_driver_0007.o scePower_driver_0008.o scePower_driver_0009.o scePower_driver_0010.o scePower_driver_0011.o scePower_driver_0012.o scePower_driver_0013.o scePower_driver_0014.o scePower_driver_0015.o scePower_driver_0016.o scePower_driver_0017.o scePower_driver_0018.o scePower_driver_0019.o scePower_driver_0020.o scePower_driver_0021.o scePower_driver_0022.o scePower_driver_0023.o scePower_driver_0024.o scePower_driver_0025.o scePower_driver_0026.o scePower_driver_0027.o scePower_driver_0028.o scePower_driver_0029.o scePower_driver_0030.o scePower_driver_0031.o scePower_driver_0032.o scePower_driver_0033.o scePower_driver_0034.o scePower_driver_0035.o scePower_driver_0036.o scePower_driver_0037.o scePower_driver_0038.o scePower_driver_0039.o scePower_driver_0040.o scePower_driver_0041.o scePower_driver_0042.o scePower_driver_0043.o scePower_driver_0044.o scePower_driver_0045.o scePower_driver_0046.o scePower_driver_0047.o scePower_driver_0048.o scePower_driver_0049.o scePower_driver_0050.o scePower_driver_0051.o scePower_driver_0052.o scePower_driver_0053.o scePower_driver_0054.o scePower_driver_0055.o scePower_driver_0056.o scePower_driver_0057.o scePower_driver_0058.o scePower_driver_0059.o scePower_driver_0060.o scePower_driver_0061.o scePower_driver_0062.o scePower_driver_0063.o scePower_driver_0064.o scePower_driver_0065.o scePower_driver_0066.o scePower_driver_0067.o scePower_driver_0068.o scePower_driver_0069.o scePower_driver_0070.o scePower_driver_0071.o scePower_driver_0072.o scePower_driver_0073.o scePower_driver_0074.o scePower_driver_0075.o scePower_driver_0076.o scePower_driver_0077.o scePower_driver_0078.o scePower_driver_0079.o scePower_driver_0080.o scePower_driver_0081.o scePower_driver_0082.o scePower_driver_0083.o scePower_driver_0084.o scePower_driver_0085.o scePower_driver_0086.o scePower_driver_0087.o scePower_driver_0088.o scePower_driver_0089.o 
%.o: scePower_driver.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(scePower_driver_OBJS)
	psp-ar cru libpsppower_driver.a $(scePower_driver_OBJS)
	psp-ranlib libpsppower_driver.a

clean:
	rm -f *.o
	rm -f *.a
