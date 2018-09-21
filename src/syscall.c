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
	tprint("createprocess\n");
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
	tprint("terminateprocess\n");
	extern pcb_t *currentPCB, *readyQueue;
	pcb_t *head/*, *tmp*/;
	extern unsigned int processCount/*, softBlock*/;
	
	if ((pcb_t *)currentPCB->p_s.a2 == NULL) head = currentPCB;
	else head = (pcb_t *)currentPCB->p_s.a2;
	
	if (head == currentPCB) currentPCB = NULL;
	
	//~ tmp = head;
	//~ while (tmp != NULL){	// TODO: all :)
		//~ if (tmp->p_first_child != NULL) tmp = tmp->p_first_child;
		//~ else {
			//~ if (tmp != head) {
				//~ tmp = tmp->p_parent;
	//			removeChild(tmp);
				//~ if (tmp->p_first_child != currentPCB) {
					//~ if (tmp->p_first_child->p_semKey == 0) {
						//~ outChildBlocked(tmp->p_first_child);
					//~ }
					//~ else outProcQ(&readyQueue,tmp->p_first_child);
				//~ }
				//~ if (tmp->p_semKey == 0) {
					//~ removeBlocked(tmp->p_semKey);
					//~ insertProcQ(&readyQueue, tmp);
				//~ }
				//~ outChild(tmp->p_first_child);
				//~ freePcb(tmp->p_first_child);
				//~ processCount--;
			//~ }
			//~ else tmp = NULL;
		//~ }
	//~ }
	//~ if (head != currentPCB) {	// currentPCB isn't blocked and isn't in readyQueue then we skip this
		//~ if (headBlocked(head->p_semKey)) {
			//~ outChildBlocked(head);
			//~ softBlock--;
		//~ }
	//~ //	else outProcQ(&readyQueue,head);	// if head is not blocked remove it
	//~ }
	//~ else currentPCB = NULL;
	
	//~ if (headBlocked(head->p_semKey)) {	// if parent of head is in WAITCHILD then we unlock it
	//	outChildBlocked(head->p_parent);
	//~ tprint("head\n");
		//~ removeBlocked(head->p_parent->p_semKey);
		//~ insertProcQ(&readyQueue, head->p_parent);
		//~ softBlock--;
	//~ }
	
	outChild(head);
	freePcb(head);
	processCount--;
	return 0;
}

void semv(){
	//~ tprint("semv\n");
	extern pcb_t *currentPCB, *readyQueue;
	int *value = (int *)currentPCB->p_s.a2;
	//~ if ((*value)++ < 0) {
	if (headBlocked(value)) {
		pcb_t *tmp = removeBlocked(value);
		tmp->p_semKey = NULL;
		insertProcQ(&readyQueue, tmp);
		(*value)++;
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

void gettime(){
	tprint("gettime\n");
	
}


void waitclock(){
	tprint("waitclock\n");
	extern pcb_t *currentPCB;
	extern int semDev[MAX_DEVICES];
	extern unsigned int softBlock;
	
	//semp
	semDev[CLOCK_SEM]--;
	insertBlocked(&semDev[CLOCK_SEM], currentPCB);
	softBlock += 1;
	currentPCB = NULL;
}

void iodevop(){
	//~ tprint("iodevop\n");
	extern pcb_t *currentPCB;
	extern int semDev[MAX_DEVICES];
	unsigned int subdev_no = 0;
	extern unsigned int softBlock;
	termreg_t *term = (termreg_t *)(currentPCB->p_s.a3 - 2*WS);		// why?????
	// TODO: device
	subdev_no = instanceNo(LINENO(currentPCB->p_s.a3 - 2*WS));
	
	//semp
	semDev[EXT_IL_INDEX(INT_TERMINAL)*DEV_PER_INT+ DEV_PER_INT + subdev_no]--;
	insertBlocked(&semDev[EXT_IL_INDEX(INT_TERMINAL)*DEV_PER_INT+ DEV_PER_INT + subdev_no], currentPCB);
	softBlock += 1;
	
	term->transm_command = currentPCB->p_s.a2;
	
	//semp
	currentPCB = NULL;
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
	if (currentPCB->p_first_child != NULL){	// if no child, don't wait
		//~ tprint("wait terminatechild\n");
		//~ currentPCB->p_s.pc -= WORD_SIZE;
		currentPCB = NULL;
	}
}
