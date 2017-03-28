PSPSDK=$(shell psp-config --pspsdk-path)
SEMAPHORE_OBJS = semaphore_0000.o semaphore_0001.o semaphore_0002.o semaphore_0003.o semaphore_0004.o 

%.o: semaphore.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(SEMAPHORE_OBJS)
	psp-ar cru libpspsemaphore.a $(SEMAPHORE_OBJS)
	psp-ranlib libpspsemaphore.a

clean:
	rm -f *.o
	rm -f *.a
