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
#include "syscall.h"

void intHandler(){
	//~ tprint("intHandler\n");
	extern pcb_t *currentPCB;
	extern cpu_t elapsed, lastTime;

	elapsed = getTODLO() - lastTime;
	lastTime = getTODLO();
	//~ currentPCB->user_time += elapsed;

	if (currentPCB != NULL) {
		((state_t *)INT_OLDAREA)->pc -= WORD_SIZE;
		SVST((state_t *)INT_OLDAREA, &currentPCB->p_s);
	}

	unsigned int cause = getCAUSE();

	if(CAUSE_IP_GET(cause, INT_TIMER)) timer_HDL();
	else if(CAUSE_IP_GET(cause, INT_LOWEST)) device_HDL(INT_LOWEST);
	else if(CAUSE_IP_GET(cause, INT_DISK)) device_HDL(INT_DISK);
	else if(CAUSE_IP_GET(cause, INT_TAPE)) device_HDL(INT_TAPE);
	else if(CAUSE_IP_GET(cause, INT_UNUSED)) device_HDL(INT_UNUSED);
	else if(CAUSE_IP_GET(cause, INT_PRINTER)) device_HDL(INT_PRINTER);
	else if(CAUSE_IP_GET(cause, INT_TERMINAL)) terminal_HDL();
	else PANIC();

	scheduler();
}

void timer_HDL(){
	extern pcb_t *currentPCB, *readyQueue;
	extern int semDev[MAX_DEVICES];
	extern cpu_t slice, tick, interval, elapsed;

	//~ currentPCB->kernel_time -= getTODLO() - elapsed;

	if (getTODLO() >= (slice + SLICE_TIME)){
		if (currentPCB){
			insertProcQ(&readyQueue, currentPCB);
			currentPCB = NULL;
		}
		slice = getTODLO();
	}

	if (getTODLO() >= (tick + TICK_TIME)){
		while ((semDev[CLOCK_SEM]) < 0) {
			currentPCB->p_s.a2 = (unsigned int)&semDev[CLOCK_SEM];
			semv();
		}
		tick = getTODLO();
	}

	interval = MIN(slice + SLICE_TIME, tick + TICK_TIME);
	interval = interval + (interval - elapsed);
	setTIMER(interval - getTODLO());
}

void device_HDL(){
	//
}

void terminal_HDL(){
	//~ tprint ("terminal_HDL\n");
	devreg_t *term;
	unsigned int terminal_no = 0;

	//1. Determinare quale dei teminali ha generato l'interrupt
	terminal_no = instanceNo(INT_TERMINAL);

	//2. Determinare se l'interrupt deriva da una scrittura, una lettura o entrambi
	term = (devreg_t *)DEV_REG_ADDR(INT_TERMINAL, terminal_no);

	if((term->term.transm_status & DEV_TERM_STATUS) == DEV_TTRS_S_CHARTRSM){
		sendACK(term, TRANSM, EXT_IL_INDEX(INT_TERMINAL) * DEV_PER_INT + DEV_PER_INT + terminal_no);
	}else if((term->term.recv_status & DEV_TERM_STATUS) == DEV_TRCV_S_CHARRECV){
		sendACK(term, RECV, EXT_IL_INDEX(INT_TERMINAL) * DEV_PER_INT + terminal_no);
	}
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

unsigned int instanceNo(unsigned int device){
	memaddr *line;
	unsigned int subdev_no = 0;

	line = (memaddr *)IDEV_BITMAP_ADDR(device);
	while(*line > 0){
		if(*line & 1) break;
		else{
			subdev_no++;
			*line = *line >> 1;
		}
	}
	return subdev_no;
}

//Copia il comando ACK nel registro transm/recv.command del device specificato a seconda di type
void sendACK(devreg_t *device, int type, int index){
	extern pcb_t *currentPCB, *readyQueue;
	extern int semDev[MAX_DEVICES];
	extern unsigned int softBlock;

	switch (type) {
		case TRANSM:
			((state_t *)INT_OLDAREA)->a1 = device->term.transm_status;
			device->term.transm_command = DEV_C_ACK;
			break;
		case RECV:
			((state_t *)INT_OLDAREA)->a1 = device->term.recv_status;
			device->term.recv_command = DEV_C_ACK;
			break;
		case GENERIC:
			((state_t *)INT_OLDAREA)->a1 = device->dtp.status;
			device->dtp.command = DEV_C_ACK;
			break;
	}


	// if readyQueue isn't empty the scheduler get the currentPCB from readyQueue and then execute the interrupt (before LDST ?!?)
	// I write this code because is good pratice not to write the same things in several places

	if ((semDev[index]) < 0) {	// tprint don't lock any process, then we skip this
		pcb_t * save = NULL;
		if (currentPCB) save = currentPCB;	// there is a process that start after this interrupt
		currentPCB = headBlocked(&semDev[index]);
		currentPCB->p_s.a1 = ((state_t *)INT_OLDAREA)->a1;	// set return value in the right process
		currentPCB->p_s.a2 = (unsigned int)&semDev[index];	// set value for semv()
		semv();
		currentPCB = save;	// restore
	}
}
