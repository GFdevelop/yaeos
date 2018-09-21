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
	//tprint("terminateprocess\n");
	extern pcb_t *currentPCB, *readyQueue;
	pcb_t *head, *tmp;
	extern unsigned int processCount, softBlock;

	if ((pcb_t *)currentPCB->p_s.a2 == NULL) head = currentPCB;
	else head = (pcb_t *)currentPCB->p_s.a2;

	if (head == currentPCB) currentPCB = NULL;

	outChild(head);
	freePcb(head);
	processCount--;
	return 0;
}
/*
int terminateprocess(){
    pcb_t *head, *tmp;

    if ((pcb_t *)currentPCB->p_s.a2 == NULL) head = currentPCB;
    else head = (pcb_t *)currentPCB->p_s.a2;
    tmp = head;
    while (tmp != NULL){    // TODO: all :)
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
    if (head != currentPCB) {   // currentPCB isn't blocked and isn't in readyQueue then we skip this
        if (head->p_semKey != NULL) {  // if head is blocked we don't remove it from readyQueue
            outChildBlocked(head);
            softBlock -= 1;
        }
        outProcQ(&readyQueue,head);    // if head is not blocked remove it
    }

    if (head->p_parent->p_semKey != NULL) {   // if parent of head is in WAITCHILD then we unlock it
        outChildBlocked(head->p_parent);
        softBlock -= 1;
        insertProcQ(&readyQueue, head->p_parent);
    }

    outChild(head);
    freePcb(head);
    processCount -= 1;
    return 0;
}*/

pcb_t * findNext(int *value){
    pcb_t *next = removeBlocked(value);
    if (next != NULL) {
        if (next->p_semKey != value) {
            pcb_t *tmp = next;
            next = findNext(value);
            insertBlocked(tmp->p_semKey, tmp);
        }
    }
    return next;
}

void printint_(int a){
	int b = a%10;
    a = a/10;
    if (a>0) printint_(a);
    if (b==0) {tprint("0");} else if (b==1) {tprint("1");}
    else if (b==2) {tprint("2");} else if (b==3) {tprint("3");}
    else if (b==4) {tprint("4");} else if (b==5) {tprint("5");}
    else if (b==6) {tprint("6");} else if (b==7) {tprint("7");}
    else if (b==8) {tprint("8");} else if (b==9) {tprint("9");}
}
void printint(int a){
	if (a<0) {tprint("-"); a = -a;}
    printint_(a);
	tprint("\n");
}

void debuggerSemv(){};
void debuggerSemp(){};

void semv(){
	debuggerSemv();
    int *value = (int *)currentPCB->p_s.a2;
	extern unsigned int softBlock;
    if(headBlocked(value) != NULL) {
        pcb_t *tmp = removeBlocked(value);
        tmp->p_semKey = NULL;
        if ((value >= semDev) && (value <= &semDev[MAX_DEVICES])) softBlock -= 1;
        insertProcQ(&readyQueue, tmp);
    }
    if(headBlocked(value) == NULL) *value += 1; //Non va bene
}

void semp(){
	debuggerSemp();
	extern unsigned int softBlock;
    int *value = (int *)currentPCB->p_s.a2;
    if (((*value) <= 0) || ((value >= semDev) && (value < &semDev[MAX_DEVICES]))){
        insertBlocked(value, currentPCB);
        if ((value >= semDev) && (value <= &semDev[MAX_DEVICES])) softBlock += 1;
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
void debuggerio(){};

void iodevop(){
	debuggerio();
	unsigned int subdev_no, device = currentPCB->p_s.a3;
	unsigned int command = currentPCB->p_s.a2;

	devreg_t *genericDev = (devreg_t *) (device - 2*WS);
	subdev_no = instanceNo(LINE_NO(device - 2*WS));
	currentPCB->p_s.a2 = (unsigned int)&semDev[EXT_IL_INDEX(INT_TERMINAL)*DEV_PER_INT+ DEV_PER_INT + subdev_no];
	semp();
	//printint(LINE_NO((unsigned int)genericDev));
	if (LINE_NO((unsigned int)genericDev)+1 != INT_TERMINAL /*se non è un terminale*/){
		genericDev->dtp.command = command;
	} else {
		int a = instanceNo(LINE_NO((unsigned int)genericDev));
		unsigned int terminalReading = ((LINE_NO((unsigned int)genericDev)+1) == INT_TERMINAL && a >> 31) ? N_DEV_PER_IL : 0;
		if (terminalReading > 0 /*se il semaforo è in lettura*/){ genericDev->term.recv_command = command;}
		else /*scrittura*/{ genericDev->term.transm_command = command;}
	}
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
	//tprint("waitchild\n");
	extern unsigned int softBlock;
	extern pcb_t *currentPCB;
	if (currentPCB->p_first_child != NULL){	// if no child, don't wait
		//~ tprint("wait terminatechild\n");
		//~ currentPCB->p_s.pc -= WORD_SIZE;
		currentPCB = NULL;
	}
	/*
	if (currentPCB->p_first_child != NULL){	// if no child, don't wait
		softBlock += 1;
		insertBlocked((int *)currentPCB, currentPCB);
		currentPCB->p_s.pc -= WORD_SIZE;
	}*/
}
