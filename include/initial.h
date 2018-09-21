#ifndef INITIAL_H
#define INITIAL_H

#include "pcb.h"
#include <arch.h>

//MACRO defined for obtain line number from a device address
#define LINE_NO(addr) (((addr - DEV_REG_START) / DEV_REGBLOCK_SIZE ) + DEV_IL_START -1 )
//#define LINE_NO(addr, dev) (((addr - DEV_REG_START - (dev * DEV_REG_SIZE)) / (N_DEV_PER_IL * DEV_REG_SIZE)) + DEV_IL_START)

unsigned int aging_times;

typedef unsigned int memaddr;
typedef unsigned int cpu_t;

extern void test();
void newArea(memaddr address, void handler());

pcb_t *readyQueue, *currentPCB;
unsigned int processCount, softBlock;
unsigned int isPseudo, isAging;								//Flag vars used to manage time
int semDev[MAX_DEVICES];
cpu_t lastAging, lastPseudo, curProc_start, kernel_start;	//Timestamps for time accounting

#endif
