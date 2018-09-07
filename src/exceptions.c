#include "exceptions.h"

#include "pcb.h"
#include "asl.h"

#include <uARMtypes.h>
#include <libuarm.h>

void TLB_handler(){
	return;
}

void PGMT_handler(){
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
			tprint("SEMP\n");
			semp();
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

	//scheduler();
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
	extern pcb_t *currentProcess;
	int * cast = (int *)currentProcess->p_s.a2;
	*cast+=1;
	removeBlocked((int *)currentProcess->p_s.a2);
}

void semp(){
	//tprint("semp\n");
	extern pcb_t *currentProcess;
	if((int *)currentProcess->p_s.a2 > 0){
		int * cast = (int *)currentProcess->p_s.a2;
		*cast-=1;
		insertBlocked((int *)currentProcess->p_s.a2, (pcb_t *)currentProcess->p_s.a2);
	}else {
		currentProcess->p_s.cpsr = STATUS_ALL_INT_ENABLE(currentProcess->p_s.cpsr);
		WAIT();
	}
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