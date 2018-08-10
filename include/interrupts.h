#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <libuarm.h>
#include <uARMconst.h>

int INT_handler();
void timer_HDL();
void device_HDL(unsigned int device);
void terminal_HDL();

#endif