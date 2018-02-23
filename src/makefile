INC_DIR = /usr/include/uarm
CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
CFLAGS = -mcpu=arm7tdmi -Wall

all: pcb.o asl.o

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@ -I $(INC_DIR) -I . -I ../include

clean:
	rm *.o *.so *.a
