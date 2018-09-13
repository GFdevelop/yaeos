#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#define TRANSM 0
#define RECV 1

void intHandler();
void timer_HDL();
void device_HDL();
void terminal_HDL();
unsigned int findLineNo(unsigned int device);
void SVST(state_t *A, state_t *B);
void sendACK(termreg_t* device, int type, int index);

#endif
