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


#include <uARMconst.h>
#include <uARMtypes.h>
#include <libuarm.h>
#include <arch.h>

#include "const.h"
#include "pcb.h"
#include "asl.h"

#include "interrupts.h"
#include "trap.h"
#include "syscall.h"
#include "scheduler.h"
#include "main.h"


//~ typedef struct {
	//~ unsigned int a1; 			//r0: first function argument / integer result
	//~ unsigned int a2; 			//r1: second function argument
	//~ unsigned int a3; 			//r2: third function argument
	//~ unsigned int a4; 			//r3: fourth function argument
	//~ unsigned int v1; 			//r4: register variable
	//~ unsigned int v2; 			//r5: register variable
	//~ unsigned int v3; 			//r6: register variable
	//~ unsigned int v4; 			//r7: register variable
	//~ unsigned int v5; 			//r8: register variable
	//~ unsigned int v6; 			//r9: (v6/rfp) register variable / real frame pointer
	//~ unsigned int sl; 			//r10: stack limit
	//~ unsigned int fp; 			//r11: frame pointer / argument pointer
	//~ unsigned int ip; 			//r12: instruction pointer / temporary workspace
	//~ unsigned int sp; 			//r13: stack pointer
	//~ unsigned int lr; 			//r14: link register
	//~ unsigned int pc; 			//r15: program counter
	//~ unsigned int cpsr;			// current program status register, kernel mode cpsr[0-4]=0x1F	?!?!?
	//~ unsigned int CP15_Control;	// virtual memory on/off, address resolution off CP15_Control[0]=0
	//~ unsigned int CP15_EntryHi;	// Address Space Identifier (ASID)
	//~ unsigned int CP15_Cause;	// cause of the PgmTrap exception
	//~ unsigned int TOD_Hi;		// time of day, high bits
	//~ unsigned int TOD_Low;		// time of day, low bits
//~ } state_t;


void newArea(unsigned int address, void handler()){
	state_t *area = (state_t *)address;
	area->pc = (memaddr)handler;
	area->sp = RAM_TOP;
	area->cpsr = STATUS_SYS_MODE;
	area->cpsr = STATUS_ALL_INT_DISABLE(area->cpsr);
	area->CP15_Control = CP15_CONTROL_NULL;
}


int main() {
	//init NEW area: Exception States Vector start at 7000 end at 72C0; each segment (processor state) is 88 bytes (58 in hex), page 70
	newArea(INT_NEWAREA,interruptHandler);
	newArea(PGMTRAP_NEWAREA,trapHandler);
	//init pcb and asl
	initPcbs();
	initASL();
	//init variables
	//int processCount = 0;
	//init semaphores
	//int sem = 0;
	//create first pcb
	pcb_t *mainPCB = allocPcb();
	mainPCB->p_s.cpsr = STATUS_ALL_INT_ENABLE(mainPCB->p_s.cpsr);
	mainPCB->p_priority = MIN_PCB_PRIORITY;
	mainPCB->p_s.CP15_Control = CP15_CONTROL_NULL;
	mainPCB->p_s.cpsr = STATUS_SYS_MODE;
	mainPCB->p_s.sp = RAM_TOP-FRAME_SIZE;
	mainPCB->p_s.pc = (memaddr)test;
	//call scheduler
	scheduler();
	return 0;
}
