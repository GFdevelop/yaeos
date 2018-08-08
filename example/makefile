include ../makefile.inc

all: $(EXE)

p1test: p1test.o ../src/pcb.o ../src/asl.o
# "$^" names of all the prerequisites
	$(LD) -T $(ELFSCRIPT) -o $@ $(SYSLIB) $^ -I $(INC_DIR) -I ../include
	echo [OK] $@

p2test: p2test.04 ../src/initial.o ../src/pcb.o ../src/asl.o ../src/interrupts.o ../src/exceptions.o ../src/syscall.o ../src/scheduler.o
	$(LD) -T $(ELFSCRIPT) -o $@ $(SYSLIB) $^ $(INC_DIR)/libdiv.o -I $(INC_DIR) -I ../include
	echo [OK] $@

p2test.04: p2test.04.c
	$(CC) $(CFLAGS) -c $< -o $@ -DQPAGE=FRAMESIZE -include arch.h -include syscall.h -I $(INC_DIR) -I ../include

clean:
	-rm -f *.o $(EXE) p2test.04

help:
	echo "OPTIONS:   [all][p1test][clean][help]"
	echo
	echo "[all][p1test]    for make executable"
	echo "[clean]          remove *.o and executable"
	echo "[help]           show this message"
	echo

.PHONY: all clean help
