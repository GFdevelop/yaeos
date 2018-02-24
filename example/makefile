include ../makefile.inc

all: $(EXE)
	echo "$(EXE)		 [OK]"

$(EXE): $(EXE).o $(LIB_DIR)pcb.o $(LIB_DIR)asl.o
	$(LD) -T $(ELFSCRIPT) -o $(EXE) $(SYSLIB) $^ -I $(INC_DIR) -I ../include

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@ -I $(INC_DIR) -I ../include

clean:
	-rm *.o $(EXE) $(LIB_DIR)*.o

.PHONY: all clean
