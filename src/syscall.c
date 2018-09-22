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

int recursiveKill(pcb_t *pcb){
	extern pcb_t *currentPCB, *readyQueue;
	extern unsigned int processCount, softBlock;
	extern int semDev[MAX_DEVICES];
	int ret = 0;
	
    if(pcb!=NULL){
        if(pcb->p_semKey != NULL){
            if((pcb->p_semKey >= &semDev[0]) && (pcb->p_semKey <= &semDev[MAX_DEVICES])){
                softBlock--;
            } else {
                if((*pcb->p_semKey) < 0){
                    (*pcb->p_semKey)++;
                }
            }
            pcb = removeBlocked(pcb->p_semKey);
            if (pcb == NULL) ret = -1;
        }
        else if(currentPCB != pcb) {
			pcb = outProcQ(&readyQueue, pcb);
			if (pcb == NULL) ret = -1;
		}
        while(pcb->p_first_child){
            ret = recursiveKill(pcb->p_first_child);
        }
        pcb = outChild(pcb);
        if (pcb == NULL) ret = -1;
        if(currentPCB == pcb){
            currentPCB = NULL;
        }
		//~ outProcQ(&readyQueue, pcb);
        freePcb(pcb);
        processCount--;
    }
    return ret;
}

int terminateprocess(){
	//~ tprint("terminateprocess\n");
	extern pcb_t *currentPCB/*, *readyQueue*/;
	//~ extern unsigned int processCount/*, softBlock*/;
	
	if ((pcb_t *)currentPCB->p_s.a2 == NULL) return recursiveKill(currentPCB);
	else return recursiveKill((pcb_t *)currentPCB->p_s.a2);
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
	//~ tprint("waitclock\n");
	extern pcb_t *currentPCB;
	extern int semDev[MAX_DEVICES];
	extern unsigned int softBlock;
	
	//semp
	semDev[CLOCK_SEM] = semDev[CLOCK_SEM] - 1;
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
	semDev[EXT_IL_INDEX(INT_TERMINAL)*DEV_PER_INT+ DEV_PER_INT + subdev_no] = semDev[EXT_IL_INDEX(INT_TERMINAL)*DEV_PER_INT+ DEV_PER_INT + subdev_no] -1;
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
	//~ tprint("waitchild\n");
	extern unsigned int softBlock;
	extern pcb_t *currentPCB;
	extern int semWaitChild;
	if (currentPCB->p_first_child != NULL){	// if no child, don't wait
		semWaitChild--;
		insertBlocked(&semWaitChild, currentPCB);
		currentPCB = NULL;
	}
}
