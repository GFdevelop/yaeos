#include "initial.h"

#include "pcb.h"
#include "asl.h"

#include "interrupts.h"
#include "exceptions.h"
#include "scheduler.h"

#include <arch.h>

/* 

typedef struct {
	unsigned int a1; 			r0: first function argument / integer result
	unsigned int a2; 			r1: second function argument
	unsigned int a3; 			r2: third function argument
	unsigned int a4; 			r3: fourth function argument
	unsigned int v1; 			r4: register variable
	unsigned int v2; 			r5: register variable
	unsigned int v3; 			r6: register variable
	unsigned int v4; 			r7: register variable
	unsigned int v5; 			r8: register variable
	unsigned int v6; 			r9: (v6/rfp) register variable / real frame pointer
	unsigned int sl; 			r10: stack limit
	unsigned int fp; 			r11: frame pointer / argument pointer
	unsigned int ip; 			r12: instruction pointer / temporary workspace
	unsigned int sp; 			r13: stack pointer
	unsigned int lr; 			r14: link register
	unsigned int pc; 			r15: program counter
	unsigned int cpsr;			current program status register, kernel mode cpsr[0-4]=0x1F
	unsigned int CP15_Control;	virtual memory on/off, address resolution off CP15_Control[0]=0
	unsigned int CP15_EntryHi;	Address Space Identifier (ASID)
	unsigned int CP15_Cause;	cause of the PgmTrap exception
	unsigned int TOD_Hi;		time of day, high bits
	unsigned int TOD_Low;		time of day, low bits
 } state_t;

 */

pcb_t *readyQueues[4], *currentProcess;
unsigned int processCount, softBlock;
semd_t *io;

unsigned int aging_elapsed = 0;
unsigned int aging_times = 0;
unsigned int isAging = 0;

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
	io = NULL;
	
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