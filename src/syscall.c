#include <syscall.h>

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

int SYSBK_handler(){

	state_t *SYSBK_Old = (state_t *) SYSBK_OLDAREA;
	
	switch(SYSBK_Old->a1){
		case CREATEPROCESS:
			tprint("CREATEPROCESS");
			createprocess();
			break;
		case TERMINATEPROCESS:
			tprint("TERMINATEPROCESS");
			terminateprocess();
			break;
		case SEMP:
			tprint("SEMP");
			semp();
			break;
		case SEMV:
			tprint("SEMV");
			semv();
			break;
		case SPECHDL:
			tprint("SPECHDL");
			spechdl();
			break;
		case GETTIME:
			tprint("GETTIME");
			gettime();
			break;
		case WAITCLOCK:
			tprint("WAITCLOCK");
			waitclock();
			break;
		case IODEVOP:
			tprint("IODEVOP");
			iodevop();
			break;
		case GETPIDS:
			tprint("GETPIDS");
			getpids();
			break;
		case WAITCHLD:
			tprint("WAITCHLD");
			waitchld();
			break;
		default:
			tprint("Breakpoint?");
			break;
	}

	return 0;
}

// --- GFdelevop's code ---

int createprocess(){
	extern pcb_t *currentPCB;
	pcb_t *childPCB = allocPcb();
	if (childPCB == NULL) return -1;
	else {
		insertChild(currentPCB, childPCB);
		//memcpy(childPCB->p_s, currentPCB->p_s, 88);
		//childPCB->p_s =  currentPCB->p_s;
		STST(&childPCB->p_s);
		currentPCB->p_s.a4 = (unsigned int)childPCB;
		return 0;
	}
}

int terminateprocess(){
	extern pcb_t *currentPCB;
	pcb_t *head;
	if (currentPCB->p_s.a2 == (int)NULL) head = currentPCB;
	else head = (pcb_t *)currentPCB->p_s.a2;
	forallProcQ(head, (void *)removeProcQ, head->p_first_child);
	removeProcQ(&head);
	return 0;
}

void semv(){
	extern pcb_t *currentPCB;
	removeBlocked((int *)currentPCB->p_s.a2);
}

void semp(){
	tprint("semp\n");
	extern pcb_t *currentPCB;
	insertBlocked((int *)currentPCB->p_s.a2, (pcb_t *)currentPCB->p_s.a2);
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
	
}

void waitclock(){
	extern pcb_t *currentPCB;
	SYSCALL(SEMP, (unsigned int)currentPCB, 0, 0);
}

unsigned int iodevop(){
	extern pcb_t *currentPCB;
	currentPCB->p_s.a3 = currentPCB->p_s.a2;
	SYSCALL(SEMP, (unsigned int)currentPCB, 0, 0);
	return 0;
}

void getpids(){
	extern pcb_t *currentPCB;
	if (currentPCB->p_s.a2 != (unsigned int)NULL) return (void)currentPCB->p_s.a2;
	else if (currentPCB->p_s.a3 != (unsigned int)NULL) return (void)currentPCB->p_s.a3;
}

//Added missing SYSCALL10
void waitchld(){

}