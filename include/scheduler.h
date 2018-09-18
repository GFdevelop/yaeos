#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "pcb.h"
#include "initial.h"

#define TIME_SLICE 3000UL
#define AGING_TIME 10000UL
#define PSEUDO_TIME 100000UL

void scheduler();
cpu_t nextSlice();

extern pcb_t *readyQueue, *currentPCB;
extern unsigned int processCount, softBlock, isPseudo, isAging;
extern cpu_t curProc_start, kernel_start, lastPseudo, lastAging;

#endif