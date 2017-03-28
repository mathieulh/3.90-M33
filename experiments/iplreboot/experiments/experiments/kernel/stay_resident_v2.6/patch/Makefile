PSPDEV=/usr/local/pspdev/bin
INCLUDES=-I $(PSPDEV)/../psp/sdk/include

all:	a.bin

a.bin: 
	$(PSPDEV)/psp-as main.s -o main.o
	$(PSPDEV)/psp-gcc $(INCLUDES) -W -Wall -G0 -fno-pic -mno-abicalls -w -S nokxploit.c -o nokxploit.s
	$(PSPDEV)/psp-as nokxploit.s -o nokxploit.o
	$(PSPDEV)/psp-gcc $(INCLUDES) -W -Wall -g -G0 -fno-pic -mno-abicalls -w -S minilibc.c -o minilibc.s
	$(PSPDEV)/psp-as minilibc.s -o minilibc.o

	$(PSPDEV)/psp-ld -T linkfile.l main.o nokxploit.o minilibc.o -o main.elf
	$(PSPDEV)/psp-strip -s main.elf
	$(PSPDEV)/psp-objcopy -O binary main.elf patch.bin

clean:
	rm -rf *~ *.o *.elf *.bin *.bin
