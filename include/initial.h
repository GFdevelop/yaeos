#ifndef INITIAL_H
#define INITIAL_H

#define DEV_REAL_ADDR(addr)	( addr + ( DEV_IL_START * DEV_REGBLOCK_SIZE ))
#define DEV_ADDR_SIZE(addr)	( DEV_REAL_ADDR(addr) - DEV_REG_START )
#define LINENO(addr)		( DEV_ADDR_SIZE(addr) / DEV_REGBLOCK_SIZE )
#define DEVICENO(addr)		(( DEV_ADDR_SIZE(addr) % DEV_REGBLOCK_SIZE ) / DEV_REG_SIZE )
#define TERMNO(addr)		(( DEV_ADDR_SIZE(addr) % DEV_REGBLOCK_SIZE ) / ( DEV_REG_SIZE / 2 ))
#define INDEVNO(termno)		( termno / 2 )
#define TERMTYPE(termno)	( termno % 2 )

typedef unsigned int memaddr;
typedef unsigned int cpu_t;

extern void test();
void newArea(memaddr address, void handler());

#endif
