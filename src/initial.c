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
cpu_t checkpoint, lastRecord, slice, lastSlice, tick, lastTick, aging, lastAging;
int semWaitChild;

/* --- AUXILIARY FUNCTION --- */
void newArea(memaddr address, void handler()){
	state_t *area = (state_t *)address;
	area->pc = (memaddr)handler;
	area->sp = RAM_TOP;
	area->cpsr = STATUS_SYS_MODE;
	area->cpsr = STATUS_ALL_INT_DISABLE(area->cpsr);
    area->CP15_Control = CP15_CONTROL_NULL;
}


int main() {
	//1. New areas initialization
	newArea(INT_NEWAREA,intHandler);
	newArea(TLB_NEWAREA,tlbHandler);
	newArea(PGMTRAP_NEWAREA,pgmtrapHandler);
	newArea(SYSBK_NEWAREA,sysbkHandler);
	//2. Phase1 data structures initialization
	initPcbs();
	initASL();
	//3. System vars initialization
	currentPCB = NULL;
	processCount = 1;
	softBlock = 0;
	
	for(int i = 0; i < MAX_DEVICES; i++) semDev[i] = 0;
	semWaitChild = 0;
	//4. First process creation
	readyQueue = allocPcb();
	readyQueue->p_priority = 0;
	readyQueue->p_s.cpsr = STATUS_SYS_MODE;
	readyQueue->p_s.cpsr = STATUS_ALL_INT_ENABLE(readyQueue->p_s.cpsr);
	readyQueue->p_s.CP15_Control = CP15_CONTROL_NULL;
	readyQueue->p_s.sp = RAM_TOP-FRAME_SIZE;
	readyQueue->p_s.pc = (memaddr)test;
	//5. Time vars initialization
	slice = SLICE_TIME;
	tick = TICK_TIME;
	aging = AGING_TIME;
	checkpoint = lastRecord = lastSlice = lastTick = lastAging = getTODLO();
	//6. ...enjoy!
	scheduler();
	
	return 0;
}
