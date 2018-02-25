include ../makefile.inc

all: $(EXE)

p1test: p1test.o ../src/pcb.o ../src/asl.o
# "$^" names of all the prerequisites
	$(LD) -T $(ELFSCRIPT) -o $@ $(SYSLIB) $^ -I $(INC_DIR) -I ../include
	echo [OK] $@

clean:
	-rm -f *.o $(EXE)

help:
	echo "OPTIONS:   [all][p1test][clean][help]"
	echo
	echo "[all][p1test]    for make executable"
	echo "[clean]          remove *.o and executable"
	echo "[help]           show this message"
	echo

.PHONY: all clean help
