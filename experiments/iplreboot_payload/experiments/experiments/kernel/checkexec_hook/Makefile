PSPDEV=/usr/local/pspdev/bin
INCLUDES=-I $(PSPDEV)/../psp/sdk/include

all:	z.bin

z.bin: 
	$(PSPDEV)/psp-as syscalls.s -o syscalls.o
	$(PSPDEV)/psp-gcc $(INCLUDES) -W -Wall -G0 -fno-pic -mno-abicalls -w -S main.c -o main.s
	$(PSPDEV)/psp-as main.s -o main.o
	$(PSPDEV)/psp-gcc $(INCLUDES) -W -Wall -G0 -fno-pic -mno-abicalls -w -S minilibc.c -o minilibc.s
	$(PSPDEV)/psp-as minilibc.s -o minilibc.o

	$(PSPDEV)/psp-ld -T linkfile.l syscalls.o main.o -o main.elf
	$(PSPDEV)/psp-strip -s main.elf
	$(PSPDEV)/psp-objcopy -O binary main.elf z.bin

clean:
	rm -rf *~ *.o *.elf *.bin
