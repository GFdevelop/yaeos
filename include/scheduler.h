#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "pcb.h"

#define TIME_SLICE 3000
#define AGING_TIME 10000
#define PSEUDO_TIME 100000

void scheduler();
unsigned int nextSlice();
void ager(pcb_t *, void *);

#endif