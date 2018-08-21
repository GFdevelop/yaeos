#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <uARMtypes.h>
#include <libuarm.h>
#include <scheduler.h>

void INT_handler();
void timer_HDL();
void device_HDL(unsigned int device);
void terminal_HDL();

state_t *INT_Old;

#endif