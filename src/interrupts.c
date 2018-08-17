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
#include "interrupts.h"


void intHandler(){
	tprint("intHandler\n");
	switch (getCAUSE()){
		case(0): tprint("0"); break;
		case(1): tprint("1"); break;
		case(2): tprint("2"); break;
		case(3): tprint("3"); break;
		case(4): tprint("4"); break;
		case(5): tprint("5"); break;
		case(6): tprint("6"); break;
		case(7): tprint("7"); break;
		default: tprint("default");
	}
}
