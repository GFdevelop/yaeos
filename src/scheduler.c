#include <scheduler.h>

unsigned int aging_elapsed = 0;
unsigned int aging_times = 0;

void scheduler(){

	extern pcb_t *readyQueues[4], *currentProcess;
	extern uint8_t processCount, softBlock;
	unsigned int turn, lastSlice = 0, aging_elapsed, isAging = 0, aging_times = 0;

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
		isAging = 0;
	}

	//Se l'aging è stato effettuato 10 volte, sono passati 100ms. Possiamo mandare il segnale di pseudo-clock.
	if(aging_times == 10){
		aging_times = 0;
		pseudo_clock();
	}

	//Scegliamo la lunghezza del prossimo timer
	nextSlice = selectSlice();
	//Se nectSlice < TIME_SLICE vuol dire che il prossimo interrupt deve essere letto come segnale di aging
	if(nextSlice != TIME_SLICE) isAging = 1;
	//Salvo il valore del timer che andrò ad impostare per dopo
	lastSlice = nextSlice;
	//Setto il timer
	setTIMER(nextSlice);

	//Se l'interrupt arrivato è di aging lo scheduler non deve rimpiazzare il processo correntemente in esecuzione, altrimenti sì
	if(!isAging){
		for(turn = PRIO_HIGH; turn >= PRIO_IDLE; turn--){
			if(readyQueues[turn] == NULL) continue;
			else{
				//Codice dello scheduler
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
	aging_elapsed += lastSlice;
	remaining = AGING_TIME - aging_elapsed;
	return MIN(TIME_SLICE, remaining);
}

void ager(){
	//Codice dell'ager
}

void pseudo_clock(){
	//codice dello pseudo_clock
}