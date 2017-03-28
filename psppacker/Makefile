OUTPUT=psppacker
OBJS=main.o
CFLAGS=-Wall -I.
LDFLAGS=-L.
LIBS = -lz

all: $(OUTPUT)

clean:
	rm -f $(OUTPUT) *.o

install:
	cp psppacker /usr/local/bin/psppacker

$(OUTPUT): $(OBJS)
	$(LINK.c) $(LDFLAGS) -o $@ $^ $(LIBS)
