PSPSDK=$(shell psp-config --pspsdk-path)
IoFileMgrForKernel_OBJS = IoFileMgrForKernel_0000.o IoFileMgrForKernel_0001.o IoFileMgrForKernel_0002.o IoFileMgrForKernel_0003.o IoFileMgrForKernel_0004.o IoFileMgrForKernel_0005.o IoFileMgrForKernel_0006.o IoFileMgrForKernel_0007.o IoFileMgrForKernel_0008.o IoFileMgrForKernel_0009.o IoFileMgrForKernel_0010.o IoFileMgrForKernel_0011.o IoFileMgrForKernel_0012.o IoFileMgrForKernel_0013.o IoFileMgrForKernel_0014.o IoFileMgrForKernel_0015.o IoFileMgrForKernel_0016.o IoFileMgrForKernel_0017.o IoFileMgrForKernel_0018.o IoFileMgrForKernel_0019.o IoFileMgrForKernel_0020.o IoFileMgrForKernel_0021.o IoFileMgrForKernel_0022.o IoFileMgrForKernel_0023.o IoFileMgrForKernel_0024.o IoFileMgrForKernel_0025.o IoFileMgrForKernel_0026.o IoFileMgrForKernel_0027.o IoFileMgrForKernel_0028.o IoFileMgrForKernel_0029.o IoFileMgrForKernel_0030.o IoFileMgrForKernel_0031.o IoFileMgrForKernel_0032.o IoFileMgrForKernel_0033.o IoFileMgrForKernel_0034.o IoFileMgrForKernel_0035.o IoFileMgrForKernel_0036.o IoFileMgrForKernel_0037.o IoFileMgrForKernel_0038.o IoFileMgrForKernel_0039.o IoFileMgrForKernel_0040.o IoFileMgrForKernel_0041.o IoFileMgrForKernel_0042.o IoFileMgrForKernel_0043.o IoFileMgrForKernel_0044.o IoFileMgrForKernel_0045.o IoFileMgrForKernel_0046.o IoFileMgrForKernel_0047.o IoFileMgrForKernel_0048.o IoFileMgrForKernel_0049.o 

%.o: IoFileMgrForKernel.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(IoFileMgrForKernel_OBJS)
	psp-ar cru libpspiofilemgr_kernel.a $(IoFileMgrForKernel_OBJS)
	psp-ranlib libpspiofilemgr_kernel.a

clean:
	rm -f *.o
	rm -f *.a
