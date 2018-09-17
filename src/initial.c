#include "initial.h"

#include "pcb.h"
#include "asl.h"

#include "interrupts.h"
#include "exceptions.h"
#include "scheduler.h"

#include <uARMconst.h>
#include <libuarm.h>
#include <arch.h>


pcb_t *readyQueue, *currentProcess;
unsigned int processCount, softBlock;
unsigned int sem_devices[MAX_DEVICES];

cpu_t lastAging, lastPseudo, curProc_start, kernel_start;
unsigned int isPseudo = 0, isAging = 0;

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
	currentProcess = NULL;
	readyQueue = NULL;

	//4. Nucleus' semaphores init
	for(i = 0; i < CLOCK_SEM; i++) sem_devices[i] = 1;
	sem_devices[CLOCK_SEM] = 0;

	//5. First process' PCB
	pcb_t *first = allocPcb();
	first->p_s.cpsr = STATUS_ALL_INT_ENABLE(first->p_s.cpsr);
	first->p_priority = 0;
	first->p_s.CP15_Control = CP15_CONTROL_NULL;
	first->p_s.cpsr = STATUS_SYS_MODE;
	first->p_s.sp = RAM_TOP - FRAMESIZE;
	first->p_s.pc = (memaddr)test;
	insertProcQ(&readyQueue, first);
	
	//6. Call to scheduler
	lastPseudo = getTODLO();
	lastAging = lastPseudo;
	scheduler();
	
	return 0;
}

void newArea(unsigned int address, void handler()){
	state_t *area = (state_t *)address;
	area->pc = (memaddr)handler;
	area->sp = RAM_TOP;
	area->cpsr = STATUS_NULL | STATUS_SYS_MODE;
	area->cpsr = STATUS_ALL_INT_DISABLE(area->cpsr);
	area->CP15_Control = CP15_CONTROL_NULL;
}