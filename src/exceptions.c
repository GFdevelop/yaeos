#include "exceptions.h"

#include "pcb.h"
#include "asl.h"
#include "scheduler.h"
#include "interrupts.h"
#include "initial.h"

#include <arch.h>
#include <uARMtypes.h>
#include <libuarm.h>

void TLB_handler(){
	if(currentPCB){
		currentPCB->user_time += (getTODLO() - curProc_start);
		kernel_start = getTODLO();
	}
	tprint("TLB\n");
	scheduler();
}

void PGMT_handler(){
	if(currentPCB){
		currentPCB->user_time += (getTODLO() - curProc_start);
		kernel_start = getTODLO();
	}
	tprint("PGMT\n");
	scheduler();
}
//TODO: Syscalls ok, but brakpoint?
void SYSBK_handler(){

	if(currentPCB){
		SVST((state_t *) SYSBK_OLDAREA, &currentPCB->p_s);
		currentPCB->user_time += (getTODLO() - curProc_start);
		kernel_start = getTODLO();		
	}
	
	switch(((state_t *) SYSBK_OLDAREA)->a1){
		case CREATEPROCESS:
			createprocess();
			break;
		case TERMINATEPROCESS:
			terminateprocess();
			break;
		case SEMP:
			semp();
			break;
		case SEMV:
			semv();
			break;
		case SPECHDL:
			tprint("SPECHDL\n");
			spechdl();
			break;
		case GETTIME:
			gettime();
			break;
		case WAITCLOCK:
			waitclock();
			break;
		case IODEVOP:
			iodevop();
			break;
		case GETPIDS:
			getpids();
			break;
		case WAITCHLD:
			waitchld();
			break;
		default:
			PANIC();
	}

	scheduler();
}

int createprocess(){
	pcb_t *new = allocPcb();
	if (new == NULL) return -1;
	else {
		SVST((state_t *)currentPCB->p_s.a2,&new->p_s);
		new->p_priority = currentPCB->p_s.a3;
		insertChild(currentPCB, new);
		*(pcb_t **)currentPCB->p_s.a4 = new;
		insertProcQ(&readyQueue,new);
		processCount += 1;
		return 0;
	}
}

int terminateprocess(){
	pcb_t *head, *tmp;
	
	if ((pcb_t *)currentPCB->p_s.a2 == NULL) head = currentPCB;
	else head = (pcb_t *)currentPCB->p_s.a2;
	tmp = head;
	while (tmp != NULL){	// TODO: all :)
		if (tmp->p_first_child != NULL) tmp = tmp->p_first_child;
		else {
			if (tmp != head) {
				tmp = tmp->p_parent;
				removeChild(tmp);
				if (headBlocked((int *)&(tmp->p_first_child))) {
					freePcb(removeBlocked((int *)&(tmp->p_first_child)));
					softBlock -= 1;
				}
				else if (tmp->p_first_child != currentPCB) freePcb(outProcQ(&readyQueue,tmp->p_first_child));
				else freePcb(tmp->p_first_child);
				processCount -= 1;
			}
			else tmp = NULL;
		}
	}
	if (head != currentPCB) {	// currentPCB isn't blocked and isn't in readyQueue then we skip this
		if (headBlocked(head->p_semKey)) {	// if head is blocked we don't remove it from readyQueue
			outChildBlocked(head);
			softBlock -= 1;
		}
		else outProcQ(&readyQueue,head);	// if head is not blocked remove it
	}
	
	if (headBlocked((int *)head->p_parent)) {	// if parent of head is in WAITCHILD then we unlock it
		removeBlocked((int *)head->p_parent);
		softBlock -= 1;
		insertProcQ(&readyQueue, head->p_parent);
	}
	
	outChild(head);
	freePcb(head);
	processCount -= 1;
	return 0;
}

void semv(){
    int *value = (int *)currentPCB->p_s.a2;
    if(headBlocked(value) != NULL) {
        softBlock -= 1;
        insertProcQ(&readyQueue, removeBlocked(value));
    }
    if(*value <= 1) *value += 1; //Non va bene
}
 
void semp(){
    int *value = (int *)currentPCB->p_s.a2;
    if (((*value) <= 0) || ((value >= semDev) && (value <= &semDev[MAX_DEVICES]))){
        insertBlocked(value, currentPCB);
        softBlock += 1;
        currentPCB = NULL;
    }else *value -= 1;
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
	currentPCB->p_s.a2 = currentPCB->user_time;
	currentPCB->p_s.a3 = ((getTODLO() - kernel_start) + currentPCB->kernel_time);
	currentPCB->p_s.a4 = getTODLO() - currentPCB->activation_time;
}

void waitclock(){
	currentPCB->p_s.a2 = (unsigned int)&semDev[CLOCK_SEM];
	semp();
}

unsigned int iodevop(){
	unsigned int subdev_no, device = currentPCB->p_s.a3;
	unsigned int command = currentPCB->p_s.a2;
	
	termreg_t *term = (termreg_t *) (device - 2*WS);
	subdev_no = instanceNo(LINE_NO(device - 2*WS));
	//TODO: Differenziare lettori - scrittori
	currentPCB->p_s.a2 = (unsigned int)&semDev[EXT_IL_INDEX(INT_TERMINAL)*DEV_PER_INT+ DEV_PER_INT + subdev_no];
	
	semp();

	term->transm_command = command;
	//return term->transm_status;
}

void getpids(){
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

void waitchld(){
	if (currentPCB->p_first_child != NULL){	// if no child, don't wait
		softBlock += 1;
		insertBlocked((int *)currentPCB, currentPCB);
		currentPCB = NULL;
	}
}