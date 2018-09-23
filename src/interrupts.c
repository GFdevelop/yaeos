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
	//~ setSTATUS(STATUS_ALL_INT_DISABLE(getSTATUS()));
	//~ tprint("intHandler\n");
	extern pcb_t *currentPCB;

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

	//~ tprint("end\n");
	//~ if (currentPCB) LDST((state_t *)INT_OLDAREA);
	scheduler();
}

int pending() {
	unsigned int cause = getCAUSE();
	if(CAUSE_IP_GET(cause, INT_LOWEST)) {
		device_HDL(INT_LOWEST);
		return 1;
	}
	else if(CAUSE_IP_GET(cause, INT_DISK)) {
		device_HDL(INT_DISK);
		return 1;
	}
	else if(CAUSE_IP_GET(cause, INT_TAPE)) {
		device_HDL(INT_TAPE);
		return 1;
	}
	else if(CAUSE_IP_GET(cause, INT_UNUSED)) {
		device_HDL(INT_UNUSED);
		return 1;
	}
	else if(CAUSE_IP_GET(cause, INT_PRINTER)) {
		device_HDL(INT_PRINTER);
		return 1;
	}
	else if(CAUSE_IP_GET(cause, INT_TERMINAL)) {
		terminal_HDL();
		return 1;
	}
	else return 0;
}

void ticker(pcb_t *removed, void *nil){
	extern pcb_t *currentPCB, *readyQueue;
	extern unsigned int softBlock;
	extern int semDev[MAX_DEVICES];

	outChildBlocked(removed);
	removed->p_semKey = NULL;
	insertProcQ(&readyQueue, removed);
	//~ if (semDev[CLOCK_SEM] < 0) {
		softBlock--;
		semDev[CLOCK_SEM] = semDev[CLOCK_SEM] + 1;
	//~ }
}

void timer_HDL(){
	//~ tprint("timer_HDL\n");
	extern pcb_t *currentPCB, *readyQueue;
	extern int semDev[MAX_DEVICES];
	extern cpu_t slice, tick, interval;
	extern unsigned int softBlock;

	if (getTODLO() >= (slice + SLICE_TIME)){
		//~ tprint("|");
		insertProcQ(&readyQueue, currentPCB);
		currentPCB = NULL;
		slice = getTODLO();
	}

	if (getTODLO() >= (tick + TICK_TIME)){
		//~ tprint("?");
		//~ if (currentPCB == NULL) currentPCB = headBlocked(&semDev[CLOCK_SEM]);
		//~ if (headBlocked(&semDev[CLOCK_SEM])) forallBlocked(&semDev[CLOCK_SEM], ticker, NULL);
		pcb_t *removed;
		currentPCB = headBlocked(&semDev[CLOCK_SEM]);
		while ((removed = removeBlocked(&semDev[CLOCK_SEM]))){
				removed->p_semKey = NULL;
				insertProcQ(&readyQueue, removed);
				softBlock--;
				semDev[CLOCK_SEM] = semDev[CLOCK_SEM] + 1;
		}
		tick = getTODLO();
	}

	interval = MIN(slice + SLICE_TIME, tick + TICK_TIME);
	setTIMER(interval - getTODLO());
}

void device_HDL(){
	tprint("device_HDL\n");
}

void terminal_HDL(){
	//~ tprint ("terminal_HDL\n");
	devreg_t *generic;
	unsigned int terminal_no = 0;

	terminal_no = instanceNo(INT_TERMINAL);

	//2. Determinare se l'interrupt deriva da una scrittura, una lettura o entrambi
	generic = (devreg_t *)DEV_REG_ADDR(INT_TERMINAL, terminal_no);

	if ((generic->term.transm_status & DEV_TERM_STATUS) == DEV_TTRS_S_CHARTRSM){
		sendACK(generic, TRANSM, EXT_IL_INDEX(INT_TERMINAL) * DEV_PER_INT + DEV_PER_INT + terminal_no);
	} else if ((generic->term.recv_status & DEV_TERM_STATUS) == DEV_TRCV_S_CHARRECV){
		sendACK(generic, RECV, EXT_IL_INDEX(INT_TERMINAL) * DEV_PER_INT + terminal_no);
	} else {
 		sendACK(generic, GENERIC, EXT_IL_INDEX(INT_TERMINAL) * DEV_PER_INT + terminal_no);
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

	//1. Determinare quale dei teminali ha generato l'interrupt
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
void sendACK(devreg_t* device, int type, int index){
	extern pcb_t *readyQueue;
	extern int semDev[MAX_DEVICES];
	extern unsigned int softBlock;

	pcb_t *firstBlocked = removeBlocked(&semDev[index]);
	//if(firstBlocked == NULL) WAIT();
	switch (type) {
		case TRANSM:
			firstBlocked->p_s.a1 = device->term.transm_status;
			device->term.transm_command = DEV_C_ACK;
			break;
		case RECV:
			firstBlocked->p_s.a1 = device->term.recv_status;
			device->term.recv_command = DEV_C_ACK;
			break;
		case GENERIC:
 			firstBlocked->p_s.a1 = device->dtp.status;
 			device->dtp.command = DEV_C_ACK;
 			break;
	}

	//semv
	insertProcQ(&readyQueue, firstBlocked);
	firstBlocked->p_semKey = NULL;
	//~ if (semDev[index] < 0) {
		softBlock--;
		semDev[index] = semDev[index] + 1;
	//~ }
}
