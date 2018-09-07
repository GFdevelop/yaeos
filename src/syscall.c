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
	tprint("createprocess\n");
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
	tprint("terminateprocess\n");
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
	extern unsigned int softBlockCount;
	int *value = (int *)currentPCB->p_s.a2;
	//insertProcQ(&readyQueue,removeBlocked(value));	// insertProcQ here and on exit from excepionsHDL?!?!?!?
	removeBlocked(value);
	if (softBlockCount) softBlockCount--;
	*value += 1;
}

void semp(){
	tprint("semp\n");
	extern pcb_t *currentPCB;
	extern unsigned int softBlockCount;
	int *value = (int *)currentPCB->p_s.a2;
	*value -= 1;
	if (*value < 0) {
		softBlockCount += 1;
		insertBlocked(value, currentPCB);
		currentPCB = NULL;
	}
}

int spechdl(){
	tprint("spechdl\n");
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
	tprint("gettime\n");
	
}


void waitclock(){
	tprint("waitclock\n");
	extern pcb_t *currentPCB;
	SYSCALL(SEMP, (unsigned int)currentPCB, 0, 0);
}

unsigned int iodevop(){
	tprint("iodevop\n");
	extern pcb_t *currentPCB;
	currentPCB->p_s.a3 = currentPCB->p_s.a2;
	SYSCALL(SEMP, (unsigned int)currentPCB, 0, 0);
	return 0;
}

void getpids(){
	tprint("getpids\n");
	extern pcb_t *currentPCB;
	if (currentPCB->p_s.a2 != (unsigned int)NULL) return (void)currentPCB->p_s.a2;
	else if (currentPCB->p_s.a3 != (unsigned int)NULL) return (void)currentPCB->p_s.a3;
}
