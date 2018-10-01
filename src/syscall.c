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
#include "exceptions.h"
#include "scheduler.h"

/*
	SYS1: SYSCALL(CREATEPROCESS, state t *statep, int priority, void **cpid)
	Creation of a new process. A PCB is allocated and the various arguments are copied into the appropriate fields of the new process' PCB.
	The newly created process is inserted both in the readyQueue and in the list of the children of the calling process.
	System variable processCount is updated.
*/
void createprocess(){
	extern pcb_t *currentPCB, *readyQueue;
	extern unsigned int processCount;

	pcb_t *childPCB = allocPcb();
	if (childPCB == NULL) currentPCB->p_s.a1 = -1;
	else {
		SVST((state_t *)currentPCB->p_s.a2, &childPCB->p_s);
		childPCB->p_priority = currentPCB->p_s.a3;
		insertChild(currentPCB, childPCB);
		insertProcQ(&readyQueue,childPCB);
		processCount++;
		*(pcb_t **)currentPCB->p_s.a4 = childPCB;
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
		if ((head->p_semKey >= &semDev[0]) && (head->p_semKey <= &semDev[MAX_DEVICES])) softBlock--;
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

void terminateprocess(){	// TODO: terminateprocess was rewrited, I have to work on it
	//~ tprint("terminateprocess\n");
	extern pcb_t *currentPCB;

	pcb_t *head;

	if ((pcb_t *)currentPCB->p_s.a2 == NULL) head = currentPCB;
	else head = (pcb_t *)currentPCB->p_s.a2;

	terminator(head);

	// TODO: are right?!?!?
	if ((currentPCB != NULL) && (head->p_parent != NULL)) currentPCB->p_s.a1 = 0;
	//~ else currentPCB->p_s.a1 = -1;
}

/*
	SYS3: SYSCALL(SEMP, int *semaddr)
	Perform a P operation on a semaphore.
	Semaphore value is decreased by one and then checked to see if its value is less than zero (which means that calling process have to wait until a V occours).
	If value < 0, calling process is placed into the queue of blocked process of the semaphore and currentPCB is set to NULL.
	If the address of the semaphore is between the device range, system variable softBlock is updated.
*/
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

/*
	SYS4: SYSCALL(SEMV, int *semaddr)
	Perform a V operation on a semaphore.
	The value is check to see if its value is less than 1 (anf then increased by 1).
	If the above condition is satisfied (which means that there are blocked process waiting for a V on that semaphore), first process of blocked
	queue is removed and placed back into readyQueue.
	If the address of the semaphore is between the device range, system variable softBlock is updated.
*/
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

/*
	SYS5: SYSCALL(SPECHDL, int type, state_t *old, state_t *new)
	Perform a V operation on a semaphore.
	The value is check to see if its value is less than 1 (anf then increased by 1).
	If the above condition is satisfied (which means that there are blocked process waiting for a V on that semaphore), first process of blocked
	queue is removed and placed back into readyQueue.
	If the address of the semaphore is between the device range, system variable softBlock is updated.
*/
void spechdl(){
	extern pcb_t *currentPCB;

	if(currentPCB->specTrap[currentPCB->p_s.a2] != (memaddr)NULL) currentPCB->p_s.a1 = -1;
	else {
		currentPCB->specTrap[currentPCB->p_s.a2] = (memaddr)currentPCB->p_s.a3;
		currentPCB->specTrap[currentPCB->p_s.a2 + SPECNEW] = (memaddr)currentPCB->p_s.a4;
		currentPCB->p_s.a1 = 0;
	}
}

void gettime(){
	//~ tprint("gettime\n");
	extern pcb_t *currentPCB;
	extern cpu_t checkpoint;

	currentPCB->kernel_time += getTODLO() - checkpoint;

	*(cpu_t *)currentPCB->p_s.a2 = currentPCB->user_time;
	*(cpu_t *)currentPCB->p_s.a3 = currentPCB->kernel_time;
	*(cpu_t *)currentPCB->p_s.a4 = (getTODLO() - currentPCB->activation_time);
}


void waitclock(){
	//~ tprint("waitclock\n");
	extern pcb_t *currentPCB;
	extern int semDev[MAX_DEVICES];
	extern unsigned int softBlock;//serve?

	currentPCB->p_s.a2 = (memaddr)&semDev[CLOCK_SEM];
	semp();
}

void iodevop(){
	//~ tprint("iodevop\n");
	extern pcb_t *currentPCB;
	extern int semDev[MAX_DEVICES];
	//~ memaddr *cmd;

	int lineNo = LINENO(currentPCB->p_s.a3);
	int devNo = EXT_IL_INDEX(lineNo) * DEV_PER_INT;
	devNo += ( lineNo == IL_TERMINAL ) ? TERMNO(currentPCB->p_s.a3) : DEVICENO(currentPCB->p_s.a3);

	// TODO: device not ready
	*(memaddr *)currentPCB->p_s.a3 = currentPCB->p_s.a2;

	currentPCB->p_s.a2 = (memaddr)&semDev[devNo];
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
	if (currentPCB->p_first_child != NULL){
		currentPCB->p_s.a2 = (unsigned int)&semWaitChild;
		semp();
	}
}
