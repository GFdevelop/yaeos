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


#include <libuarm.h>
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
	
	int type = SPECNEW;
	switch (oldArea){
		case SYSBK_OLDAREA: type--;
		case TLB_OLDAREA: type--;
		case PGMTRAP_OLDAREA: type--;
	}
	
	if (currentPCB->specTrap[type] == (memaddr)NULL) {
		currentPCB->p_s.a2 = (memaddr)NULL;
		terminateprocess();
	}
	else {
		SVST((state_t *)oldArea, (state_t *)currentPCB->specTrap[type]);
		SVST((state_t *)currentPCB->specTrap[type + SPECNEW], &currentPCB->p_s);
	}
	
	scheduler();
}

void tlbHandler(){
	extern pcb_t *currentPCB;
	extern cpu_t checkpoint, lastRecord;
	
	if ((currentPCB->p_s.cpsr & STATUS_SYS_MODE) == STATUS_USER_MODE) currentPCB->user_time += getTODLO() - checkpoint;
	else currentPCB->kernel_time += getTODLO() - checkpoint;
	lastRecord = checkpoint = getTODLO();
	
	trapHandler(TLB_OLDAREA);
}

void pgmtrapHandler(){
	extern pcb_t *currentPCB;
	extern cpu_t checkpoint, lastRecord;
	
	if ((currentPCB->p_s.cpsr & STATUS_SYS_MODE) == STATUS_USER_MODE) currentPCB->user_time += getTODLO() - checkpoint;
	else currentPCB->kernel_time += getTODLO() - checkpoint;
	lastRecord = checkpoint = getTODLO();
	
	trapHandler(PGMTRAP_OLDAREA);
}

void sysbkHandler(){
	extern pcb_t *currentPCB;
	extern cpu_t checkpoint, lastRecord;
	
	SVST((state_t *)SYSBK_OLDAREA, &currentPCB->p_s);
	
	if ((currentPCB->p_s.cpsr & STATUS_SYS_MODE) == STATUS_USER_MODE) currentPCB->user_time += getTODLO() - checkpoint;
	else currentPCB->kernel_time += getTODLO() - checkpoint;
	lastRecord = checkpoint = getTODLO();
	
	if ((CAUSE_EXCCODE_GET(currentPCB->p_s.CP15_Cause) & EXC_SYSCALL) == EXC_SYSCALL){
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
				semv(currentPCB->p_s.a2);
				break;
			case(SEMP):
				semp(currentPCB->p_s.a2);
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
				trapHandler(SYSBK_OLDAREA);
		}
	}
	
	scheduler();
}
