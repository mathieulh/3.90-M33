PSPSDK=$(shell psp-config --pspsdk-path)
SysMemForKernel_OBJS = SysMemForKernel_0000.o SysMemForKernel_0001.o SysMemForKernel_0002.o SysMemForKernel_0003.o SysMemForKernel_0004.o SysMemForKernel_0005.o SysMemForKernel_0006.o SysMemForKernel_0007.o SysMemForKernel_0008.o SysMemForKernel_0009.o SysMemForKernel_0010.o SysMemForKernel_0011.o SysMemForKernel_0012.o SysMemForKernel_0013.o SysMemForKernel_0014.o SysMemForKernel_0015.o SysMemForKernel_0016.o SysMemForKernel_0017.o SysMemForKernel_0018.o SysMemForKernel_0019.o SysMemForKernel_0020.o SysMemForKernel_0021.o SysMemForKernel_0022.o SysMemForKernel_0023.o SysMemForKernel_0024.o SysMemForKernel_0025.o SysMemForKernel_0026.o SysMemForKernel_0027.o SysMemForKernel_0028.o SysMemForKernel_0029.o SysMemForKernel_0030.o SysMemForKernel_0031.o SysMemForKernel_0032.o SysMemForKernel_0033.o SysMemForKernel_0034.o SysMemForKernel_0035.o SysMemForKernel_0036.o SysMemForKernel_0037.o SysMemForKernel_0038.o SysMemForKernel_0039.o SysMemForKernel_0040.o SysMemForKernel_0041.o SysMemForKernel_0042.o SysMemForKernel_0043.o SysMemForKernel_0044.o SysMemForKernel_0045.o SysMemForKernel_0046.o SysMemForKernel_0047.o SysMemForKernel_0048.o SysMemForKernel_0049.o SysMemForKernel_0050.o SysMemForKernel_0051.o SysMemForKernel_0052.o SysMemForKernel_0053.o SysMemForKernel_0054.o SysMemForKernel_0055.o SysMemForKernel_0056.o SysMemForKernel_0057.o SysMemForKernel_0058.o SysMemForKernel_0059.o SysMemForKernel_0060.o SysMemForKernel_0061.o SysMemForKernel_0062.o SysMemForKernel_0063.o SysMemForKernel_0064.o SysMemForKernel_0065.o SysMemForKernel_0066.o SysMemForKernel_0067.o SysMemForKernel_0068.o SysMemForKernel_0069.o SysMemForKernel_0070.o SysMemForKernel_0071.o SysMemForKernel_0072.o SysMemForKernel_0073.o SysMemForKernel_0074.o SysMemForKernel_0075.o SysMemForKernel_0076.o SysMemForKernel_0077.o SysMemForKernel_0078.o SysMemForKernel_0079.o SysMemForKernel_0080.o SysMemForKernel_0081.o SysMemForKernel_0082.o SysMemForKernel_0083.o SysMemForKernel_0084.o SysMemForKernel_0085.o SysMemForKernel_0086.o SysMemForKernel_0087.o SysMemForKernel_0088.o SysMemForKernel_0089.o SysMemForKernel_0090.o SysMemForKernel_0091.o SysMemForKernel_0092.o SysMemForKernel_0093.o SysMemForKernel_0094.o SysMemForKernel_0095.o SysMemForKernel_0096.o SysMemForKernel_0097.o SysMemForKernel_0098.o SysMemForKernel_0099.o SysMemForKernel_0100.o 


%.o: SysMemForKernel.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(SysMemForKernel_OBJS)
	psp-ar cru libpspsysmem_kernel.a $(SysMemForKernel_OBJS)
	psp-ranlib libpspsysmem_kernel.a

clean:
	rm -f *.o
	rm -f *.a
