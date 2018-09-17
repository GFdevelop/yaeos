#include "interrupts.h"

#include "pcb.h"
#include "asl.h"
#include "scheduler.h"
#include "initial.h"
#include "exceptions.h"

#include <uARMtypes.h>
#include <libuarm.h>
#include <arch.h>

extern state_t *INT_Old;
extern pcb_t *currentProcess, *readyQueue;
extern unsigned int softBlock, kernel_start, isPseudo;
extern int sem_devices[MAX_DEVICES];
//Hints from pages 130 and 63, uARMconst.h and libuarm.h
void INT_handler(){

	kernel_start = getTODLO();
	INT_Old = (state_t *) INT_OLDAREA;
	if(currentProcess){
		INT_Old->pc = INT_Old->pc - 4;
		SVST(INT_Old, &currentProcess->p_s);
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

	return;
}

//TODO: The lower the line, the higher the priority
void timer_HDL(){
	extern unsigned int isPseudo, isAging;
	extern cpu_t lastPseudo, lastAging;
	extern pcb_t *currentProcess;

	cpu_t timeToPseudo = getTODLO() - lastPseudo;

	if(timeToPseudo >= PSEUDO_TIME || isPseudo){
		isPseudo = 0;
		forallBlocked(&sem_devices[CLOCK_SEM], pseudo_clock, NULL);
		lastPseudo = getTODLO();
	}

	if(isAging){
		isAging = 0;
		forallProcQ(readyQueue, ager, NULL);
		lastAging = getTODLO();
	}else{
		insertProcQ(&readyQueue, currentProcess);
		currentProcess = NULL;
	}
}

void device_HDL(unsigned int device){
	return;
}

void terminal_HDL(){

	termreg_t *term;
	unsigned int terminal_no;

	//1. Determinare quale dei teminali ha generato l'interrupt	
	terminal_no = instanceNo(INT_TERMINAL);

	//2. Determinare se l'interrupt deriva da una scrittura, una lettura o entrambi
	term = (termreg_t *)DEV_REG_ADDR(INT_TERMINAL, terminal_no);
	
	//3. Mandare l'ACK corrispondente
	if((term->transm_status & DEV_TERM_STATUS) == DEV_TTRS_S_CHARTRSM){
		sendACK(term, TRANSM, EXT_IL_INDEX(INT_TERMINAL) * DEV_PER_INT + DEV_PER_INT + terminal_no);
	}else if((term->recv_status & DEV_TERM_STATUS) == DEV_TRCV_S_CHARRECV){
		sendACK(term, RECV, EXT_IL_INDEX(INT_TERMINAL) * DEV_PER_INT + terminal_no);
	}

}

//Necessaria per salvare lo stato del processo dalle aree *_OLD evitando 'undefined reference to memcpy'
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

//Partendo dal device, analizza la bitmap per capire quale sub-device ha un interrupt pendente.
//I sub-device più in basso come numero, hanno priorità maggiore
unsigned int instanceNo(int device){
	unsigned int subdev_no = 0;

	memaddr *line = (memaddr *)IDEV_BITMAP_ADDR(device);
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
void sendACK(termreg_t* device, int type, int index){
	extern int sem_devices[MAX_DEVICES];

	pcb_t *firstBlocked = headBlocked(&sem_devices[index]);
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
	currentProcess->p_s.a2 = (unsigned int)&sem_devices[index];
	semv();
}

void pseudo_clock(){
	extern pcb_t *readyQueue;
	extern int sem_devices[MAX_DEVICES];
	extern unsigned int softBlock;

	insertProcQ(&readyQueue, removeBlocked(&sem_devices[CLOCK_SEM]));
	softBlock -= 1;
}

void ager(pcb_t *item, void *args){
	item->p_priority += 1;
}