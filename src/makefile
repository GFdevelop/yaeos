include ../makefile.inc

all: shared static

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@ -I $(INC_DIR) -I ../include

shared: pcb.so asl.so
	echo shared libraries [OK]

%.so: %.o
	$(CC) $(CPU) -shared -Wl,-soname,$@ -o $@ $<

static: $(addprefix lib,pcb.a asl.a)
	echo static libraries [OK]

%.a: $(%.o:lib%,%)
	$(AR) rcs $@ $<

clean:
	-rm *.o *.so *.a

.PHONY: all shared static clean
