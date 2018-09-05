#include "initial.h"

#include "pcb.h"
#include "asl.h"

#include "interrupts.h"
#include "exceptions.h"
#include "scheduler.h"

#include <uARMconst.h>
#include <libuarm.h>
#include <arch.h>

#define NDEVICES (DEV_PER_INT * (DEV_USED_INTS + 1))

pcb_t *readyQueues[4], *currentProcess;
unsigned int processCount, softBlock;
unsigned int *sem_devices[NDEVICES], *sem_pseudoclock;

unsigned int aging_elapsed = 0;
unsigned int aging_times = 0;
unsigned int isAging = 0;
unsigned int kernelStart, curProc_start;

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
	for(i = PRIO_LOW; i < PRIO_IDLE; i++) readyQueues[i] = NULL;

	//4. Nucleus' semaphores init
	for(i = 0; i < NDEVICES; i++) sem_devices[i] = 0;
	sem_pseudoclock = 0;

	//5. First process' PCB
	pcb_t *first = allocPcb();
	first->p_s.cpsr = STATUS_ALL_INT_ENABLE(first->p_s.cpsr);
	first->p_priority = PRIO_NORM;
	first->p_s.CP15_Control = CP15_CONTROL_NULL;
	first->p_s.cpsr = STATUS_SYS_MODE;
	first->p_s.sp = RAM_TOP - FRAMESIZE;
	first->p_s.pc = (memaddr)test;
	insertProcQ(&readyQueues[PRIO_NORM], first);
	
	//6. Call to scheduler
	scheduler();
	
	return 0;
}

void newArea(unsigned int address, void handler()){
	state_t *area = (state_t *)address;
	area->pc = (memaddr)handler;
	area->sp = RAM_TOP;
	area->cpsr = STATUS_SYS_MODE;
	area->cpsr = STATUS_ALL_INT_DISABLE(area->cpsr);
	area->CP15_Control = CP15_CONTROL_NULL;
}