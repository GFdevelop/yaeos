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
	extern cpu_t checkpoint, slice, lastSlice, tick;

	if (processCount){
		if (currentPCB == NULL) {
			if (headProcQ(readyQueue) != NULL) {
				setTIMER(SLICE_TIME);	// prevent the process from being inserted back into the readyQueue

				currentPCB = removeProcQ(&readyQueue);

				slice = SLICE_TIME;
				lastSlice = getTODLO();
				if (currentPCB->activation_time == 0) currentPCB->activation_time = checkpoint;
			}
			else if (softBlock) {
				setSTATUS(STATUS_ALL_INT_ENABLE(getSTATUS()));
				WAIT();
			}
			else PANIC();
		}
		else currentPCB->kernel_time += getTODLO() - checkpoint;

		checkpoint = getTODLO();
		setTIMER(MIN(slice,tick));

		LDST(&currentPCB->p_s);
	}
	tprint("no process count\n");
	HALT();
}
