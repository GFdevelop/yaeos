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


int createprocess(){
	//~ tprint("createprocess\n");
	extern pcb_t *currentPCB, *readyQueue;
	extern unsigned int processCount;
	pcb_t *childPCB = allocPcb();
	if (childPCB == NULL) return -1;
	else {
		SVST((state_t *)currentPCB->p_s.a2,&childPCB->p_s);
		childPCB->p_priority = currentPCB->p_s.a3;
		insertChild(currentPCB, childPCB);
		*(pcb_t **)currentPCB->p_s.a4 = childPCB;
		insertProcQ(&readyQueue,childPCB);
		processCount++;
		return 0;
	}
}

int terminateprocess(){
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
				if ((tmp->p_semKey >= &semDev[0]) && (tmp->p_semKey <= &semDev[MAX_DEVICES])) softBlock--;
				if((*tmp->p_semKey) < 0) (*tmp->p_semKey)++;
				tmp->p_semKey = NULL;
			}
			else if (tmp != currentPCB) {
				if (!outProcQ(&readyQueue, tmp)) return -1;
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

			if (tmp->p_sib != NULL) {
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
	}

	return ret;
}

void semv(){
	//~ tprint("semv\n");
	extern pcb_t *currentPCB, *readyQueue;
	int *value = (int *)currentPCB->p_s.a2;
	//~ if ((*value)++ < 0) {
	if (++(*value) < 1) {
		pcb_t *tmp = removeBlocked(value);
		insertProcQ(&readyQueue, tmp);
		tmp->p_semKey = NULL;
	}
}

void semp(){
	//~ tprint("semp\n");
	extern pcb_t *currentPCB;
	int *value = (int *)currentPCB->p_s.a2;
	if (--(*value) < 0) {
		//~ tprint("locked\n");
		if (insertBlocked(value, currentPCB)) PANIC();
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

/*
Questa system call restituisce il valore di tre “tempi” del processo:
Il tempo usato dal processo in modalità user
Il tempo usato dal processo in modalità kernel (gestione system call e interrupt relativi al processo)
Il tempo trascorso dalla prima attivazione del processo.
*/
void gettime(){
	//tprint("gettime\n");
}


void waitclock(){
	//~ tprint("waitclock\n");
	extern pcb_t *currentPCB;
	extern int semDev[MAX_DEVICES];
	extern unsigned int softBlock;

	//semp
	semDev[CLOCK_SEM] -= 1;
	if (insertBlocked(&semDev[CLOCK_SEM], currentPCB)) PANIC();
	softBlock += 1;
	currentPCB = NULL;
}

void iodevop(){
	//~ tprint("iodevop\n");
	extern pcb_t *currentPCB;
	extern int semDev[MAX_DEVICES];
	unsigned int subdev_no = 0;
	extern unsigned int softBlock;
	devreg_t *genericDev = (devreg_t *)(currentPCB->p_s.a3 - 2*WS);		// why?????
	// TODO: device
	subdev_no = instanceNo(LINENO(currentPCB->p_s.a3 - 2*WS));

	semDev[EXT_IL_INDEX(INT_TERMINAL)*DEV_PER_INT+ DEV_PER_INT + subdev_no] -= 1;
	insertBlocked(&semDev[EXT_IL_INDEX(INT_TERMINAL)*DEV_PER_INT+ DEV_PER_INT + subdev_no], currentPCB);
	softBlock += 1;
	currentPCB = NULL;
	if (LINENO((unsigned int)genericDev)+1 == INT_TERMINAL /*se è un terminale*/){
 		if ((((LINENO((unsigned int)genericDev)+1) == INT_TERMINAL && subdev_no >> 31) ? N_DEV_PER_IL : 0) == 0){ /*scrittura*/
			genericDev->term.transm_command = ((state_t *)SYSBK_OLDAREA)->a2;
 		} else /*lettura*/ genericDev->term.recv_command = ((state_t *)SYSBK_OLDAREA)->a2;
 	} else genericDev->dtp.command = ((state_t *)SYSBK_OLDAREA)->a2;
	//genericDev->term.transm_command = ((state_t *)SYSBK_OLDAREA)->a2;
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
	//~ tprint("waitchild\n");
	extern unsigned int softBlock;
	extern pcb_t *currentPCB;
	extern int semWaitChild;
	if (currentPCB->p_first_child != NULL){	// if no child, don't wait
		//~ tprint("waitchild\n");
		//~ currentPCB->p_s.pc -= WORD_SIZE;
		semWaitChild--;
		if (insertBlocked(&semWaitChild, currentPCB)) PANIC();
		currentPCB = NULL;
	}
}
