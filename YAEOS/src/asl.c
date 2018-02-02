#include <uARMconst.h>
#include "const.h"
#include "pcb.h"
#include "asl.h"

semd_t semd_table[MAXSEMD];
semd_t *semdFree_h = &pcbFree_table[MAXSEMD];
semd_t *semdhash[ASHDSIZE];

int insertBlocked(int *key, pcb_t *p){
	int temp;
	return temp;
}

pcb_t *headBlocked(int *key){
	pcb_t * temp;
	return temp;
}

pcb_t* removeBlocked(int *key){
	pcb_t * temp;
	return temp;
}

void forallBlocked(int *key, void (*fun)(pcb_t *pcb, void *), void *arg){
	
}

//outChildBlocked(pcb_t *p);

void initASL(){
	int size = semdFree_h-semd_table-1;
	if (size >= 0) {
		semdFree_h = semdFree_h-1;
		initASL();
		if (size < 19)	(semd_table[size]).s_next = &semd_table[size+1];
		else semd_table[size].s_next = NULL;
	}
}
