#ifndef SCHEDULER_H
#define SCHEDULER_H

#define SLICE_TIME 3000
#define AGING_TIME 10000
#define TICK_TIME 100000

void scheduler();
void ager(pcb_t *readyPCB, void *args);

#endif
