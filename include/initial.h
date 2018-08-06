#ifndef INITIAL_H
#define INITIAL_H


#define PRIO_IDLE	0
#define PRIO_LOW	1
#define PRIO_NORM	2
#define PRIO_HIGH	3

typedef unsigned int memaddr;
typedef unsigned int cpu_t;

extern void test();
void newArea(unsigned int address, void handler());


#endif
