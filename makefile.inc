SHELL = /bin/sh
PROJ = yaeos
VERSION = 1.2.1a
ARCH = arm7tdmi
SRCTAR = $(PROJ)-$(VERSION)-src
RELTAR = $(PROJ)-$(VERSION)-$(ARCH)
EXE = p1test p2test
SRC = pcb asl
LIBS = $(addprefix lib,$(SRC))
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
AR = arm-none-eabi-ar
CFLAGS = -mcpu=$(ARCH) -Wall -std=gnu11
ifeq ($(shell uname),Darwin)
	INC_DIR = /usr/local/include/uarm
else
	INC_DIR = /usr/include/uarm
endif
SYSLIB = $(INC_DIR)/crtso.o $(INC_DIR)/libuarm.o
ELFSCRIPT = $(INC_DIR)/ldscripts/elf32ltsarm.h.uarmcore.x


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
