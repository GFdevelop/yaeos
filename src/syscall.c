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
	//~ tprint("createprocess\n");
	extern pcb_t *currentPCB, *readyQueue;
	extern int processCount;
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
	pcb_t *head, *tmp;
	extern int processCount, softBlock;
	
	if ((pcb_t *)currentPCB->p_s.a2 == NULL) head = currentPCB;
	else head = (pcb_t *)currentPCB->p_s.a2;
	tmp = head;
	while (tmp != NULL){	// TODO: all :)
		if (tmp->p_first_child != NULL) tmp = tmp->p_first_child;
		else {
			if (tmp != head) {
				tmp = tmp->p_parent;
				//~ removeChild(tmp);
				if (tmp->p_first_child != currentPCB) {
					if (headBlocked(tmp->p_first_child->p_semKey)) {
						outChildBlocked(tmp->p_first_child);
						softBlock--;
					}
					else outProcQ(&readyQueue,tmp->p_first_child);
				}
				if (headBlocked((int *)tmp)) {
					removeBlocked((int *)tmp);
					softBlock--;
					insertProcQ(&readyQueue, tmp);
				}
				outChild(tmp->p_first_child);
				freePcb(tmp->p_first_child);
				processCount--;
			}
			else tmp = NULL;
		}
	}
	if (head != currentPCB) {	// currentPCB isn't blocked and isn't in readyQueue then we skip this
		if (headBlocked(head->p_semKey)) {	// if head is blocked we don't remove it from readyQueue
			outChildBlocked(head);
			softBlock--;
		}
		else outProcQ(&readyQueue,head);	// if head is not blocked remove it
	}
	
	if (headBlocked((int *)head->p_parent)) {	// if parent of head is in WAITCHILD then we unlock it
		//~ outChildBlocked(head->p_parent);
		removeBlocked((int *)head->p_parent);
		softBlock--;
		insertProcQ(&readyQueue, head->p_parent);
	}
	
	outChild(head);
	freePcb(head);
	processCount--;
	return 0;
}

void semv(){
	//~ tprint("semv\n");
	extern pcb_t *currentPCB, *readyQueue;
	extern int softBlock;
	extern int semDev[MAX_DEVICES];
	//~ state_t *old = (state_t*)SYSBK_OLDAREA;
	//~ int *value = (int *)(old->a2);
	int *value = (int *)currentPCB->p_s.a2;
	if (((*value)++ < 0) || ((value >= semDev) && (value <= &semDev[MAX_DEVICES]))) {
		//~ tprint("unlocked\n");
		softBlock -= 1;
		//~ currentPCB = removeBlocked((int *)a2);
		insertProcQ(&readyQueue, removeBlocked(value));
	}
}

void semp(){
	//~ tprint("semp\n");
	extern pcb_t *currentPCB;
	extern int softBlock;
	extern int semDev[MAX_DEVICES];
	//~ state_t *old = (state_t*)SYSBK_OLDAREA;
	//~ int *value = (int *)(old->a2);
	int *value = (int *)currentPCB->p_s.a2;
	if ((--(*value) < 0) || ((value >= semDev) && (value <= &semDev[MAX_DEVICES]))) {
		//~ tprint("locked\n");
		softBlock += 1;
		insertBlocked(value, currentPCB);
		currentPCB = NULL;
		//~ scheduler();
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
	//~ SYSCALL(SEMP, (unsigned int)currentPCB, 0, 0);
}

void iodevop(){
	//~ tprint("iodevop\n");
	extern pcb_t *currentPCB;
	extern int semDev[MAX_DEVICES];
	unsigned int device_no = 0;
	termreg_t *term = (termreg_t *)(currentPCB->p_s.a3 - 2*WS);		// why?????
	// TODO: device
	device_no = findLineNo(LINENO(currentPCB->p_s.a3 - 2*WS));
	currentPCB->p_s.a2 = (unsigned int)&semDev[EXT_IL_INDEX(INT_TERMINAL)*DEV_PER_INT+ DEV_PER_INT + device_no];
	semp();
	term->transm_command = ((state_t *)SYSBK_OLDAREA)->a2;	// currentPCB->p_s.a2 is changed for the semp()
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
	extern int softBlock;
	extern pcb_t *currentPCB;
	if (currentPCB->p_first_child != NULL){	// if no child, don't wait
		softBlock += 1;
		insertBlocked((int *)currentPCB, currentPCB);
		currentPCB = NULL;
	}
}
