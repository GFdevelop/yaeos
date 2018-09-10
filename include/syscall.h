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

//~ int SYSCALL(CREATEPROCESS, state t *statep, int priority, void **cpid);
//~ int SYSCALL(TERMINATEPROCESS, void * pid);
//~ void SYSCALL(SEMP, int *semaddr);
//~ void SYSCALL(SEMV, int *semaddr);
//~ int SYSCALL(SPECHDL, int type, state_t *old, state_t *new);
//~ void SYSCALL(GETTIME, cputime_t *user, cputime_t *kernel, cputime_t *wallclock);
//~ void SYSCALL(WAITCLOCK);
//~ unsigned int SYSCALL(IODEVOP, unsigned int command, unsigned int *comm_device_register);
//~ void SYSCALL(GETPIDS, void **pid, void **ppid);
//~ void SYSCALL(WAITCHLD);

int createprocess();
int terminateprocess();
void semv(unsigned int a2);
void semp(unsigned int a2);
int spechdl();
void gettime();
void waitclock();
void iodevop(unsigned int a2, unsigned int a3);
void getpids();

#endif
