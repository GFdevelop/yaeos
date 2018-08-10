#ifndef SCHEDULER_H
#define SCHEDULER_H

 
#define PRIO_IDLE 0 
#define PRIO_LOW 1
#define PRIO_NORM 2
#define PRIO_HIGH 3

#define TIME_SLICE 3
#define AGE_TIME 10
#define PSEUDO_TIME 100

void scheduler();

#endif