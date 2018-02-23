SYSLIB = /usr/include/uarm/crtso.o /usr/include/uarm/libuarm.o
ELFSCRIPT = /usr/include/uarm/ldscripts/elf32ltsarm.h.uarmcore.x
INC_DIR = /usr/include/uarm
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
CFLAGS = -mcpu=arm7tdmi -Wall
LIB_DIR=../src/

p1test: p1test.o $(LIB_DIR)pcb.o $(LIB_DIR)asl.o
	$(LD) -T $(ELFSCRIPT) -o $@ $(SYSLIB) $^ -I $(INC_DIR) -I . -I ../include

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@ -I $(INC_DIR) -I . -I ../include

clean:
	rm *.o p1test
