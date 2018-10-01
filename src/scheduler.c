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
	extern cpu_t checkpoint, lastRecord, slice, lastSlice, tick, lastTick;
	
	if (processCount) {
		if (currentPCB == NULL) {
			if (headProcQ(readyQueue) != NULL) {
				setTIMER(SLICE_TIME); //Prevent the process from being inserted back into readyQueue
				
				currentPCB = removeProcQ(&readyQueue);
				
				lastSlice = getTODLO(); //Timestamp that will be used for next slice lenght calculus
				slice = SLICE_TIME;
				if (currentPCB->activation_time == 0) currentPCB->activation_time = checkpoint;
			}
			else if (softBlock) { //2nd case of deadlock detection system, softBlock > 0 -> WAIT()
				setSTATUS(STATUS_ALL_INT_ENABLE(getSTATUS()));
				WAIT();
			}
			else PANIC(); //3rd case of deadlock detection, deadlock had occours -> PANIC()
		}
		else currentPCB->kernel_time += getTODLO() - checkpoint; //Process returning from a syscall or interrupt. Kernel time accounting.
		
		lastRecord = checkpoint = getTODLO(); //Time vars update 
		setTIMER(MIN(slice + lastSlice, tick + lastTick) - getTODLO()); //Setting the actual next slice
		
		LDST(&currentPCB->p_s);
	}
	tprint("No process count\n");
	HALT(); //1st case of deadlock detection, ProcessCount == 0 -> HALT()
}
