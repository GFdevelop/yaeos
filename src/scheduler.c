#include "scheduler.h"

#include "pcb.h"
#include "initial.h"

#include <libuarm.h>

extern pcb_t *readyQueue, *currentProcess;
extern unsigned int processCount, softBlock, isPseudo, isAging;
extern cpu_t curProc_start, kernel_start, lastPseudo, lastAging;

void scheduler(){
	
	if(currentProcess == NULL){	
		if(headProcQ(readyQueue) != NULL){
			
			currentProcess = removeProcQ(&readyQueue);	
		}else{
			if(processCount == 0) HALT();	//Shutdown
			else{
				if(softBlock == 0) PANIC();	//Deadlock
				else{
					setSTATUS(STATUS_ALL_INT_ENABLE(getSTATUS()));
					WAIT();
				}
			}
		}
	}else{

	}
	setTIMER(nextSlice());
	curProc_start = getTODLO();
	LDST(&currentProcess->p_s);

}

cpu_t nextSlice(){
	cpu_t toPseudo, toAging, slice;

	toPseudo = getTODLO() - lastPseudo;
	toAging = getTODLO() - lastAging;

/*
	slice = MIN(TIME_SLICE, MIN(toPseudo, toAging));
	if(slice == toPseudo) isPseudo = 1;
	else if(slice == toAging) isAging = 1;
	return slice;
*/
	return MIN(TIME_SLICE, MIN(toPseudo, toAging));
}