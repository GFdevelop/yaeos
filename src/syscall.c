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


#include "libuarm.h"
#include "pcb.h"
#include "asl.h"
#include "syscall.h"


int createprocess(){
	extern pcb_t *currentPCB;
	pcb_t *childPCB = allocPcb();
	if (childPCB == NULL) return -1;
	else {
		insertChild(currentPCB, childPCB);
		//memcpy(childPCB->p_s, currentPCB->p_s, 88);
		//childPCB->p_s =  currentPCB->p_s;
		STST(&childPCB->p_s);
		currentPCB->p_s.a4 = (unsigned int)childPCB;
		return 0;
	}
}

int terminateprocess(){
	extern pcb_t *currentPCB;
	pcb_t *head;
	if (currentPCB->p_s.a2 == (int)NULL) head = currentPCB;
	else head = (pcb_t *)currentPCB->p_s.a2;
	forallProcQ(head, (void *)removeProcQ, head->p_first_child);
	removeProcQ(&head);
	return 0;
}

void semv(){
	tprint("semv\n");
	extern pcb_t *currentPCB, *readyQueue;
	insertProcQ(&readyQueue,removeBlocked((int *)currentPCB->p_s.a2));
	unsigned int value = currentPCB->p_s.a2;
	value++;
}

void semp(){
	tprint("semp\n");
	extern pcb_t *currentPCB, *readyQueue;
	extern unsigned int softBlockCount;
	int *value = (int *)currentPCB->p_s.a2;
	if (!value) {
		currentPCB->p_s.cpsr = STATUS_ALL_INT_ENABLE(currentPCB->p_s.cpsr);
		WAIT();
	}
	*value -= 1;
	outProcQ(&readyQueue,currentPCB);
	currentPCB = NULL;
	softBlockCount += 1;
	insertBlocked((int *)currentPCB->p_s.a2, (pcb_t *)currentPCB->p_s.a2);
}

int spechdl(){
	// TODO: only one time for type
	extern pcb_t *currentPCB;
	unsigned int area;
	if (currentPCB->p_s.a2 == SPECSYSBP) area = SYSBK_NEWAREA;
	else if (currentPCB->p_s.a2 == SPECTLB)  area = TLB_NEWAREA;
	else if (currentPCB->p_s.a2 == SPECPGMT)  area = PGMTRAP_NEWAREA;
	else return -1;
	currentPCB->p_s.a3 = area;
	area = currentPCB->p_s.a4;
	return 0;
}

void gettime(){
	
}


void waitclock(){
	extern pcb_t *currentPCB;
	SYSCALL(SEMP, (unsigned int)currentPCB, 0, 0);
}

unsigned int iodevop(){
	extern pcb_t *currentPCB;
	currentPCB->p_s.a3 = currentPCB->p_s.a2;
	SYSCALL(SEMP, (unsigned int)currentPCB, 0, 0);
	return 0;
}

void getpids(){
	extern pcb_t *currentPCB;
	if (currentPCB->p_s.a2 != (unsigned int)NULL) return (void)currentPCB->p_s.a2;
	else if (currentPCB->p_s.a3 != (unsigned int)NULL) return (void)currentPCB->p_s.a3;
}
