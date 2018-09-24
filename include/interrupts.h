#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#define TRANSM 0
#define RECV 1
#define GENERIC 2

void intHandler();
void timer_HDL();
void device_HDL();
void terminal_HDL();
void SVST(state_t *A, state_t *B);
unsigned int instanceNo(unsigned int device);
void sendACK(devreg_t *device, int type, int index);

#endif
