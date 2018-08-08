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


#include "libuarm.h"
#include "pcb.h"
#include "asl.h"
#include "syscall.h"
#include "exceptions.h"


void tlbHandler(){
	tprint("tlbHandler\n");
}

void pgmtrapHandler(){
	tprint("pgmtrapHandler\n");
}

void sysbkHandler(){
	tprint("sysbkHandler\n");
	extern pcb_t *currentPCB;
	switch(currentPCB->p_s.a1){
		case(CREATEPROCESS):
			createprocess();
			break;
		case(TERMINATEPROCESS):
			terminateprocess();
			break;
		case(SEMV):
			semv();
			break;
		case(SEMP):
			semp();
			break;
		case(SPECHDL):
			spechdl();
			break;
		case(GETTIME):
			gettime();
			break;
		case(WAITCLOCK):
			waitclock();
			break;
		case(IODEVOP):
			iodevop();
			break;
		case(GETPIDS):
			getpids();
			break;
	}
	tprint("end\n");
}
