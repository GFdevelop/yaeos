#include <interrupt.h>

//Hints from pages 130 and 63, uARMconst.h and libuarm.h
int INT_handler(){

	state_t *INT_Old = (state_t *) INT_OLDAREA;
	INT_Old->pc = INT_Old->pc - 4;

	unsigned int cause = getCAUSE();

	if(CAUSE_IP_GET(cause, INT_TIMER)){
		tprint("Timer interrupt");
		timer_HDL();
	}else if(CAUSE_IP_GET(cause, INT_LOWEST)){
		tprint("Lowest interrupt");
		device_HDL(INT_LOWEST);
	}else if(CAUSE_IP_GET(cause, INT_DISK)){
		tprint("Disk interrupt");
		device_HDL(INT_DISK);
	}else if(CAUSE_IP_GET(cause, INT_TAPE)){
		tprint("Tape interrupt");
		device_HDL(INT_TAPE);
	}else if(CAUSE_IP_GET(cause, INT_UNUSED)){
		tprint("Unused interrupt");
		device_HDL(INT_UNUSED);
	}else if(CAUSE_IP_GET(cause, INT_PRINTER)){
		tprint("Printer interrupt");
		device_HDL(INT_PRINTER);
	}else if(CAUSE_IP_GET(cause, INT_TERMINAL)){
		tprint("Terminal interrupt");
		terminal_HDL();
	}else{
		tprint("Interrupt not recognized!");
		PANIC();
	}

	return 0;
}

//TODO: The lower the line, the higher the priority
void timer_HDL(){
	//TODO: Distinguish between 3ms slice expiration and 100ms pseudo-clock
}

void device_HDL(unsigned int device){

}

void terminal_HDL(){
	//TODO: Handle terminal priority
}