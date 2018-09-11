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
			tprint("CREATEPROCESS\n");
			createprocess();
			break;
		case TERMINATEPROCESS:
			tprint("TERMINATEPROCESS\n");
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
			tprint("GETPIDS\n");
			getpids();
			break;
		case WAITCHLD:
			tprint("WAITCHLD\n");
			waitchld();
			break;
		default:
			tprint("Breakpoint?");
			break;
	}

	//tprint("Allora\n");
	scheduler();
	return;
}

// --- GFdelevop's code ---

int createprocess(){
	extern pcb_t *currentProcess;
	pcb_t *childPCB = allocPcb();
	if (childPCB == NULL) return -1;
	else {
		insertChild(currentProcess, childPCB);
		STST(&childPCB->p_s);
		currentProcess->p_s.a4 = (unsigned int)childPCB;
		return 0;
	}
}

int terminateprocess(){
	extern pcb_t *currentProcess;
	pcb_t *head;
	if (currentProcess->p_s.a2 == (int)NULL) head = currentProcess;
	else head = (pcb_t *)currentProcess->p_s.a2;
	forallProcQ(head, (void *)removeProcQ, head->p_first_child);
	removeProcQ(&head);
	return 0;
}

void semv(){
    extern pcb_t *currentProcess, *readyQueue;
    extern unsigned int softBlock;
    pcb_t *tmp;
    int *value = (int *)currentProcess->p_s.a2;
    if (headBlocked(value)) {
    	//tprint("Sblocca\n");
        softBlock--;
        tmp = removeBlocked(value);
        insertProcQ(&readyQueue, tmp);
    }
    *value += 1;
}
 
void semp(){
    extern pcb_t *currentProcess, *readyQueue;
    extern unsigned int softBlock;
    extern int sem_devices[MAX_DEVICES];

    int *value = (int *)currentProcess->p_s.a2;
    if (((*value) <= 0) || ((value >= sem_devices) && (value <= &sem_devices[MAX_DEVICES - 1]))){
        //tprint("Entra\n");
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
	extern pcb_t *currentProcess;
	SYSCALL(SEMP, (unsigned int)currentProcess, 0, 0);
}

unsigned int iodevop(){
	extern pcb_t *currentProcess;
	extern int sem_devices[MAX_DEVICES];
	unsigned int subdev_no, device = currentProcess->p_s.a3;
	unsigned int command = currentProcess->p_s.a2;
	
	termreg_t *term = (termreg_t *) (device - 2*WS);
	subdev_no = instanceNo(LINE_NO(device - 2*WS));
	currentProcess->p_s.a2 = (unsigned int)&sem_devices[EXT_IL_INDEX(INT_TERMINAL)*DEV_PER_INT+ DEV_PER_INT + subdev_no];
	
	semp();

	term->transm_command = command;
	/*
	if(sem_devices[EXT_IL_INDEX(INT_TERMINAL)*DEV_PER_INT+ DEV_PER_INT + subdev_no]) semp();
	else{
		term->transm_command = command;
		LDST(&currentProcess->p_s);
	}
	*/
	return term->transm_status;
}

void getpids(){
	extern pcb_t *currentProcess;
	if (currentProcess->p_s.a2 != (unsigned int)NULL) return (void)currentProcess->p_s.a2;
	else if (currentProcess->p_s.a3 != (unsigned int)NULL) return (void)currentProcess->p_s.a3;
}

//Added missing SYSCALL10
void waitchld(){

}