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

void scheduler(){
	extern pcb_t *readyQueue, *currentPCB;
	extern unsigned int processCount, softBlock;

	if (processCount){
		if (currentPCB == NULL) {
			if (headProcQ(readyQueue) != NULL) {
				currentPCB = removeProcQ(&readyQueue);
			}
			else if (softBlock) {
				setSTATUS(STATUS_ALL_INT_ENABLE(getSTATUS()));
				WAIT();
			}
			else PANIC();
		}

		LDST(&currentPCB->p_s);
	}
	tprint("no process count\n");
	HALT();
}
