/* * * * * * * * * * * * * * * * * * * * * * * * *
 * YAEOS' phase 1 implementation proposed by     *
 * - Francesco Fornari 							 *
 * - Gabriele Fulgaro							 *
 * - Mattia Polverini							 *
 * 												 *
 * Operating System course						 *
 * A.A. 2017/2018 								 *
 * Alma Mater Studiorum - University of Bologna  *
 * * * * * * * * * * * * * * * * * * * * * * * * */
 
#include <uARMconst.h>
#include "const.h"
#include "pcb.h"
#include "asl.h"

semd_t semd_table[MAXSEMD];
semd_t *semdFree_h = &semd_table[MAXSEMD];
semd_t *semdhash[ASHDSIZE];

int insertBlocked(int *key, pcb_t *p){
	int ret = 0;
	if (p == NULL) ret = -1;
	else {
		int hash = (*key/4)%8;
		if (semdhash[hash] == NULL){
			 //il puntatore salta fuori dall'intervallo, vedere la remove
			if ((semdFree_h >= &semd_table[MAXSEMD]) || (semdFree_h < semd_table)) ret = -1;
			else {
				semdhash[hash] = semdFree_h;
				semdFree_h = semdFree_h->s_next;
				semdhash[hash]->s_next = NULL;
				semdhash[hash]->s_key = key;
				p->p_semKey = key;
				insertProcQ(&(*semdhash)[hash].s_procQ,p);
			}
		}
		else if (semdhash[hash]->s_key != key){
			semd_t * prev = semdhash[hash];
			semdhash[hash] = semdhash[hash]->s_next;
			ret = insertBlocked(key,p);
			if ((semdhash[hash]->s_key == key) && (prev->s_next == NULL)) prev->s_next = semdhash[hash];
			semdhash[hash] = prev;
		}
		else {
			p->p_semKey = key;
			insertProcQ(&(*semdhash)[hash].s_procQ,p);
		}
	}
	return ret;
}

pcb_t *headBlocked(int *key){
	int hash = (*key/4)%8;
	if (semdhash[hash] == NULL) return NULL;
	else return headProcQ((*semdhash)[hash].s_procQ);
}

pcb_t* removeBlocked(int *key){
	int hash = (*key/4)%8;
	pcb_t * ret = NULL;
	if (semdhash[hash] == NULL) ret = NULL;
	else if (semdhash[hash]->s_key == key) {
		ret = removeProcQ(&(*semdhash)[hash].s_procQ);
		if (semdhash[hash]->s_procQ == NULL){
			semdhash[hash]->s_key = NULL;
			semd_t * next = semdhash[hash]->s_next;
			semdhash[hash]->s_next = semdFree_h;
			//questo credo che fa saltare il puntatore fuori dalla lista libera
			semdFree_h = semdhash[hash];
			semdhash[hash] = next;
		}
	}
	else {
		semd_t * prev = semdhash[hash];
		semdhash[hash] = semdhash[hash]->s_next;
		ret = removeBlocked(key);
		if (semdhash[hash] != prev->s_next) prev->s_next = semdhash[hash];
		semdhash[hash] = prev;
	}
	return ret;
}

void forallBlocked(int *key, void (*fun)(pcb_t *pcb, void *), void *arg){
	int hash = (*key/4)%8;
	if (semdhash[hash] != NULL) {
		if (semdhash[hash]->s_key == key) forallProcQ((*semdhash)[hash].s_procQ, fun, arg);
		else {
			semd_t * prev = semdhash[hash];
			semdhash[hash] = semdhash[hash]->s_next;
			forallBlocked(key, fun, arg);
			semdhash[hash] = prev;
		}
	}
}

void outChildBlocked(pcb_t *p){
	if (p != NULL){
		int hash = (*p->p_semKey/4)%8;
		if (semdhash[hash] != NULL) {
			if (semdhash[hash]->s_key == p->p_semKey) removeBlocked(p->p_semKey);
			else {
				semd_t * prev = semdhash[hash];
				semdhash[hash] = semdhash[hash]->s_next;
				outChildBlocked(p);
				semdhash[hash] = prev;
			}
		}
	}
}

void initASL(){
	int size = semdFree_h-semd_table;
	if (size > 0) {
		semdFree_h = semdFree_h-1;
		initASL();
		if (size < 20)	(semd_table[size-1]).s_next = &semd_table[size];
		else semd_table[size-1].s_next = NULL;
	}
}
