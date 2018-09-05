#include "scheduler.h"

#include "pcb.h"

#include <libuarm.h>

void scheduler(){

	extern pcb_t *readyQueues[4], *currentProcess;
	extern unsigned int processCount, softBlock;
	extern unsigned int isAging, aging_times, aging_elapsed;
	extern slice_t lastSlice;
	unsigned int turn, nextSlice;

	//Codice eseguito solo se l'ultimo timer settato non è uguale alla lunghezza di un TIME_SLICE
	//ma è minore in quanto è il tempo restante per arrivare al AGING_TIME
	if(isAging){
		unsigned int prio;
		pcb_t *tmp = NULL;

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

	//Scegliamo la lunghezza del prossimo timer
	nextSlice = selectSlice();
	//Se nectSlice < TIME_SLICE vuol dire che il prossimo interrupt deve essere letto come segnale di aging
	if(nextSlice != TIME_SLICE && aging_elapsed != 0) isAging = 1;
	//Salvo il valore del timer che andrò ad impostare per dopo
	lastSlice.start = getTODLO();
	lastSlice.duration = nextSlice;
	//Setto il timer
	setTIMER(nextSlice);

	//Se l'interrupt arrivato è di aging lo scheduler non deve rimpiazzare il processo correntemente in esecuzione, altrimenti sì
	if(!isAging){
		tprint("Not aging\n");
		for(turn = PRIO_HIGH; turn >= PRIO_IDLE; turn--){
			if(readyQueues[turn] != NULL){
				//Codice dello scheduler
				tprint("New process\n");
				currentProcess = readyQueues[turn];
				LDST(&(currentProcess->p_s));
			}
		}
		if(!processCount){
			tprint("processCount = 0, SHUTDOWN\n");
			HALT();
		}else{
			if(!softBlock){
				tprint("System is deadlocked, sir. PANIC!\n");
				PANIC();
			}else WAIT();
		}
	} 

	tprint("You shouldn't have arrived here\n");
}


//La funzione ritorna il minimo il valore di TIME_SLICE e il valore (AGING_TIME - n. microsecondi passati dall'ultimo aging)
unsigned int selectSlice(){
	unsigned int remaining;
	extern unsigned int isAging, aging_elapsed;
	extern slice_t lastSlice;
	if(isAging){
		isAging = 0;
		return TIME_SLICE - (getTODLO() - lastSlice.start);
	}else{
		aging_elapsed += lastSlice.duration;
		remaining = AGING_TIME - aging_elapsed;
		return MIN(TIME_SLICE, remaining);
	}
}

void ager(){
	extern pcb_t *readyQueues[4];
	
}

void pseudo_clock(){
	return;
	//codice dello pseudo_clock
	//V on the semaphore mantained semaphore (see pag. 131)
}