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


#include "libuarm.h"
#include "pcb.h"
#include "initial.h"
#include "syscall.h"
#include "scheduler.h"

void debugger(){};


void scheduler(){
	extern pcb_t *readyQueue, *currentPCB;
	extern unsigned int processCount, softBlock;
	extern cpu_t slice, tick, interval;
	
	if (processCount){
		if (currentPCB == NULL) {
			if (readyQueue != NULL) currentPCB = removeProcQ(&readyQueue);
			else if (softBlock) {
				//~ tprint("wait scheduler\n");
				setSTATUS(STATUS_ALL_INT_ENABLE(getSTATUS()));
				WAIT();
			}
			else PANIC();
		}
		
		//~ if (currentPCB->activation_time == 0) currentPCB->activation_time = getTODLO();	// TODO: freePCB reset time
		interval = getTODLO() - MIN(slice + SLICE_TIME, tick + TICK_TIME);
		//~ interval = interval - getTODLO();
		setTIMER(interval);
		
		LDST(&currentPCB->p_s);
		
		//((void (*)(void))readyQueue[turn--]->p_s.pc)();
	}
	tprint("no process count\n");
	HALT();
}
