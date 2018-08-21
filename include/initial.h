#ifndef INITIAL_H
#define INITIAL_H

#include <uARMtypes.h>
#include <arch.h>
#include <libuarm.h>

//--- PHASE 1 ---
#include <const.h>

#include <pcb.h>
#include <asl.h>
//--- PHASE 2 ---
#include <scheduler.h>
#include <interrupts.h>
#include <exceptions.h>


typedef unsigned int memaddr;

extern void test();
void newArea(memaddr address, void handler());

unsigned int aging_elapsed = 0;
unsigned int aging_times = 0;
unsigned int isAging = 0;
slice_t lastSlice;

#endif
