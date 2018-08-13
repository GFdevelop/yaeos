#ifndef SCHEDULER_H
#define SCHEDULER_H

 
#define PRIO_IDLE 0 
#define PRIO_LOW 1
#define PRIO_NORM 2
#define PRIO_HIGH 3

#define TIME_SLICE 3000
#define AGING_TIME 10000
#define PSEUDO_TIME 100000

void scheduler();
unsigned int selectSlice();
void ager();
void pseudo_clock();

#endif