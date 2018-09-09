#include "exceptions.h"

#include "pcb.h"
#include "asl.h"
#include "scheduler.h"
#include "interrupts.h"

#include <uARMtypes.h>
#include <libuarm.h>

extern pcb_t*currentProcess;

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

	unsigned int a1, a2, a3, a4;

	state_t *SYSBK_Old = (state_t *) SYSBK_OLDAREA;
	if(currentProcess){
		a1 = SYSBK_Old->a1;
		a2 = SYSBK_Old->a2;
		a3 = SYSBK_Old->a3;
		a4 = SYSBK_Old->a4;
		SYSBK_Old->pc -= 4;
		SVST(SYSBK_Old, &currentProcess->p_s);
	}
	
	switch(a1){
		case CREATEPROCESS:
			tprint("CREATEPROCESS\n");
			createprocess();
			break;
		case TERMINATEPROCESS:
			tprint("TERMINATEPROCESS\n");
			terminateprocess();
			break;
		case SEMP:
			tprint("SEMP\n");
			semp(&a2);
			break;
		case SEMV:
			tprint("SEMV\n");
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
			tprint("IODEVOP\n");
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
	tprint("semv\n");
	extern pcb_t *currentProcess, *readyQueue;
	insertProcQ(&readyQueue,removeBlocked((int *)currentProcess->p_s.a2));
	unsigned int value = currentProcess->p_s.a2;
	value++;
}
 
void semp(unsigned int *semaddr){
	tprint("semp\n");
	extern pcb_t *currentProcess, *readyQueue;
	extern unsigned int softBlock;
	pcb_t *tmp = currentProcess;

	if (*semaddr == 0) {
		//tmp->p_s.cpsr = STATUS_ENABLE_TIMER(tmp->p_s.cpsr);
		tprint("Here");
		WAIT();
	}
	*semaddr -= 1;
	outProcQ(&readyQueue,tmp);
	//currentPCB = NULL;
	softBlock += 1;
	insertBlocked((int *)tmp->p_s.a2, (pcb_t *)tmp->p_s.a2);
	tprint("Allora");
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
	currentProcess->p_s.a3 = currentProcess->p_s.a2;
	SYSCALL(SEMP, (unsigned int)currentProcess, 0, 0);
	return 0;
}

void getpids(){
	extern pcb_t *currentProcess;
	if (currentProcess->p_s.a2 != (unsigned int)NULL) return (void)currentProcess->p_s.a2;
	else if (currentProcess->p_s.a3 != (unsigned int)NULL) return (void)currentProcess->p_s.a3;
}

//Added missing SYSCALL10
void waitchld(){

}