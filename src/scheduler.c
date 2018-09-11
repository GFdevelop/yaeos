#include "scheduler.h"

#include "pcb.h"

#include <libuarm.h>

void scheduler(){

	extern pcb_t *readyQueue, *currentProcess;
	extern unsigned int processCount, softBlock;
	extern unsigned int isAging, aging_elapsed, aging_times, curProc_start, kernelStart;
	unsigned int slice;

	//4. AGING: di norma slice = TIME_SLICE ma, se il tempo mancante per l'aging < TIME_SLICE
	//il prossimo timer viene settato a questa quantità e l'interrupt deve essere interpretato in tal senso 
	if(isAging){
		//tprint("*\n");
		isAging = 0;
		aging_elapsed = 0;	
		
		forallProcQ(readyQueue, ager, NULL);

		aging_times += 1;
	}
	//1. Alla fine di ogni TIME_SLICE, currentProcess viene rimesso a NULL
	//
	if(currentProcess == NULL){	
		if(headProcQ(readyQueue) != NULL){
			slice = nextSlice();//TIME_SLICE;//;	//Selezioniamo la durata del nuovo timer
			currentProcess = removeProcQ(&readyQueue);	//rimuoviamo il prcesso dalla coda, se tutto va bene sarà rimesso in coda poi
		}else{
			if(!processCount){
				tprint("processCount = 0, SHUTDOWN\n");
				HALT();
			}else{
				if(!softBlock){
					tprint("System is deadlocked, sir. PANIC!\n");
					PANIC();
				}else{
					setSTATUS(STATUS_ALL_INT_ENABLE(getSTATUS()));
					WAIT();
				}
			}
		}
	//3. Ci troviamo in questo caso se un processo viene interrotto durante la sua esecuzione
	}else{
		//Addebito il tempo trascorso in kernel mode dal processo
		//tprint("Here3");
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

void ager(struct pcb_t *pcb, void *count){
	pcb->p_priority += 1;	
}