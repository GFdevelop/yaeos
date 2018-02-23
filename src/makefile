include ../makefile.inc

all: pcb.o asl.o

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@ -I $(INC_DIR) -I ../include

clean:
	rm *.o *.so *.a
