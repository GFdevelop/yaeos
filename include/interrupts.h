#ifndef INTERRUPTS_H
#define INTERRUPTS_H

void intHandler();
void timer_HDL();
void device_HDL();
void terminal_HDL();
unsigned int findLineNo(unsigned int device);
void SVST(state_t *A, state_t *B);

#endif
