#ifndef SYSCALL_H
#define SYSCALL_H

#define CREATEPROCESS 1
#define TERMINATEPROCESS 2
#define SEMV 3
#define SEMP 4
#define SPECHDL 5
#define GETTIME 6
#define WAITCLOCK 7
#define IODEVOP 8
#define GETPIDS 9
#define WAITCHLD 10

#define SPECSYSBP 0
#define SPECTLB 1
#define SPECPGMT 2

void createprocess();
void terminateprocess();
void semv();
void semp();
void spechdl();
void gettime();
void waitclock();
void iodevop();
void getpids();
void waitchild();

#endif
