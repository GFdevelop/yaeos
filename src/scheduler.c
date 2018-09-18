#include "scheduler.h"

#include "pcb.h"
#include "initial.h"

#include <libuarm.h>

cpu_t slice;											//Keeps track of the value of last slice

void scheduler(){
	//Every time a process ends, currentProcess is set to NULL.
	//This means that a new process have to be selected for execution.
	if(currentProcess == NULL){	
		if(headProcQ(readyQueue) != NULL){				//NULL head means empty readyQueue
			currentProcess = removeProcQ(&readyQueue);	
			slice = nextSlice();
		}else{											//If readyQueue is empty, deadlock control is performed
			if(processCount == 0) HALT();				//Shutdown
			else{ 
				if(softBlock == 0) PANIC();				//Deadlock
				else{									//Wait for an interrupt
					setSTATUS(STATUS_ALL_INT_ENABLE(getSTATUS()));
					WAIT();
				}
			}
		}
	}else{												//currentProcess != NULL means that scheduler has been called after an interrupt
		slice -= (kernel_start - curProc_start);		//slice updated to remaining time and kernel time just spent is added
		currentProcess->kernel_time += (getTODLO() - kernel_start);
	}
	setTIMER(slice);									
	curProc_start = getTODLO();
	if(!currentProcess->activation_time) currentProcess->activation_time = curProc_start; 
	LDST(&currentProcess->p_s);

}

cpu_t nextSlice(){
	//Remaining times to pseudo-clock tick or aging tick are calculated
	cpu_t toAging = AGING_TIME - (getTODLO() - lastAging); 
	cpu_t toPseudo = PSEUDO_TIME - (getTODLO() - lastPseudo);
	cpu_t next = TIME_SLICE;
	//If one of the remaining times is lower than 3ms, next slice and relative flag are set to reflect that
	if(next >= toPseudo) next = toPseudo;
	if(next >= toAging) next = toAging;

	if(next == toPseudo) isPseudo = 1;
	if(next == toAging) isAging = 1;
	
	return next;
}