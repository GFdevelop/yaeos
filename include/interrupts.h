#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <uARMtypes.h>

#define RSTAT 0x0
#define RCMD 0x4
#define TSTAT 0x8
#define TCMD 0xC

void INT_handler();
void timer_HDL();
void device_HDL(unsigned int device);
void terminal_HDL();

void SVST(state_t *A, state_t *B);

state_t *INT_Old;

#endif