#ifndef MAIN_H
#define MAIN_H


#define MAX_PCB_PRIORITY		10
#define MIN_PCB_PRIORITY		0
#define DEFAULT_PCB_PRIORITY		5

typedef unsigned int memaddr;
typedef unsigned int cpu_t;

extern void test();
void newArea(unsigned int address, void handler());


#endif
