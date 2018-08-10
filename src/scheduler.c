#include <scheduler.h>

void scheduler(){

	extern pcb_t *readyQueues[4];
	extern uint8_t processCount, softBlock;
	int turn = PRIO_IDLE;

	/*
		Scheduler preemptive con aging.
		Time-Sclice = 3ms 
		Ogni 10ms la priorità viene aumentata di 1 fino a PRIO_HIGH

		readyQueue[0...3] = NULL
		+ precessCount = 0 ---> SHUTDOWN (HALT())
		+ softBlockCount > 0 ---> WAIT()
		+ softBlockCount = 0 ---> DEADLOCK (PANIC())
	*/

	setTIMER(100000UL);
	while (processCount){
		if (readyQueue[turn] != NULL) LDST(&readyQueue[turn]->p_s);
		//SYSCALL(SEMV, (unsigned int)readyQueue[turn], 0, 0);
		//((void (*)(void))readyQueue[turn--]->p_s.pc)();
		//tprint("test\n");
		//if (softBlockCount) WAIT();
		//else PANIC();
		if (!turn) turn = PRIO_HIGH;
	}
}