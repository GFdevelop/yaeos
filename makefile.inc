CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
AR = arm-none-eabi-ar
CFLAGS = -mcpu=arm7tdmi -Wall
SYSLIB = /usr/include/uarm/crtso.o /usr/include/uarm/libuarm.o
ELFSCRIPT = /usr/include/uarm/ldscripts/elf32ltsarm.h.uarmcore.x
INC_DIR = /usr/include/uarm
LIB_DIR=../src/
BIN_DIR = ../bin/
EXE=p1test

SHELL	= /bin/sh
#.SILENT :
