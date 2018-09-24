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
cpu_t slice, tick, interval;
int semWaitChild;

void newArea(memaddr address, void handler()){
	state_t *area = (state_t *)address;
	area->pc = (memaddr)handler;
	area->sp = RAM_TOP;
	area->cpsr = STATUS_ALL_INT_DISABLE((area->cpsr) | STATUS_SYS_MODE);
    area->CP15_Control = (area->CP15_Control) & ~(0x00000001);
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

	// init variables
	readyQueue = NULL;
	currentPCB = NULL;
	processCount = 1;
	softBlock = 0;

	// init semaphores
	for(int i = 0; i < MAX_DEVICES; i++) semDev[i] = 0;
	semWaitChild = 0;

	// tprint("create first pcb
	currentPCB = allocPcb();
	currentPCB->p_s.cpsr = STATUS_ALL_INT_ENABLE(currentPCB->p_s.cpsr) | STATUS_SYS_MODE;
	currentPCB->p_priority = 0;
	currentPCB->p_s.CP15_Control = (currentPCB->p_s.CP15_Control) & ~(0x00000001);
	currentPCB->p_s.sp = RAM_TOP-FRAME_SIZE;
	currentPCB->p_s.pc = (memaddr)test;
	insertProcQ(&readyQueue, currentPCB);

	// call scheduler
	slice = getTODLO();
	tick = slice;
	interval = slice + SLICE_TIME;
	setTIMER(SLICE_TIME);
	scheduler();

	return 0;
}
