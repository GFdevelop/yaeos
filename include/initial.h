#ifndef INITIAL_H
#define INITIAL_H

typedef unsigned int memaddr;
typedef unsigned int cpu_t;

extern void test();
void newArea(memaddr address, void handler());

#endif