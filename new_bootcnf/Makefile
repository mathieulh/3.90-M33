OUTPUT=bootcnf
OBJS=main.o
CFLAGS=-Wall -I.
LDFLAGS=-L.
LIBS = -lmhash

all: $(OUTPUT)

clean:
	rm -f $(OUTPUT) *.o

$(OUTPUT): $(OBJS)
	$(LINK.c) $(LDFLAGS) -o $@ $^ $(LIBS)
