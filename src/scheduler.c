#include <scheduler.h>
#include <string.h>

void scheduler(){

	extern pcb_t *readyQueues[4], *currentProcess;
	extern unsigned int processCount, softBlock;
	extern unsigned int isAging, aging_times, aging_elapsed;
	extern state_t *INT_Old;
	extern slice_t lastSlice;
	unsigned int turn, nextSlice;

	/*
		Scheduler preemptive con aging.
		Time-Sclice = 3ms 
		Ogni 10ms la priorità viene aumentata di 1 fino a PRIO_HIGH

		readyQueues[0...3] = NULL
		+ precessCount = 0 ---> SHUTDOWN (HALT())
		+ softBlockCount > 0 ---> WAIT()
		+ softBlockCount = 0 ---> DEADLOCK (PANIC())
	*/

	//Codice eseguito solo se l'ultimo timer settato non è uguale alla lunghezza di un TIME_SLICE
	//ma è minore in quanto è il tempo restante per arrivare al AGING_TIME
	if(isAging){
		aging_times++;
		ager();
		aging_elapsed = 0;
		//Se l'aging è stato effettuato 10 volte, sono passati 100ms. Possiamo mandare il segnale di pseudo-clock.
		if(aging_times == 10){
			aging_times = 0;
			pseudo_clock();
		}
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
		for(turn = PRIO_HIGH; turn >= PRIO_IDLE; turn--){
			if(readyQueues[turn] == NULL) continue;
			else{
				//Codice dello scheduler
				currentProcess->p_s = *INT_Old;
				LDST(&(readyQueues[turn]->p_s));
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
	unsigned int prio;
	pcb_t *tmp;
	for(prio = PRIO_NORM; prio >= PRIO_LOW; prio--){
		tmp = removeProcQ(&(readyQueues[prio])); 
		while(tmp != NULL){
			insertProcQ(&readyQueues[prio+1], tmp);
			tmp = removeProcQ(&readyQueues[prio]);
		}
	}
}

void pseudo_clock(){
	//codice dello pseudo_clock
}