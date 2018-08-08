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
	extern pcb_t *readyQueue[PRIO_HIGH+1];
	extern int processCount;
	extern int softBlockCount;
	int turn = PRIO_IDLE;
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
