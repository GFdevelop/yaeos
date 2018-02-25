include ../makefile.inc

all: $(EXE)

p1test: p1test.o ../src/pcb.o ../src/asl.o
# "$^" names of all the prerequisites
	$(LD) -T $(ELFSCRIPT) -o $@ $(SYSLIB) $^ -I $(INC_DIR) -I ../include
	echo [OK] $@

clean:
	-rm -f *.o $(EXE)

.PHONY: all clean
