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
	int turn = PRIO_LOW;
	while (turn--){
		//SYSCALL(SEMV, (unsigned int)readyQueue[turn], 0, 0);
		//setTIMER(100000UL);
		((void (*)(void))readyQueue[turn]->p_s.pc)();
		//tprint("test\n");
		//WAIT();
		if (!turn) turn = PRIO_HIGH;
	}
}
