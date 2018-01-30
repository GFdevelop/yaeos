INC_DIR = /usr/include/uarm
CFLAGS = -fPIC
CPU = -mcpu=arm7tdmi
BIN_DIR = ../bin/

SYSLIB = /usr/include/uarm/crtso.o /usr/include/uarm/libuarm.o
ELFSCRIPT = /usr/include/uarm/ldscripts/elf32ltsarm.h.uarmcore.x

CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
AR = arm-none-eabi-ar

phase1: pcb.o p1test.o
	$(LD) -T $(ELFSCRIPT) -o phase1 $(SYSLIB) p1test.o pcb.o -I $(INC_DIR) -I .

p1test.o: p1test.c
	$(CC) $(CPU) -c p1test.c -o p1test.o -I $(INC_DIR) -I .

pcb.o: pcb.c pcb.h const.h
	$(CC) $(CPU) $(CFLAGS) -c pcb.c -o pcb.o -I $(INC_DIR) -I .
