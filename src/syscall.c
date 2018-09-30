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
#include <libuarm.h>
#include <arch.h>

#include "pcb.h"
#include "asl.h"

#include "initial.h"
#include "syscall.h"
#include "interrupts.h"
#include "exceptions.h"
#include "scheduler.h"


void createprocess(){
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

void terminator(pcb_t *head){
	extern pcb_t *currentPCB, *readyQueue;
	extern unsigned int processCount, softBlock;
	extern int semDev[MAX_DEVICES];
	extern int semWaitChild;
	
	if (head->p_first_child != NULL) terminator(head->p_first_child);
	
	if (head->p_semKey != NULL) {
		if ((head->p_semKey >= &semDev[0]) && (head->p_semKey <= &semDev[MAX_DEVICES - 1])) softBlock--;
		if((*head->p_semKey) < 0) (*head->p_semKey)++;
		outChildBlocked(head);
	}
	else if (head != currentPCB) outProcQ(&readyQueue, head);
	else currentPCB = NULL;
	
	if ((head->p_parent != NULL) && (head->p_parent->p_semKey == &semWaitChild)) {
		outChildBlocked(head->p_parent);
		insertProcQ(&readyQueue,head->p_parent);
		semWaitChild++;
	}
	
	outChild(head);
	freePcb(head);
	processCount--;
}

void terminateprocess(){
	extern pcb_t *currentPCB;
	
	pcb_t *head;
	
	if ((pcb_t *)currentPCB->p_s.a2 == NULL) head = currentPCB;
	else head = (pcb_t *)currentPCB->p_s.a2;
	
	terminator(head);
	
	if (currentPCB != NULL) {
		if (head == NULL) currentPCB->p_s.a1 = 0;
		else currentPCB->p_s.a1 = -1;
	}
}

void semv(){
	extern pcb_t *currentPCB, *readyQueue;
	extern unsigned int softBlock;
	extern int semDev[MAX_DEVICES];
	extern cpu_t checkpoint;
	
	int *value = (int *)currentPCB->p_s.a2;
	if ((*value)++ < 1) {
		pcb_t *tmp = removeBlocked(value);
		insertProcQ(&readyQueue, tmp);
		if ((value >= semDev) && (value <= &semDev[MAX_DEVICES - 1])) softBlock--;
		
		currentPCB->kernel_time += getTODLO() - checkpoint;
		checkpoint = getTODLO();
	}
}

void semp(){
	extern pcb_t *currentPCB;
	extern unsigned int softBlock;
	extern int semDev[MAX_DEVICES];
	extern cpu_t checkpoint;
	
	int *value = (int *)currentPCB->p_s.a2;
	if (--(*value) < 0) {
		if (insertBlocked(value, currentPCB)) PANIC();
		if ((value >= semDev) && (value <= &semDev[MAX_DEVICES - 1])) softBlock++;
		
		currentPCB->kernel_time += getTODLO() - checkpoint;
		checkpoint = getTODLO();
		
		currentPCB = NULL;
	}
}

void spechdl(){
	extern pcb_t *currentPCB;
	
	if ((currentPCB->specTrap[currentPCB->p_s.a2] != (memaddr)NULL) ||
		(currentPCB->p_s.a3 == (memaddr)NULL) || (currentPCB->p_s.a4 == (memaddr)NULL)) {
		currentPCB->p_s.a1 = -1;
	}
	else {
		currentPCB->specTrap[currentPCB->p_s.a2] = currentPCB->p_s.a3;
		currentPCB->specTrap[currentPCB->p_s.a2 + SPECNEW] = currentPCB->p_s.a4;
		currentPCB->p_s.a1 = 0;
	}
}

void gettime(){
	extern pcb_t *currentPCB;
	extern cpu_t checkpoint;
	
	currentPCB->kernel_time += getTODLO() - checkpoint;
	
	*(cpu_t *)currentPCB->p_s.a2 = currentPCB->user_time;
	*(cpu_t *)currentPCB->p_s.a3 = currentPCB->kernel_time;
	*(cpu_t *)currentPCB->p_s.a4 = (getTODLO() - currentPCB->activation_time);
}


void waitclock(){
	extern pcb_t *currentPCB;
	extern int semDev[MAX_DEVICES];
	extern unsigned int softBlock;
	
	currentPCB->p_s.a2 = (memaddr)&semDev[CLOCK_SEM];
	semp();
}

void iodevop(){
	extern pcb_t *currentPCB;
	extern int semDev[MAX_DEVICES];
	//~ memaddr *cmd;
	
	int lineNo = LINENO(currentPCB->p_s.a3);
	int devNo = ( lineNo == IL_TERMINAL ) ? TERMNO(currentPCB->p_s.a3) : DEVICENO(currentPCB->p_s.a3);
	
	/*if ((*((memaddr *)currentPCB->p_s.a3 - 1)) != DEV_S_READY) currentPCB->p_s.pc -= WORD_SIZE;		// -1 is status field
	else */*(memaddr *)currentPCB->p_s.a3 = currentPCB->p_s.a2;
	
	currentPCB->p_s.a2 = (memaddr)&semDev[EXT_IL_INDEX(lineNo) * DEV_PER_INT + devNo];
	semp();
	
	
	//~ int lineNo = LINENO(currentPCB->p_s.a3);
	
	//~ int subDevNo = TERMNO(currentPCB->p_s.a3);
	//~ int devNo = INDEVNO(subDevNo);
	//~ devreg_t *device = (devreg_t *)DEV_REG_ADDR(lineNo, devNo);
	
	//~ if ((lineNo >= IL_DISK) && (lineNo < IL_TERMINAL)) cmd = &device->dtp.command;
	//~ else {
		//~ devNo = subDevNo;
		//~ cmd = ( TERMTYPE(subDevNo) ) ? &device->term.transm_command : &device->term.recv_command;
	//~ }
	
	//~ devNo += EXT_IL_INDEX(lineNo) * DEV_PER_INT;
	
	//~ currentPCB->p_s.a2 = (unsigned int)&semDev[devNo];
	//~ semp();
	//~ *cmd = ((state_t *)SYSBK_OLDAREA)->a2;
}

void getpids(){
	extern pcb_t *currentPCB;
	if (currentPCB->p_parent == NULL) {	// root
		if ((pcb_t **)currentPCB->p_s.a2 != NULL) *(pcb_t **)currentPCB->p_s.a2 = NULL;
		if ((pcb_t **)currentPCB->p_s.a3 != NULL) *(pcb_t **)currentPCB->p_s.a3 = NULL;
	}
	else {	// one process
		if ((pcb_t **)currentPCB->p_s.a2 != NULL) *(pcb_t **)currentPCB->p_s.a2 = currentPCB;
		if ((pcb_t **)currentPCB->p_s.a3 != NULL) {
			if (currentPCB->p_parent->p_parent == NULL) *(pcb_t **)currentPCB->p_s.a3 = NULL;	//if parent is root
			else *(pcb_t **)currentPCB->p_s.a3 = currentPCB->p_parent;
		}
	}
}

void waitchild(){
	extern pcb_t *currentPCB;
	extern int semWaitChild;
	if (currentPCB->p_first_child != NULL){
		currentPCB->p_s.a2 = (unsigned int)&semWaitChild;
		semp();
	}
}
