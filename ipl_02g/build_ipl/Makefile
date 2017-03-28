OUTPUT=build_ipl
OBJS=main.o 
CFLAGS=-Wall -I.
LDFLAGS=-L.

all: $(OUTPUT)

clean:
	rm -f $(OUTPUT) *.o

$(OUTPUT): $(OBJS)
	$(LINK.c) $(LDFLAGS) -o $@ $^ $(LIBS)
