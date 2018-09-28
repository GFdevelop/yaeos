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
	extern pcb_t *currentPCB;
	extern state_t *sys5vector[6];
	
	((state_t *)TLB_OLDAREA)->pc -= WORD_SIZE;
	SVST((state_t *)TLB_OLDAREA, &currentPCB->p_s);
	
	if (sys5vector[SPECTLB] == NULL) {
		currentPCB->p_s.a2 = (memaddr)NULL;
		terminateprocess();
	}
	else {
		unsigned int cause = getCAUSE();
		SVST(&currentPCB->p_s,(state_t *)sys5vector[SPECTLB]);
		SVST((state_t *)sys5vector[SPECTLB + 3],&currentPCB->p_s);
		setCAUSE(cause);
	}
	
	scheduler();
}

void pgmtrapHandler(){
	//~ tprint("pgmtrapHandler\n");
	extern pcb_t *currentPCB;
	extern state_t *sys5vector[6];
	
	((state_t *)PGMTRAP_OLDAREA)->pc -= WORD_SIZE;
	SVST((state_t *)PGMTRAP_OLDAREA, &currentPCB->p_s);
	
	if (sys5vector[SPECPGMT] == NULL) {
		currentPCB->p_s.a2 = (memaddr)NULL;
		terminateprocess();
	}
	else {
		unsigned int cause = getCAUSE();
		SVST(&currentPCB->p_s,(state_t *)sys5vector[SPECPGMT]);
		SVST((state_t *)sys5vector[SPECPGMT + 3],&currentPCB->p_s);
		setCAUSE(cause);
	}
	
	scheduler();
}

void sysbkHandler(){
	//~ tprint("sysbkHandler\n");
	extern pcb_t *currentPCB;
	extern cpu_t checkpoint;
	extern state_t *sys5vector[6];
	
	SVST((state_t *)SYSBK_OLDAREA, &currentPCB->p_s);
	
	if (currentPCB->p_s.cpsr & STATUS_USER_MODE)currentPCB->user_time += getTODLO() - checkpoint;
	else currentPCB->kernel_time += getTODLO() - checkpoint;
	checkpoint = getTODLO();
	
	
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
		//~ tprint("sys10+\n");
			((state_t *)SYSBK_OLDAREA)->pc -= WORD_SIZE;
			if (sys5vector[SPECSYSBP] == NULL) {
				currentPCB->p_s.a2 = (memaddr)NULL;
				terminateprocess();
			}
			else {
				unsigned int cause = getCAUSE();
				SVST(&currentPCB->p_s,(state_t *)sys5vector[SPECSYSBP]);
				SVST((state_t *)sys5vector[SPECSYSBP + 3],&currentPCB->p_s);
				setCAUSE(cause);
			}
	}
	
	scheduler();
}
