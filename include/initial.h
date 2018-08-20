#ifndef INITIAL_H
#define INITIAL_H

#include <uARMtypes.h>
#include <uARMconst.h>
#include <arch.h>
#include <libuarm.h>

#include <const.h>
#include "pcb.h"
#include "asl.h"
#include "sheduler.h"

typedef unsigned int memaddr;

extern void test();
void newArea(memaddr address, void handler());

unsigned int aging_elapsed = 0;
unsigned int aging_times = 0;
unsigned int isAging = 0;
unsigned int aging_times = 0;
slice_t lastSlice;

#endif
