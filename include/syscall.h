#ifndef SYSCALL_H
#define SYSCALL_H

#define CREATEPROCESS 1
#define TERMINATEPROCESS 2
#define SEMP 3
#define SEMV 4
#define SPECHDL 5
#define GETTIME 6
#define WAITCLOCK 7
#define IODEVOP 8
#define GETPIDS 9
#define WAITCHLD 10

int SYSBK_handler();

int createprocess();
int terminateprocess();
void semv();
void semp();
int spechdl();
void gettime();
void waitclock();
unsigned int iodevop();
void getpids();
void waitchld();

#endif
