#ifndef INITIAL_H
#define INITIAL_H


#define PRIO_IDLE	0
#define PRIO_LOW	1
#define PRIO_NORM	2
#define PRIO_HIGH	3

#define LINENO(addr)	(((addr - DEV_REG_START) / DEV_REGBLOCK_SIZE ) - 1 + DEV_IL_START)


typedef unsigned int memaddr;
typedef unsigned int cpu_t;

extern void test();
void newArea(memaddr address, void handler());


#endif
