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

void debugger(){}

void scheduler(){
	extern pcb_t *readyQueue, *currentPCB;
	extern unsigned int processCount, softBlock;
	extern cpu_t slice, elapsed, lastTime;

	if (processCount){
		if (currentPCB == NULL) {
			if (headProcQ(readyQueue) != NULL) {
				currentPCB = removeProcQ(&readyQueue);
				if (currentPCB->activation_time == 0) currentPCB->activation_time = getTODLO();
				slice = getTODLO();
				setTIMER(SLICE_TIME);
			}
			else if (softBlock) {
				debugger();
				setTIMER(SLICE_TIME);
				setSTATUS(STATUS_ALL_INT_ENABLE(getSTATUS()));
				WAIT();
			}
			else PANIC();
		}

		elapsed = getTODLO() - lastTime;
		lastTime = getTODLO();
		currentPCB->kernel_time += elapsed;

		LDST(&currentPCB->p_s);
	}
	tprint("no process count\n");
	HALT();
}
