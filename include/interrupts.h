#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <uARMtypes.h>

#define RSTAT 0x00
#define RCMD 0x04
#define TSTAT 0x08
#define TCMD 0x0C

void INT_handler();
void timer_HDL();
void device_HDL(unsigned int device);
void terminal_HDL();

void SVST(state_t *A, state_t *B);

state_t *INT_Old;

#endif