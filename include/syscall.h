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


#define SPECNEW 3
#define SPECSYSBP 0
#define SPECTLB 1
#define SPECPGMT 2


#define DEV_REAL_ADDR(addr)	( addr + ( DEV_IL_START * DEV_REGBLOCK_SIZE ))
#define DEV_ADDR_SIZE(addr)	( DEV_REAL_ADDR(addr) - DEV_REG_START )
#define LINENO(addr)		( DEV_ADDR_SIZE(addr) / DEV_REGBLOCK_SIZE )
#define DEVICENO(addr)		(( DEV_ADDR_SIZE(addr) % DEV_REGBLOCK_SIZE ) / DEV_REG_SIZE )
#define TERMNO(addr)		(( DEV_ADDR_SIZE(addr) % DEV_REGBLOCK_SIZE ) / ( DEV_REG_SIZE / 2 ))
#define INDEVNO(termno)		( termno / 2 )
#define TERMTYPE(termno)	( termno % 2 )


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
