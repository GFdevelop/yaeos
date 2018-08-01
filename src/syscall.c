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


#include "pcb.h"


int createprocess(){
	extern pcb_t *currentPCB;
	pcb_t *childPCB = allocPcb();
	if (childPCB == NULL) return -1;
	else {
		insertChild(currentPCB, childPCB);
		//memcpy(childPCB->p_s, currentPCB->p_s, 88);
		childPCB->p_s =  currentPCB->p_s;
		&cpid = childPCB;
		return 0;
	}
}
