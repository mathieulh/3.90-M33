# Type 'make' to compile only main.c to produce a.bin
# Type 'make loader2.png' to generate wallpaper PNG
# Type 'make loader2.tif' to generate TIF exploit image


PSPDEV=/usr/local/pspdev/bin
INCLUDES=-I $(PSPDEV)/../psp/sdk/include

#all: loader.bin a.bin
#all: loader.bin a.bin copied
#	cp a.bin GTA.bin.mips
#	cp GTA.bin.base GTA.bin	
#	cat GTA.bin.mips >> GTA.bin
#	cp GTA.bin f:\

# fanj-all is the default target.  Builds the 3 flavours of the exploit,
# and copies them to the PSP memstick mounted at /psp/
# From there, they have to be encrypted using the savedata sample on a v2.0 TIFF
fanj-all: clean GTAUS.BIN GTAUK.BIN GTADE.BIN
	cp *.BIN /psp/

ditlew: GTAUS.BIN ditlew-copy clean
ditlew-copy:
	cp GTAUS.BIN f:\GTA.bin

ditlew2: GTAUS.BIN ditlew-copy2 clean
ditlew-copy2:
	cp GTAUS.BIN e:\upload\GTA.bin

a.bin:  loader.c
	$(PSPDEV)/psp-gcc $(INCLUDES) -O2 -W -Wall -G0 -fno-pic -mno-abicalls -w -S loader.c -o loader.s
	$(PSPDEV)/psp-as loader.s -o loader.o
	$(PSPDEV)/psp-ld -T psp20bin.x loader.o -o loader.elf
	$(PSPDEV)/psp-strip -s loader.elf
	$(PSPDEV)/psp-objcopy -O binary loader.elf a.bin

GTAUS.BIN: hack.exe a.bin
	./hack.exe GTAUS.BIN.virgin a.bin GTAUS.BIN
	
GTAUK.BIN: hack.exe a.bin
	./hack.exe GTAUK.BIN.virgin a.bin GTAUK.BIN
	
GTADE.BIN: hack.exe a.bin
	./hack.exe GTADE.BIN.virgin a.bin GTADE.BIN
	
GTAJP.BIN: hack.exe a.bin
	./hack.exe GTAJP.BIN.virgin a.bin GTAJP.BIN
	
hack.exe:  hack_any.c
	gcc hack_any.c -o hack.exe

fanj-copy:  clean GTAUK.BIN
	cp GTAUK.BIN /psp/

setdate:
	touch -t 200303150000 /psp/psp/savedata/ULES00151S5
	touch -t 200303150000 /psp/psp/savedata/ULES00182S5
	touch -t 200303150000 /psp/psp/savedata/ULUS10041S5

clean:
	rm -rf *~ *.o *.s *.elf *.bin *.exe *.BIN

