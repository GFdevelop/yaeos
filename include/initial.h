#ifndef INITIAL_H
#define INITIAL_H

#define LINENO(addr,dev)(((addr-DEV_REG_START-((dev) * DEV_REG_SIZE))/(N_DEV_PER_IL * DEV_REG_SIZE))+DEV_IL_START)

typedef unsigned int memaddr;
typedef unsigned int cpu_t;

extern void test();
void newArea(memaddr address, void handler());

#endif
