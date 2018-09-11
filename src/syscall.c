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
#include "arch.h"
#include "pcb.h"
#include "asl.h"
#include "syscall.h"
#include "initial.h"
#include "interrupts.h"
#include "scheduler.h"


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

int terminateprocess(unsigned int a2){
	tprint("terminateprocess\n");
	extern pcb_t *currentPCB;
	pcb_t *head;
	if (currentPCB->p_s.a2 == (int)NULL) head = currentPCB;
	else head = (pcb_t *)currentPCB->p_s.a2;
	forallProcQ(head, (void *)removeProcQ, head->p_first_child);
	removeProcQ(&head);
	return 0;
}

void semv(unsigned int a2){
	//~ tprint("semv\n");
	extern pcb_t *currentPCB;
	extern unsigned int softBlockCount;
	//~ state_t *old = (state_t*)SYSBK_OLDAREA;
	//~ int *value = (int *)(old->a2);
	//~ int *value = (int *)(currentPCB->p_s.a2);
	if ((*(int *)a2)++ < 0) {
		//~ tprint("unlocked\n");
		softBlockCount--;
		currentPCB = removeBlocked((int *)a2);
	}
}

void semp(unsigned int a2){
	//~ tprint("semp\n");
	extern pcb_t *currentPCB;
	extern unsigned int softBlockCount;
	//~ state_t *old = (state_t*)SYSBK_OLDAREA;
	//~ int *value = (int *)(old->a2);
	//~ int *value = (int *)(currentPCB->p_s.a2);
	if (--(*(int *)a2) < 0) {
		//~ tprint("locked\n");
		softBlockCount += 1;
		insertBlocked((int *)a2, currentPCB);
		currentPCB = NULL;
		scheduler();
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

void iodevop(unsigned int a2, unsigned int a3){
	tprint("iodevop\n");
	extern pcb_t *currentPCB;
	extern int semDev[MAX_DEVICES];
	unsigned int device_no = 0;
	termreg_t *term = (termreg_t *)(a3 - 2*WS);		// why?????
	//~ SVST((state_t*)SYSBK_OLDAREA,&currentPCB->p_s);
	device_no = findLineNo(LINENO(a3 - 2*WS));
	semp((unsigned int)&semDev[EXT_IL_INDEX(INT_TERMINAL)*DEV_PER_INT+ DEV_PER_INT + device_no]);
	//~ if((a2 & DEV_TERM_STATUS) == DEV_TRCV_S_CHARRECV){
		//~ term->recv_command = a2;
	//~ }else if((a2 & DEV_TERM_STATUS) == DEV_TTRS_S_CHARTRSM){
		term->transm_command = a2;
	//~ }
	currentPCB->p_s.a1 = term->transm_status;
	//~ return currentPCB->p_s.a1;
}

void getpids(){
	tprint("getpids\n");
	extern pcb_t *currentPCB;
	if (currentPCB->p_s.a2 != (unsigned int)NULL) return (void)currentPCB->p_s.a2;
	else if (currentPCB->p_s.a3 != (unsigned int)NULL) return (void)currentPCB->p_s.a3;
}
