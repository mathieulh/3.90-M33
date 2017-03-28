PSPSDK=$(shell psp-config --pspsdk-path)
ModuleMgrForKernel_OBJS = ModuleMgrForKernel_0000.o ModuleMgrForKernel_0001.o ModuleMgrForKernel_0002.o ModuleMgrForKernel_0003.o ModuleMgrForKernel_0004.o ModuleMgrForKernel_0005.o ModuleMgrForKernel_0006.o ModuleMgrForKernel_0007.o ModuleMgrForKernel_0008.o ModuleMgrForKernel_0009.o ModuleMgrForKernel_0010.o ModuleMgrForKernel_0011.o ModuleMgrForKernel_0012.o ModuleMgrForKernel_0013.o ModuleMgrForKernel_0014.o ModuleMgrForKernel_0015.o ModuleMgrForKernel_0016.o ModuleMgrForKernel_0017.o ModuleMgrForKernel_0018.o ModuleMgrForKernel_0019.o ModuleMgrForKernel_0020.o ModuleMgrForKernel_0021.o ModuleMgrForKernel_0022.o ModuleMgrForKernel_0023.o ModuleMgrForKernel_0024.o ModuleMgrForKernel_0025.o ModuleMgrForKernel_0026.o ModuleMgrForKernel_0027.o ModuleMgrForKernel_0028.o ModuleMgrForKernel_0029.o ModuleMgrForKernel_0030.o ModuleMgrForKernel_0031.o ModuleMgrForKernel_0032.o ModuleMgrForKernel_0033.o ModuleMgrForKernel_0034.o ModuleMgrForKernel_0035.o ModuleMgrForKernel_0036.o ModuleMgrForKernel_0037.o ModuleMgrForKernel_0038.o ModuleMgrForKernel_0039.o ModuleMgrForKernel_0040.o ModuleMgrForKernel_0041.o ModuleMgrForKernel_0042.o ModuleMgrForKernel_0043.o ModuleMgrForKernel_0044.o ModuleMgrForKernel_0045.o 


%.o: ModuleMgrForKernel.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(ModuleMgrForKernel_OBJS)
	psp-ar cru libpspmodulemgr_kernel.a $(ModuleMgrForKernel_OBJS)
	psp-ranlib libpspmodulemgr_kernel.a

clean:
	rm -f *.o
	rm -f *.a
