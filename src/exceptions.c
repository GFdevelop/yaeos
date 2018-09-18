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
	if(currentProcess){
		currentProcess->user_time += (getTODLO() - curProc_start);
		kernel_start = getTODLO();
	}
	tprint("TLB\n");
	scheduler();
}

void PGMT_handler(){
	if(currentProcess){
		currentProcess->user_time += (getTODLO() - curProc_start);
		kernel_start = getTODLO();
	}
	tprint("PGMT\n");
	scheduler();
}
//TODO: Syscalls ok, but brakpoint?
void SYSBK_handler(){

	if(currentProcess){
		SVST((state_t *) SYSBK_OLDAREA, &currentProcess->p_s);
		currentProcess->user_time += (getTODLO() - curProc_start);
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
		SVST((state_t *)currentProcess->p_s.a2,&new->p_s);
		new->p_priority = currentProcess->p_s.a3;
		insertChild(currentProcess, new);
		*(pcb_t **)currentProcess->p_s.a4 = new;
		insertProcQ(&readyQueue,new);
		processCount += 1;
		return 0;
	}
}

int terminateprocess(){
	pcb_t *head, *tmp;
	
	if ((pcb_t *)currentProcess->p_s.a2 == NULL) head = currentProcess;
	else head = (pcb_t *)currentProcess->p_s.a2;
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
				else if (tmp->p_first_child != currentProcess) freePcb(outProcQ(&readyQueue,tmp->p_first_child));
				else freePcb(tmp->p_first_child);
				processCount -= 1;
			}
			else tmp = NULL;
		}
	}
	if (head != currentProcess) {	// currentProcess isn't blocked and isn't in readyQueue then we skip this
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
    pcb_t *tmp;
    int *value = (int *)currentProcess->p_s.a2;
    if (headBlocked(value)) {
        softBlock -= 1;
        tmp = removeBlocked(value);
        insertProcQ(&readyQueue, tmp);
    }
    if(*value <= 1) *value += 1;
}
 
void semp(){
    int *value = (int *)currentProcess->p_s.a2;
    if (((*value) <= 0) || ((value >= sem_devices) && (value <= &sem_devices[CLOCK_SEM]))){
        insertBlocked(value, currentProcess);
        softBlock += 1;
        currentProcess = NULL;
    }else *value -= 1;
}

int spechdl(){
	// TODO: only one time for type
	extern pcb_t *currentProcess;
	unsigned int area;
	if (currentProcess->p_s.a2 == SPECSYSBP) area = SYSBK_NEWAREA;
	else if (currentProcess->p_s.a2 == SPECTLB)  area = TLB_NEWAREA;
	else if (currentProcess->p_s.a2 == SPECPGMT)  area = PGMTRAP_NEWAREA;
	else return -1;
	currentProcess->p_s.a3 = area;
	area = currentProcess->p_s.a4;
	return 0;
}

void gettime(){
	currentProcess->p_s.a2 = currentProcess->user_time;
	currentProcess->p_s.a3 = ((getTODLO() - kernel_start) + currentProcess->kernel_time);
	currentProcess->p_s.a4 = getTODLO() - currentProcess->activation_time;
}

void waitclock(){
	currentProcess->p_s.a2 = (unsigned int)&sem_devices[CLOCK_SEM];
	semp();
}

unsigned int iodevop(){
	unsigned int subdev_no, device = currentProcess->p_s.a3;
	unsigned int command = currentProcess->p_s.a2;
	
	termreg_t *term = (termreg_t *) (device - 2*WS);
	subdev_no = instanceNo(LINE_NO(device - 2*WS));
	//TODO: Differenziare lettori - scrittori
	currentProcess->p_s.a2 = (unsigned int)&sem_devices[EXT_IL_INDEX(INT_TERMINAL)*DEV_PER_INT+ DEV_PER_INT + subdev_no];
	
	semp();

	term->transm_command = command;
	return term->transm_status;
}

void getpids(){
	if (currentProcess->p_parent == NULL) {
		//~ tprint("root\n");
		if ((pcb_t **)currentProcess->p_s.a2 != NULL) *(pcb_t **)currentProcess->p_s.a2 = NULL;
		if ((pcb_t **)currentProcess->p_s.a3 != NULL) *(pcb_t **)currentProcess->p_s.a3 = NULL;
	}
	else {
		//~ tprint("process\n");
		if ((pcb_t **)currentProcess->p_s.a2 != NULL) *(pcb_t **)currentProcess->p_s.a2 = currentProcess;
		if ((pcb_t **)currentProcess->p_s.a3 != NULL) {
			if (currentProcess->p_parent->p_parent == NULL) *(pcb_t **)currentProcess->p_s.a3 = NULL;	//if parent is root
			else *(pcb_t **)currentProcess->p_s.a3 = currentProcess->p_parent;
		}
	}
}

void waitchld(){
	if (currentProcess->p_first_child != NULL){	// if no child, don't wait
		softBlock += 1;
		insertBlocked((int *)currentProcess, currentProcess);
		currentProcess = NULL;
	}
}