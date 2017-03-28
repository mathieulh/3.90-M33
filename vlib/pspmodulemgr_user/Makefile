PSPSDK=$(shell psp-config --pspsdk-path)
ModuleMgrForUser_OBJS = ModuleMgrForUser_0000.o ModuleMgrForUser_0001.o ModuleMgrForUser_0002.o ModuleMgrForUser_0003.o ModuleMgrForUser_0004.o ModuleMgrForUser_0005.o ModuleMgrForUser_0006.o ModuleMgrForUser_0007.o ModuleMgrForUser_0008.o ModuleMgrForUser_0009.o ModuleMgrForUser_0010.o ModuleMgrForUser_0011.o ModuleMgrForUser_0012.o ModuleMgrForUser_0013.o ModuleMgrForUser_0014.o ModuleMgrForUser_0015.o ModuleMgrForUser_0016.o ModuleMgrForUser_0017.o ModuleMgrForUser_0018.o ModuleMgrForUser_0019.o ModuleMgrForUser_0020.o 


%.o: ModuleMgrForUser.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(ModuleMgrForUser_OBJS)
	psp-ar cru libpspmodulemgr_user.a $(ModuleMgrForUser_OBJS)
	psp-ranlib libpspmodulemgr_user.a

clean:
	rm -f *.o
	rm -f *.a
