#include <uARMconst.h>
#include "const.h"
#include "pcb.h"
#include "asl.h"

semd_t semd_table[MAXSEMD];
semd_t *semdFree_h = &semd_table[MAXSEMD];
semd_t *semdhash[ASHDSIZE];

int insertBlocked(int *key, pcb_t *p){
	long k = (*key)*61803398875;
	int hash = (8*(k-(int)k))-(10^10);
	if (semdhash[hash] == NULL) {
		if (semdFree_h == &semd_table[MAXSEMD]) return -1;
		else {
			semdhash[hash] = semdFree_h;
			semdFree_h->s_next = NULL;
			semdFree_h = semdFree_h+1;
			return 0;
		}
	}
	else {
		if (semdFree_h == &semd_table[MAXSEMD]) return -1;
		else {
			semd_t * tmp = semdhash[hash];
			semdhash[hash] = semdFree_h;
			semdFree_h->s_next = tmp;
			semdFree_h = semdFree_h+1;
			return 0;
		}
	}
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