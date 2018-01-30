noinst_LIBRARIES = libmikabooq.a
libmikabooq_a_SOURCES = mikabooq.c
libmikabooq_a_CFLAGS = -mcpu=arm7tdmi -fPIC -I /usr/include/uarm -I .


noinst_PROGRAMS = p1test.elf
p1test_elf_SOURCES = p1test.c
p1test_elf_CFLAGS = -mcpu=arm7tdmi -include stdint.h -I /usr/include/uarm -I .
p1test_elf_LDADD = mikabooq.h
