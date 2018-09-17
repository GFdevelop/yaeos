#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "pcb.h"
#include "initial.h"

#define TIME_SLICE 3000UL
#define AGING_TIME 10000UL
#define PSEUDO_TIME 100000UL

void scheduler();
cpu_t nextSlice();

#endif