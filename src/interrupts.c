#include "interrupts.h"

#include "scheduler.h"

#include <libuarm.h>

extern state_t *INT_Old;
//Hints from pages 130 and 63, uARMconst.h and libuarm.h
void INT_handler(){

	INT_Old = (state_t *) INT_OLDAREA;
	INT_Old->pc -= 4;

	unsigned int cause = getCAUSE();

	if(CAUSE_IP_GET(cause, INT_TIMER)){
		tprint("Timer interrupt\n");
		timer_HDL();
	}else if(CAUSE_IP_GET(cause, INT_LOWEST)){
		tprint("Lowest interrupt\n");
		device_HDL(INT_LOWEST);
	}else if(CAUSE_IP_GET(cause, INT_DISK)){
		tprint("Disk interrupt\n");
		device_HDL(INT_DISK);
	}else if(CAUSE_IP_GET(cause, INT_TAPE)){
		tprint("Tape interrupt\n");
		device_HDL(INT_TAPE);
	}else if(CAUSE_IP_GET(cause, INT_UNUSED)){
		tprint("Unused interrupt\n");
		device_HDL(INT_UNUSED);
	}else if(CAUSE_IP_GET(cause, INT_PRINTER)){
		tprint("Printer interrupt\n");
		device_HDL(INT_PRINTER);
	}else if(CAUSE_IP_GET(cause, INT_TERMINAL)){
		tprint("Terminal interrupt\n");
		terminal_HDL();
	}else{
		tprint("Interrupt not recognized!\n");
		PANIC();
	}

	return;
}

//TODO: The lower the line, the higher the priority
void timer_HDL(){
	scheduler();
	//TODO: Distinguish between 3ms slice expiration and 100ms pseudo-clock
}

void device_HDL(unsigned int device){

}

void terminal_HDL(){
	//TODO: Handle terminal priority
}