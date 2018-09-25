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
#include "initial.h"
#include "syscall.h"
#include "exceptions.h"
#include "interrupts.h"
#include "scheduler.h"


void tlbHandler(){
	//~ tprint("tlbHandler\n");
}

void pgmtrapHandler(){
	//~ tprint("pgmtrapHandler\n");
}

void sysbkHandler(){
	//~ tprint("sysbkHandler\n");
	extern pcb_t *currentPCB;
	extern cpu_t elapsed, lastTime;
	
	elapsed = getTODLO() - lastTime;
	lastTime = getTODLO();
	//~ currentPCB->user_time += elapsed;
	
	if (currentPCB) {
		SVST((state_t *)SYSBK_OLDAREA, &currentPCB->p_s);
	}
	
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
			semp();
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
			iodevop();
			break;
		case(GETPIDS):
			getpids();
			break;
		case(WAITCHLD):
			waitchild();
			break;
		default:
			tprint("default\n");
	}
	
	scheduler();
}
