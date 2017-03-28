OUTPUT=bootcnf
OBJS = main.o BootConfig.o BootMode.o ModuleEntry.o

CXXFLAGS=
LIBS=

all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	$(CXX) -o $@ $^ $(LIBS)

clean:
	rm -f $(OUTPUT) *.o

