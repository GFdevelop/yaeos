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
#include <arch.h>
#include "pcb.h"
#include "asl.h"
#include "syscall.h"
#include "exceptions.h"
#include "interrupts.h"
#include "scheduler.h"


void tlbHandler(){
	tprint("tlbHandler\n");
}

void pgmtrapHandler(){
	tprint("pgmtrapHandler\n");
}

void sysbkHandler(){
	tprint("sysbkHandler\n");
	setSTATUS(STATUS_ALL_INT_ENABLE(getSTATUS()));
	extern pcb_t *currentPCB;
	
	//~ if (currentPCB) {
		//~ ((state_t *)SYSBK_OLDAREA)->pc -= 2*WORD_SIZE;
		//~ SVST((state_t *)SYSBK_OLDAREA, &currentPCB->p_s);
	//~ }
	
	switch(((state_t *)SYSBK_OLDAREA)->a1){
		case(CREATEPROCESS):
			createprocess();
			break;
		case(TERMINATEPROCESS):
			terminateprocess();
			break;
		case(SEMV):
			semv(((state_t *)SYSBK_OLDAREA)->a2);
			break;
		case(SEMP):
			semp(((state_t *)SYSBK_OLDAREA)->a2);
			break;
		case(SPECHDL):
			spechdl();
			break;
		case(GETTIME):
			gettime();
			break;
		case(WAITCLOCK):
			waitclock();
			break;
		case(IODEVOP):
			iodevop(((state_t *)SYSBK_OLDAREA)->a2,((state_t *)SYSBK_OLDAREA)->a3);
			break;
		case(GETPIDS):
			getpids();
			break;
		default:
			tprint("default\n");
	}
	
	
	//~ tprint("end\n");
	//~ if (currentPCB) insertProcQ(&readyQueue, currentPCB);
	//~ else tprint("NULL exc\n");
	//~ scheduler();
	setSTATUS(STATUS_ALL_INT_DISABLE(getSTATUS()));
	LDST((state_t *)SYSBK_OLDAREA);
}
