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
#include <arch.h>
#include <libuarm.h>
#include <asl.h>
#include "initial.h"
#include "interrupts.h"
#include "scheduler.h"

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

void intHandler(){
	//tprint("intHandler\n");
	extern pcb_t *readyQueue, *currentPCB;
	
	state_t *oldState = (state_t *)INT_OLDAREA;
	if (currentPCB) {
		(*oldState).pc -= WORD_SIZE;
		SVST(oldState, &currentPCB->p_s);
	}
	
	if(CAUSE_IP_GET((unsigned int)getCAUSE(), INT_TIMER)){
		tprint("Timer interrupt\n");
		timer_HDL();
	}else if(CAUSE_IP_GET((unsigned int)getCAUSE(), INT_LOWEST)){
		tprint("Lowest interrupt\n");
		device_HDL(INT_LOWEST);
	}else if(CAUSE_IP_GET((unsigned int)getCAUSE(), INT_DISK)){
		tprint("Disk interrupt\n");
		device_HDL(INT_DISK);
	}else if(CAUSE_IP_GET((unsigned int)getCAUSE(), INT_TAPE)){
		tprint("Tape interrupt\n");
		device_HDL(INT_TAPE);
	}else if(CAUSE_IP_GET((unsigned int)getCAUSE(), INT_UNUSED)){
		tprint("Unused interrupt\n");
		device_HDL(INT_UNUSED);
	}else if(CAUSE_IP_GET((unsigned int)getCAUSE(), INT_PRINTER)){
		tprint("Printer interrupt\n");
		device_HDL(INT_PRINTER);
	}else if(CAUSE_IP_GET((unsigned int)getCAUSE(), INT_TERMINAL)){
		//tprint("Terminal interrupt\n");
		terminal_HDL();
	}else{
		tprint("Interrupt not recognized!\n");
		PANIC();
	}
	
	insertProcQ(&readyQueue, currentPCB);
	scheduler();
}

void timer_HDL(){
	
}

void device_HDL(){

}

void terminal_HDL(){
	//tprint ("terminal_HDL\n");
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
}
