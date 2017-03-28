OUTPUT=msinst
OBJS=main.o fileio.o
CFLAGS=-Wall -I.
LDFLAGS=-L.

all: $(OUTPUT)

clean:
	rm -f $(OUTPUT) *.o

$(OUTPUT): $(OBJS)
	$(LINK.c) $(LDFLAGS) -o $@ $^ $(LIBS)
