#include "interrupts.h"

#include "pcb.h"
#include "scheduler.h"

#include <libuarm.h>

extern state_t *INT_Old;
extern pcb_t *currentProcess;
//Hints from pages 130 and 63, uARMconst.h and libuarm.h
void INT_handler(){

	INT_Old = (state_t *) INT_OLDAREA;
	INT_Old->pc -= 4;
	SVST(INT_Old, &currentProcess->p_s);

	unsigned int cause = getCAUSE();

	if(CAUSE_IP_GET(cause, INT_TIMER)){
		tprint("Timer interrupt\n");
		timer_HDL();
	}else if(CAUSE_IP_GET(cause, INT_LOWEST)){
		tprint("Lowest interrupt\n");
		device_HDL(INT_LOWEST);
	}else if(CAUSE_IP_GET(cause, INT_DISK)){
		tprint("Disk interrupt\n");
		device_HDL(INT_DISK);
	}else if(CAUSE_IP_GET(cause, INT_TAPE)){
		tprint("Tape interrupt\n");
		device_HDL(INT_TAPE);
	}else if(CAUSE_IP_GET(cause, INT_UNUSED)){
		tprint("Unused interrupt\n");
		device_HDL(INT_UNUSED);
	}else if(CAUSE_IP_GET(cause, INT_PRINTER)){
		tprint("Printer interrupt\n");
		device_HDL(INT_PRINTER);
	}else if(CAUSE_IP_GET(cause, INT_TERMINAL)){
		tprint("Terminal interrupt\n");
		terminal_HDL();
	}else{
		tprint("Interrupt not recognized!\n");
		PANIC();
	}

	return;
}

//TODO: The lower the line, the higher the priority
void timer_HDL(){
	scheduler();
	//TODO: Distinguish between 3ms slice expiration and 100ms pseudo-clock
}

void device_HDL(unsigned int device){

}

void terminal_HDL(){
	//TODO: Handle terminal priority
}

void SVST(state_t *A, state_t *B){
	B->a1 = A->a1;
	B->a2 = A->a2;
	B->a3 = A->a3;
	B->a4 = A->a4;
	B->v1 = A->v1;
	B->v2 = A->v2;
	B->v3 = A->v3;
	B->v4 = A->v4;
	B->v5 = A->v5;
	B->v6 = A->v6;
	B->sl = A->sl;
	B->fp = A->fp;
	B->ip = A->ip;
	B->sp = A->sp;
	B->lr = A->lr;
	B->pc = A->pc;
	B->cpsr = A->cpsr;
	B->CP15_Control = A->CP15_Control;
	B->CP15_EntryHi = A->CP15_EntryHi;
	B->CP15_Cause = A->CP15_Cause;
	B->TOD_Hi = A->TOD_Hi;
	B->TOD_Low = A->TOD_Low;
}