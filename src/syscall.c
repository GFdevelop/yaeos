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


#include <libuarm.h>
#include <arch.h>

#include "pcb.h"
#include "asl.h"

#include "initial.h"
#include "syscall.h"
#include "interrupts.h"
#include "scheduler.h"


void createprocess(){
	//~ tprint("createprocess\n");
	extern pcb_t *currentPCB, *readyQueue;
	extern unsigned int processCount;
	pcb_t *childPCB = allocPcb();
	if (childPCB == NULL) currentPCB->p_s.a1 = -1;
	else {
		SVST((state_t *)currentPCB->p_s.a2,&childPCB->p_s);
		childPCB->p_priority = currentPCB->p_s.a3;
		insertChild(currentPCB, childPCB);
		*(pcb_t **)currentPCB->p_s.a4 = childPCB;
		insertProcQ(&readyQueue,childPCB);
		processCount++;
		currentPCB->p_s.a1 = 0;
	}
}

//~ void terminator(pcb_t *head){
	//~ if (head->p_first_child != NULL) {
		//~ terminator(head->p_first_child);
	//~ }
	
	//~ if (head->p_semKey != NULL) {
		//~ outChildBlocked(head);
		//~ if ((head->p_semKey >= &semDev[0]) && (head->p_semKey <= &semDev[MAX_DEVICES])) softBlock--;
		//~ if((*head->p_semKey) < 0) (*head->p_semKey)++;
		//~ head->p_semKey = NULL;
	//~ }
	//~ else if (tmp != currentPCB) {
		//~ if (!outProcQ(&readyQueue, tmp)) return -1;
	//~ }
	//~ else currentPCB = NULL;
	
	//~ if (tmp->p_parent != NULL) {
		//~ if (tmp->p_parent->p_semKey == &semWaitChild){
			//~ outChildBlocked(tmp->p_parent);
			//~ tmp->p_parent->p_semKey = NULL;
			//~ insertProcQ(&readyQueue,tmp->p_parent);
			//~ semWaitChild++;
		//~ }
	//~ }
//~ }

void terminateprocess(){	// TODO: terminateprocess was rewrited, I have to work on it
	//~ tprint("terminateprocess\n");
	extern pcb_t *currentPCB, *readyQueue;
	extern unsigned int processCount, softBlock;
	extern int semDev[MAX_DEVICES];
	extern int semWaitChild;
	
	pcb_t *head, *tmp;
	int ret = 0;
	
	if ((pcb_t *)currentPCB->p_s.a2 == NULL) head = currentPCB;
	else head = (pcb_t *)currentPCB->p_s.a2;
	
	tmp = head;
	
	while (tmp != NULL) {
		if (tmp->p_first_child != NULL) tmp = tmp->p_first_child;
		else {
			if (tmp->p_semKey != NULL) {
				outChildBlocked(tmp);
				if ((tmp->p_semKey >= &semDev[0]) && (tmp->p_semKey <= &semDev[MAX_DEVICES - 1])) softBlock--;
				if((*tmp->p_semKey) < 0) (*tmp->p_semKey)++;
				tmp->p_semKey = NULL;
			}
			else if (tmp != currentPCB) {
				outProcQ(&readyQueue, tmp);
			}
			else currentPCB = NULL;
			
			if (tmp->p_parent != NULL) {
				if (tmp->p_parent->p_semKey == &semWaitChild){
					outChildBlocked(tmp->p_parent);
					tmp->p_parent->p_semKey = NULL;
					insertProcQ(&readyQueue,tmp->p_parent);
					semWaitChild++;
				}
			}
			
			if ((tmp != head) && (tmp->p_parent != NULL)) {
				tmp = tmp->p_parent;
				outChild(tmp->p_first_child);
				freePcb(tmp->p_first_child);
			}
			else {
				outChild(tmp);
				freePcb(tmp);
				tmp = NULL;
			}
			processCount--;
		}
	}
	
	if (currentPCB != NULL) {
		if (head->p_parent != NULL) {
			currentPCB->p_s.a1 = ret;
		}
	}
}

// REMEMBER: sendACK() don't use this function, the trouble is not here!!!
void semv(){
	extern pcb_t *currentPCB, *readyQueue;
	extern unsigned int softBlock;
	extern int semDev[MAX_DEVICES];
	
	int *value = (int *)currentPCB->p_s.a2;
	if ((*value)++ < 1) {
		pcb_t *tmp = removeBlocked(value);
		insertProcQ(&readyQueue, tmp);
		if ((value >= semDev) && (value <= &semDev[MAX_DEVICES - 1])) softBlock--;
	}
}

void semp(){
	extern pcb_t *currentPCB;
	extern unsigned int softBlock;
	extern int semDev[MAX_DEVICES];
	
	int *value = (int *)currentPCB->p_s.a2;
	if (--(*value) < 0) {
		if (insertBlocked(value, currentPCB)) PANIC();
		if ((value >= semDev) && (value <= &semDev[MAX_DEVICES - 1])) softBlock++;
		currentPCB = NULL;
	}
}

int spechdl(){
	tprint("spechdl\n");
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
	//~ tprint("gettime\n");
	extern pcb_t *currentPCB;
	
	*(cpu_t *)currentPCB->p_s.a1 = (getTODLO() - currentPCB->activation_time);
	*(cpu_t *)currentPCB->p_s.a2 = currentPCB->user_time;
	*(cpu_t *)currentPCB->p_s.a3 = currentPCB->kernel_time;
}


void waitclock(){
	//~ tprint("waitclock\n");
	extern pcb_t *currentPCB;
	extern int semDev[MAX_DEVICES];
	extern unsigned int softBlock;
	
	currentPCB->p_s.a2 = (unsigned int)&semDev[CLOCK_SEM];
	semp();
}

void iodevop(){	// TODO: check and fix iodevop()
	//~ tprint("iodevop\n");
	extern pcb_t *currentPCB;
	extern int semDev[MAX_DEVICES];
	unsigned int subdev_no = 0;
	extern unsigned int softBlock;
	devreg_t *genericDev = (devreg_t *)(currentPCB->p_s.a3 - 2*WS);
	subdev_no = instanceNo(LINENO(currentPCB->p_s.a3 - 2*WS));
	
	currentPCB->p_s.a2 = (unsigned int)&semDev[EXT_IL_INDEX(INT_TERMINAL)*DEV_PER_INT+ DEV_PER_INT + subdev_no];
	semp();
	
 	if ((LINENO((unsigned int)genericDev)+1) == INT_TERMINAL ){ /* se è un terminale */
		if (((subdev_no >> 31) ? N_DEV_PER_IL : 0) == 0){
			/* in scrittura */
			genericDev->term.transm_command = ((state_t *)SYSBK_OLDAREA)->a2;
		} else {
			/* in lettura */
			genericDev->term.recv_command = ((state_t *)SYSBK_OLDAREA)->a2;
		}
	} else {
		/* se è un device generico */
		genericDev->dtp.command = ((state_t *)SYSBK_OLDAREA)->a2;
	}
}

void getpids(){
	//~ tprint("getpids\n");
	extern pcb_t *currentPCB;
	if (currentPCB->p_parent == NULL) {
		//~ tprint("root\n");
		if ((pcb_t **)currentPCB->p_s.a2 != NULL) *(pcb_t **)currentPCB->p_s.a2 = NULL;
		if ((pcb_t **)currentPCB->p_s.a3 != NULL) *(pcb_t **)currentPCB->p_s.a3 = NULL;
	}
	else {
		//~ tprint("process\n");
		if ((pcb_t **)currentPCB->p_s.a2 != NULL) *(pcb_t **)currentPCB->p_s.a2 = currentPCB;
		if ((pcb_t **)currentPCB->p_s.a3 != NULL) {
			if (currentPCB->p_parent->p_parent == NULL) *(pcb_t **)currentPCB->p_s.a3 = NULL;	//if parent is root
			else *(pcb_t **)currentPCB->p_s.a3 = currentPCB->p_parent;
		}
	}
}

void waitchild(){
	tprint("waitchild\n");
	extern unsigned int softBlock;
	extern pcb_t *currentPCB;
	extern int semWaitChild;
	if (currentPCB->p_first_child != NULL){
		currentPCB->p_s.a2 = (unsigned int)&semWaitChild;
		semp();
	}
}
