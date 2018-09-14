#include "exceptions.h"

#include "pcb.h"
#include "asl.h"
#include "scheduler.h"
#include "interrupts.h"
#include "initial.h"

#include <arch.h>
#include <uARMtypes.h>
#include <libuarm.h>

extern pcb_t*currentProcess;

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

void TLB_handler(){
	tprint("TLB\n");
	return;
}

void PGMT_handler(){
	tprint("PGMT\n");
	return;
}

/*
int SYSCALL(CREATEPROCESS, state t *statep, int priority, void **cpid);
int SYSCALL(TERMINATEPROCESS, void * pid);
void SYSCALL(SEMP, int *semaddr);
void SYSCALL(SEMV, int *semaddr);
int SYSCALL(SPECHDL, int type, state_t *old, state_t *new);
void SYSCALL(GETTIME, cputime_t *user, cputime_t *kernel, cputime_t *wallclock);
void SYSCALL(WAITCLOCK);
unsigned int SYSCALL(IODEVOP, unsigned int command, unsigned int *comm_device_register);
void SYSCALL(GETPIDS, void **pid, void **ppid);
void SYSCALL(WAITCHLD);
*/

//TODO: Syscalls ok, but brakpoint?
void SYSBK_handler(){

	state_t *SYSBK_Old = (state_t *) SYSBK_OLDAREA;
	if(currentProcess){
		//SYSBK_Old->pc = SYSBK_Old->pc - 4;
		SVST(SYSBK_Old, &currentProcess->p_s);
	}
	
	switch(SYSBK_Old->a1){
		case CREATEPROCESS:
			//tprint("CREATEPROCESS\n");
			createprocess();
			break;
		case TERMINATEPROCESS:
			//tprint("TERMINATEPROCESS\n");
			terminateprocess();
			break;
		case SEMP:
			//tprint("SEMP\n");
			semp();
			break;
		case SEMV:
			//tprint("SEMV\n");
			semv();
			break;
		case SPECHDL:
			tprint("SPECHDL\n");
			spechdl();
			break;
		case GETTIME:
			tprint("GETTIME\n");
			gettime();
			break;
		case WAITCLOCK:
			tprint("WAITCLOCK\n");
			waitclock();
			break;
		case IODEVOP:
			//tprint("IODEVOP\n");
			iodevop();
			break;
		case GETPIDS:
			//ètprint("GETPIDS\n");
			getpids();
			break;
		case WAITCHLD:
			//tprint("WAITCHLD\n");
			waitchld();
			break;
		default:
			tprint("Breakpoint?");
			break;
	}

	scheduler();
	return;
}

int createprocess(){
	extern pcb_t *currentProcess, *readyQueue;
	extern unsigned int processCount;
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
	extern pcb_t *currentProcess, *readyQueue;
	pcb_t *head, *tmp;
	extern unsigned int processCount, softBlock;
	
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
    extern pcb_t *currentProcess, *readyQueue;
    extern unsigned int softBlock;
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
    extern pcb_t *currentProcess, *readyQueue;
    extern unsigned int softBlock;
    extern int sem_devices[MAX_DEVICES];

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
	
}

void waitclock(){
	extern int sem_devices[MAX_DEVICES];
	currentProcess->p_s.a2 = (unsigned int)&sem_devices[CLOCK_SEM];
	semp();
}

unsigned int iodevop(){
	extern pcb_t *currentProcess;
	extern int sem_devices[MAX_DEVICES];
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
	extern pcb_t *currentProcess;
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
	extern unsigned int softBlock;
	extern pcb_t *currentProcess;
	if (currentProcess->p_first_child != NULL){	// if no child, don't wait
		softBlock += 1;
		insertBlocked((int *)currentProcess, currentProcess);
		currentProcess = NULL;
	}
}