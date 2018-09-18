#include "interrupts.h"

#include "pcb.h"
#include "asl.h"
#include "scheduler.h"
#include "initial.h"
#include "exceptions.h"

#include <uARMtypes.h>
#include <libuarm.h>
#include <arch.h>

void INT_handler(){
	
	if(currentPCB){
		SVST((state_t *)INT_OLDAREA, &currentPCB->p_s);			//Copy the INT_OLDAREA into currentPCB
		currentPCB->user_time += (getTODLO() - curProc_start);
		kernel_start = getTODLO();
		currentPCB->p_s.pc -= WS;								//Reset the program counter to the previous instruction
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

	if(isPseudo || (getTODLO() - lastPseudo >= PSEUDO_TIME)){
		isPseudo = 0;
		forallBlocked(&semDev[CLOCK_SEM], pseudo_clock, NULL);
		lastPseudo = getTODLO();
	}

	if(isAging){
		isAging = 0;
		forallProcQ(readyQueue, ager, NULL);
		lastAging = getTODLO();
	}else{
		insertProcQ(&readyQueue, currentPCB);
		currentPCB = NULL;
	}
}

void device_HDL(unsigned int device){
	return;
}

void terminal_HDL(){

	termreg_t *term;
	unsigned int terminal_no;

	//1. Determines which sub-device has caused interrupt	
	terminal_no = instanceNo(INT_TERMINAL);

	//2. Determines if the interrupt is for writing or reading (or both) and sends the relative ACK
	term = (termreg_t *)DEV_REG_ADDR(INT_TERMINAL, terminal_no);
	
	if((term->transm_status & DEV_TERM_STATUS) == DEV_TTRS_S_CHARTRSM){
		sendACK(term, TRANSM, EXT_IL_INDEX(INT_TERMINAL) * DEV_PER_INT + DEV_PER_INT + terminal_no);
	}else if((term->recv_status & DEV_TERM_STATUS) == DEV_TRCV_S_CHARRECV){
		sendACK(term, RECV, EXT_IL_INDEX(INT_TERMINAL) * DEV_PER_INT + terminal_no);
	}

}

//Auxiliary function necessary to copy states from OLD areas avoidin 'undefined reference to memcpy' error
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

//Starting from the device, it analyzes the bitmap to understand which sub-device has a pending interrupt
unsigned int instanceNo(int device){
	unsigned int subdev_no = 0;

	memaddr *line = (memaddr *)IDEV_BITMAP_ADDR(device);
	while(*line > 0){
		if(*line & 1) break;
		else{
			subdev_no++;
			*line = *line >> 1;								//Shift is used to navigate the bitmap
		}	
	}
	return subdev_no;
}

//Send the ACK signal and copy the device status to the appropriate fields based on type value
//Then, free the busy device
void sendACK(termreg_t* device, int type, int index){
	pcb_t *firstBlocked = headBlocked(&semDev[index]);
	switch (type) {
		case TRANSM:
			firstBlocked->p_s.a1 = device->transm_status;
			device->transm_command = DEV_C_ACK;
			break;
		case RECV:
			firstBlocked->p_s.a1 = device->recv_status;
			device->recv_command = DEV_C_ACK;
			break;
	}
	currentPCB->p_s.a2 = (unsigned int)&semDev[index];
	semv();
}

void pseudo_clock(){
	insertProcQ(&readyQueue, removeBlocked(&semDev[CLOCK_SEM]));
	softBlock -= 1;
}

//Increase by 1 the priority of each process in the readyQueue
void ager(pcb_t *item, void *args){
	item->p_priority += 1;
}