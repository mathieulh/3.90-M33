PSPSDK=$(shell psp-config --pspsdk-path)
SysMemUserForUser_OBJS = SysMemUserForUser_0000.o SysMemUserForUser_0001.o SysMemUserForUser_0002.o SysMemUserForUser_0003.o SysMemUserForUser_0004.o SysMemUserForUser_0005.o SysMemUserForUser_0006.o SysMemUserForUser_0007.o SysMemUserForUser_0008.o SysMemUserForUser_0009.o SysMemUserForUser_0010.o SysMemUserForUser_0011.o SysMemUserForUser_0012.o SysMemUserForUser_0013.o SysMemUserForUser_0014.o SysMemUserForUser_0015.o SysMemUserForUser_0016.o SysMemUserForUser_0017.o SysMemUserForUser_0018.o 
%.o: SysMemUserForUser.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(SysMemUserForUser_OBJS)
	psp-ar cru libpspsysmem_user.a $(SysMemUserForUser_OBJS)
	psp-ranlib libpspsysmem_user.a

clean:
	rm -f *.o
	rm -f *.a
