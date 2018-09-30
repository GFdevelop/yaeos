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


// TODO: timing
void trapHandler(memaddr oldArea){
	extern pcb_t *currentPCB;
	
	int type = SPECNULL;
	switch(oldArea){
		case SYSBK_OLDAREA: type--;
		case TLB_OLDAREA: type--;
		case PGMTRAP_OLDAREA: type--;
	}
	
	if (currentPCB->specTrap[type + SPECNULL] == (memaddr)NULL) {
		currentPCB->p_s.a2 = (memaddr)NULL;
		terminateprocess();
	}
	else {
		((state_t *)oldArea)->pc -= WORD_SIZE;
		SVST((state_t *)oldArea, &currentPCB->p_s);
		unsigned int cause = getCAUSE();
		SVST(&currentPCB->p_s,(state_t *)currentPCB->specTrap[type]);
		SVST((state_t *)currentPCB->specTrap[type + SPECNULL],&currentPCB->p_s);
		setCAUSE(cause);
	}
	
	scheduler();
}

void tlbHandler(){
	trapHandler(TLB_OLDAREA);
}

void pgmtrapHandler(){
	trapHandler(PGMTRAP_OLDAREA);
}

void sysbkHandler(){
	extern pcb_t *currentPCB;
	extern cpu_t checkpoint;
	
	SVST((state_t *)SYSBK_OLDAREA, &currentPCB->p_s);
	
	if ((currentPCB->p_s.cpsr & STATUS_USER_MODE) == STATUS_USER_MODE) currentPCB->user_time += getTODLO() - checkpoint;
	else currentPCB->kernel_time += getTODLO() - checkpoint;
	checkpoint = getTODLO();
	
	if (((currentPCB->p_s.cpsr & STATUS_SYS_MODE) == STATUS_USER_MODE) && (currentPCB->p_s.a1 <= 10)){
		SVST(&currentPCB->p_s,(state_t *)PGMTRAP_OLDAREA);
		((state_t *)PGMTRAP_OLDAREA)->CP15_Cause = CAUSE_EXCCODE_SET(currentPCB->p_s.CP15_Cause, EXC_RESERVEDINSTR);
		pgmtrapHandler();
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
			if (currentPCB->specTrap[SPECSYSBP + SPECNULL] == (memaddr)NULL) {
				currentPCB->p_s.a2 = (memaddr)NULL;
				terminateprocess();
			}
			else {
				SVST(&currentPCB->p_s,(state_t *)currentPCB->specTrap[SPECSYSBP]);
				SVST((state_t *)currentPCB->specTrap[SPECSYSBP + SPECNULL],&currentPCB->p_s);
			}
	}
	
	scheduler();
}
