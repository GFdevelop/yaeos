#ifndef INIT_H
#define INIT_H

#include <uARMtypes.h>
#include <uARMconst.h>
#include <arch.h>
#include <libuarm.h>

#include <const.h>
#include <pcb.h>
#include <asl.h>

typedef uint8_t memaddr;

extern void test();
void newArea(uint8_t address, void handler());

#endif
