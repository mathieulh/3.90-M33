OUTPUT=bin2elf
OBJS=main.o
CFLAGS=-Wall -I.
LDFLAGS=-L.
LIBS = -lz

all: $(OUTPUT)

clean:
	rm -f $(OUTPUT) *.o
$(OUTPUT): $(OBJS)
	$(LINK.c) $(LDFLAGS) -o $@ $^ $(LIBS)
