/* * * * * * * * * * * * * * * * * * * * * * * * *
 * YAEOS' phase 2 implementation proposed by     *
 * - Francesco Fornari 							 *
 * - Gabriele Fulgaro							 *
 * - Mattia Polverini							 *
 * 												 *
 * Operating System course						 *
 * A.A. 2017/2018 								 *
 * Alma Mater Studiorum - University of Bologna  *
 * * * * * * * * * * * * * * * * * * * * * * * * */


#include <uARMconst.h>
#include <uARMtypes.h>
#include <libuarm.h>
#include <arch.h>

#include "const.h"
#include "pcb.h"
#include "asl.h"

#include "interrupts.h"
#include "exceptions.h"
#include "syscall.h"
#include "scheduler.h"
#include "initial.h"

pcb_t *readyQueue, *currentPCB;
unsigned int processCount, softBlock;
int semDev[MAX_DEVICES];
cpu_t checkpoint, slice, lastSlice, tick, lastTick;
int semWaitChild;

void newArea(memaddr address, void handler()){
	state_t *area = (state_t *)address;
	area->pc = (memaddr)handler;
	area->sp = RAM_TOP;
	area->cpsr = STATUS_SYS_MODE;
	area->cpsr = STATUS_ALL_INT_DISABLE(area->cpsr);
    area->CP15_Control = CP15_CONTROL_NULL;
}

int main() {
	// init NEW area
	newArea(INT_NEWAREA,intHandler);
	newArea(TLB_NEWAREA,tlbHandler);
	newArea(PGMTRAP_NEWAREA,pgmtrapHandler);
	newArea(SYSBK_NEWAREA,sysbkHandler);

	// init pcb and asl
	initPcbs();
	initASL();

	//~ tprint("init variables\n");
	currentPCB = NULL;
	processCount = 1;
	softBlock = 0;

	//~ tprint("init semaphores\n");
	for(int i = 0; i < MAX_DEVICES; i++) semDev[i] = 0;
	semWaitChild = 0;

	//~ tprint("create first pcb\n");
	readyQueue = allocPcb();
	readyQueue->p_priority = 0;
	readyQueue->p_s.cpsr = STATUS_SYS_MODE;
	readyQueue->p_s.cpsr = STATUS_ALL_INT_ENABLE(readyQueue->p_s.cpsr);
	readyQueue->p_s.CP15_Control = CP15_CONTROL_NULL;
	readyQueue->p_s.sp = RAM_TOP-FRAME_SIZE;
	readyQueue->p_s.pc = (memaddr)test;

	//~ checkpoint = getTODLO();
	slice = SLICE_TIME;
	tick = TICK_TIME;
	lastSlice = lastTick = getTODLO();

	//~ tprint("call scheduler\n");
	scheduler();

	return 0;
}
