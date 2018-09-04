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
}

void device_HDL(unsigned int device){
	return;
}

/*
Acknowledge the outstanding interrupt. 
For all devices except the Interval Timer this is accomplished by writing the acknowledge command code in the interrupting device’s COMMAND device register. 
Alternatively, writing a new command in the interrupting device’s device register will also acknowledge the interrupt. 
An interrupt for a timer device is acknowledged by loading the timer with a new value.
Initialize all nucleus maintained semaphores. In addition to the above nucleus
variables, there is one semaphore variable for each external (sub)device in μARM,
plus a semaphore to represent a pseudo-clock timer. Since terminal devices are
actually two independent sub-devices (see Section 5.7-pops), the nucleus maintains
two semaphores for each terminal device. All of these semaphores need to be
initialized to zero.
*/
void terminal_HDL(){
	//ACK the interrupt with DEV_C_ACK in commmand register of the device
	//Perform a V operation (weight 1) on the nucleus maintained semaphore associated
    //with the interrupting (sub)device if the semaphore has value less than 1.
	//nucleus should mantain 2 semaphore for each terminl sub-device
	//TODO: Handle terminal priority

	//1. Determinare quale dei teminali ha generato l'interrupt
	//2. Determinare se l'interrupt deriva da una scrittura, una letturaa o entrambi
	//Se due interrupt arrivano insieme, devono essere entrambi ack per avere la giusta device mapbit

	//tprint("Here\n");
	termreg_t *term;
	memaddr *line, *term_start;
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
	//term = (termreg_t *) term_start;
	tprint("Here2\n");
	/*recv_status = (termreg_t *) term_start + RSTAT;
	recv_cmd = (termreg_t *) term_start + RCMD;
	transm_status = (termreg_t *) term_start + TSTAT;
	transm_cmd = (termreg_t *) term_start + TCMD + 0x2;*/

	if (term->recv_status == 0) tprint ("R0\n");
  	else if (term->recv_status == 1) tprint ("R1\n");
  	else if (term->recv_status == 2) tprint ("R2\n");
  	else if (term->recv_status == 3) tprint ("R3\n");
  	else if (term->recv_status == 4) tprint ("R4\n");
  	else if (term->recv_status == 5) tprint ("R5\n");

  	if ((term->transm_status & DEV_TERM_STATUS) == 0) tprint ("T0\n");
  	else if ((term->transm_status & DEV_TERM_STATUS) == 1) tprint ("T1\n");
  	else if ((term->transm_status & DEV_TERM_STATUS) == 2) tprint ("T2\n");
  	else if ((term->transm_status & DEV_TERM_STATUS) == 3) tprint ("T3\n");
  	else if ((term->transm_status & DEV_TERM_STATUS) == 4) tprint ("T4\n");
  	else if ((term->transm_status & DEV_TERM_STATUS) == 5) tprint ("T5\n");

	tprint("Here3\n");
	if((term->recv_status & DEV_TERM_STATUS) == DEV_TRCV_S_CHARRECV){
		term->recv_command = DEV_C_ACK;
		tprint("Here4\n");
	}else if((term->transm_status & DEV_TERM_STATUS) == DEV_TTRS_S_CHARTRSM){
		term->transm_command = DEV_C_ACK;

	}
	tprint("Here6\n");
	//SYSCALL(SEMV, (unsigned int)currentProcess, 0, 0);
	insertProcQ(&readyQueues[1], currentProcess);
	//tprint("Here7\n");
	/*
	Softcount --
	gestione semafori e ready queue
	*/

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