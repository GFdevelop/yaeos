SHELL = /bin/sh
PROJ = yaeos
VERSION = 1.0.0
ARCH = arm7tdmi
SRCTAR = $(PROJ)-$(VERSION)-src
RELTAR = $(PROJ)-$(VERSION)-$(ARCH)
EXE = p1test
SRC = pcb asl
LIBS = $(addprefix lib,$(SRC))
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
AR = arm-none-eabi-ar
CFLAGS = -mcpu=$(ARCH) -Wall
SYSLIB = /usr/include/uarm/crtso.o /usr/include/uarm/libuarm.o
ELFSCRIPT = /usr/include/uarm/ldscripts/elf32ltsarm.h.uarmcore.x
INC_DIR = /usr/include/uarm


# don't print recipe (alternative use "@" before command)
.SILENT:

# don't remove intermediate files
.SECONDARY:

# delete target if exit nonzero
.DELETE_ON_ERROR:

# ignore error (alternative use "-" before command)
.IGNORE: clean


# prerequisite name is string before ".o", ex. pcb.o:pcb.c
%.o : %.c
# "$@" file name of the target of the rule
	$(CC) $(CFLAGS) -c $< -o $@ -I $(INC_DIR) -I ../include
