#include "scheduler.h"

#include "pcb.h"

#include <libuarm.h>

void scheduler(){

	extern pcb_t *readyQueues[4], *currentProcess;
	extern unsigned int processCount, softBlock;
	extern unsigned int isAging, aging_elapsed, aging_times, curProc_start, kernelStart;
	unsigned int slice, turn;

	//4. AGING: di norma slice = TIME_SLICE ma, se il tempo mancante per l'aging < TIME_SLICE
	//il prossimo timer viene settato a questa quantità e l'interrupt deve essere interpretato in tal senso 
	if(isAging){
		unsigned int prio;
		pcb_t *tmp = NULL;

		isAging = 0;
		aging_elapsed = 0;	
		for(prio = PRIO_HIGH; prio >= PRIO_LOW; prio--){	
			while(headProcQ(readyQueues[prio]) != NULL){
				tmp = removeProcQ(&readyQueues[prio]);
				tmp->p_priority += 1;
				insertProcQ(&readyQueues[prio+1], tmp);
			}
		}	

		aging_times += 1;
	}
	//1. Alla fine di ogni TIME_SLICE, currentProcess viene rimesso a NULL
	if(currentProcess == NULL){
		for(turn = PRIO_HIGH; turn >= PRIO_NORM; turn--){	//Selezioniamo un nuovo processo
			if(headProcQ(readyQueues[turn]) != NULL){
				slice = nextSlice();	//Selezioniamo la durata del nuovo timer
				currentProcess = removeProcQ(&readyQueues[turn]);	//rimuoviamo il prcesso dalla coda, se tutto va bene sarà rimesso in coda poi
			}
		}
		//2. Deadlock detection
		if(currentProcess == NULL){
			if(processCount == 0){
				tprint("processCount = 0, SHUTDOWN\n");
				HALT();
			}else{
				if(softBlock == 0){
					tprint("System is deadlocked, sir. PANIC!\n");
					PANIC();
				}else WAIT();
			}
		}
	//3. Ci troviamo in questo caso se un processo viene interrotto durante la sua esecuzione
	}else{
		//Addebito il tempo trascorso in kernel mode dal processo
		currentProcess->kernel_time += (getTODLO() - kernelStart); 
		//Il nuovo valore del timer sarà il tempo rimanente
		slice = TIME_SLICE - (kernelStart - curProc_start);
	}

	setTIMER(slice);	//Setto effettivamente il prossimo timer
	curProc_start = getTODLO();	
	LDST(&currentProcess->p_s);	//Ricomincia la festa!

}

unsigned int nextSlice(){
	extern unsigned int aging_elapsed, isAging, curProc_start;
	unsigned int slice;

	aging_elapsed += (getTODLO() - curProc_start);
	slice = MIN(TIME_SLICE, (AGING_TIME - aging_elapsed));
	if(slice < TIME_SLICE) isAging = 1;
	return slice;
}