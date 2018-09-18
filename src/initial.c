#include "initial.h"

#include "asl.h"

#include "interrupts.h"
#include "exceptions.h"
#include "scheduler.h"

#include <uARMconst.h>
#include <libuarm.h>

int main(int argc, char const *argv[]){

	int i;

	//1. NEWAREAs init
	newArea(INT_NEWAREA, INT_handler);
	newArea(TLB_NEWAREA, TLB_handler);
	newArea(PGMTRAP_NEWAREA, PGMT_handler);
	newArea(SYSBK_NEWAREA, SYSBK_handler);
	
	//2. Phase1's structures init
	initPcbs();
	initASL();

	//3. Nucleus maintained variables init
	processCount = 1;
	softBlock = 0;
	currentPCB = NULL;
	readyQueue = NULL;

	//4. Nucleus' semaphores init
	for(i = 0; i < CLOCK_SEM; i++) semDev[i] = 1;
	semDev[CLOCK_SEM] = 0;

	//5. First process' PCB
	pcb_t *first = allocPcb();
	first->p_s.cpsr = STATUS_ALL_INT_ENABLE(first->p_s.cpsr);
	first->p_priority = 0;
	first->p_s.CP15_Control = CP15_CONTROL_NULL;
	first->p_s.cpsr = STATUS_SYS_MODE;
	first->p_s.sp = RAM_TOP - FRAMESIZE;
	first->p_s.pc = (memaddr)test;
	insertProcQ(&readyQueue, first);
	
	//6. Scheduler's vars initialization and call to scheduler
	lastPseudo = getTODLO();
	lastAging = getTODLO();
	isAging = 0;
	isPseudo = 0;
	scheduler();
	
	PANIC();
}

void newArea(unsigned int address, void handler()){
	state_t *area = (state_t *)address;
	area->pc = (memaddr)handler;
	area->sp = RAM_TOP;
	area->cpsr = STATUS_NULL | STATUS_SYS_MODE;
	area->cpsr = STATUS_ALL_INT_DISABLE(area->cpsr);
	area->CP15_Control = CP15_CONTROL_NULL;
}