#include "interrupts.h"

#include "pcb.h"
#include "scheduler.h"
#include "initial.h"
#include "exceptions.h"

#include <uARMtypes.h>
#include <libuarm.h>
#include <arch.h>

extern state_t *INT_Old;
extern pcb_t *currentProcess, *readyQueues[4];
extern unsigned int softBlock, kernelStart;
//Hints from pages 130 and 63, uARMconst.h and libuarm.h
void INT_handler(){

	kernelStart = getTODLO();
	INT_Old = (state_t *) INT_OLDAREA;
	INT_Old->pc -= 4;
	SVST(&currentProcess->p_s, INT_Old);

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

	scheduler();

	return;
}

//TODO: The lower the line, the higher the priority
void timer_HDL(){
	extern unsigned int aging_times, isAging;
	extern pcb_t *currentProcess;

	if(aging_times == 10){
		aging_times = 0;
		//pseudo-clock()
	}

	if(!isAging){
		insertProcQ(&readyQueues[currentProcess->p_priority], currentProcess);
		currentProcess = NULL;
	}
}

void device_HDL(unsigned int device){
	return;
}

void terminal_HDL(){

	termreg_t *term;
	memaddr *line;
	unsigned int terminal_no = 0;


	//1. Determinare quale dei teminali ha generato l'interrupt
	line = (memaddr *)IDEV_BITMAP_ADDR(INT_TERMINAL);
	while(*line > 0){
		if(*line & 1) break;
		else{
			terminal_no++;
			*line = *line >> 1;
		}	
	}
	
	//2. Determinare se l'interrupt deriva da una scrittura, una lettura o entrambi
	term = (termreg_t *)DEV_REG_ADDR(INT_TERMINAL, terminal_no);
	
	if((term->recv_status & DEV_TERM_STATUS) == DEV_TRCV_S_CHARRECV){
		term->recv_command = DEV_C_ACK;
	}else if((term->transm_status & DEV_TERM_STATUS) == DEV_TTRS_S_CHARTRSM){
		term->transm_command = DEV_C_ACK;
	}
	
	insertProcQ(&readyQueues[currentProcess->p_priority], currentProcess);

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