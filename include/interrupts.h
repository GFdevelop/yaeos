#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#define TRANSM 0
#define RECV 1

#include <uARMtypes.h>
#include "asl.h"
#include "initial.h"

void INT_handler();
void timer_HDL();
void device_HDL(unsigned int);
void terminal_HDL();

void SVST(state_t*, state_t*);
unsigned int instanceNo(int);
void sendACK(termreg_t*, int, int);
void pseudo_clock();
void ager(pcb_t*, void *);

extern pcb_t *currentPCB, *readyQueue;
extern unsigned int softBlock, kernel_start, isPseudo, isAging;
extern cpu_t lastPseudo, lastAging;
extern int semDev[MAX_DEVICES];

#endif