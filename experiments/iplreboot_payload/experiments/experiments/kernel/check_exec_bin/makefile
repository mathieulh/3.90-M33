PSPSDK=$(shell psp-config --pspsdk-path)
PSPDEV=$(shell psp-config --pspdev-path)
INCLUDE=$(PSPSDK)/include

all:    clean checkexec.bin

clean:
	rm -rf *~ *.o *.elf *.bin *.s *.bin

CC       = psp-gcc
CFLAGS   := -D PSP -I $(INCLUDE) -W -Wall -O0 -G0 -fno-pic -mno-abicalls -w -fomit-frame-pointer

checkexec.o: CheckExec.c
	$(CC) $(CFLAGS) -S CheckExec.c -o checkexec.s
	$(PSPDEV)/bin/psp-as checkexec.s -o checkexec.o

checkexec.bin: checkexec.o linktobin.x
	$(PSPDEV)/bin/psp-ld -T linktobin.x -L$(PSPSDK)/lib checkexec.o -lpsplibc -o checkexec.elf
	$(PSPDEV)/bin/psp-strip -s checkexec.elf
	$(PSPDEV)/bin/psp-objcopy -O binary checkexec.elf checkexec.bin

