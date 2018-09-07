#ifndef INITIAL_H
#define INITIAL_H

#define NDEVICES (DEV_PER_INT * (DEV_USED_INTS + 1))

typedef unsigned int memaddr;
typedef unsigned int cpu_t;

extern void test();
void newArea(memaddr address, void handler());

#endif