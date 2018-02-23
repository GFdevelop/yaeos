include ../makefile.inc

p1test: p1test.o $(LIB_DIR)pcb.o $(LIB_DIR)asl.o
	$(LD) -T $(ELFSCRIPT) -o $@ $(SYSLIB) $^ -I $(INC_DIR) -I ../include

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@ -I $(INC_DIR) -I ../include

clean:
	rm *.o p1test
