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

#include "asl.h"

semd_t semd_table[MAXSEMD];
semd_t *semdFree_h = &semd_table[MAXSEMD];
semd_t *semdhash[ASHDSIZE];

void printint_(int a){
	int b = a%10;
    a = a/10;
    if (a>0) printint_(a);
    if (b==0) {tprint("0");} else if (b==1) {tprint("1");}
    else if (b==2) {tprint("2");} else if (b==3) {tprint("3");}
    else if (b==4) {tprint("4");} else if (b==5) {tprint("5");}
    else if (b==6) {tprint("6");} else if (b==7) {tprint("7");}
    else if (b==8) {tprint("8");} else if (b==9) {tprint("9");}
}
void printint(int a){
	if (a<0) {tprint("-"); a = -a;}
    printint_(a);
	tprint("\n");
}

int insertBlocked(int *key, pcb_t *p){
	//if (key==-1) {HALT();}
	/*tprint("\n");
	printint((int)key);
	printint(*key);
	printint((int)key/2);
	printint(((int)key/2)%ASHDSIZE);
	tprint("\n");*/
	if ((key == NULL)||(p == (pcb_t *)NULL)) return -1;
	else {
/*
Complete information at point [5] in design_choices.txt
*/
		int hash = ((int)key/2)%ASHDSIZE;	// get hash index (PROJECT CHOICES)
		if (semdhash[hash] == NULL){	// if index is empty or s_next (recursion) is NULL then alloc semaphore
			if (semdFree_h == NULL) return -1;
			else {
				semdhash[hash] = semdFree_h;	// put head of free list in hash index
				semdFree_h = semdFree_h->s_next;	// head of free list is next of free list
				semdhash[hash]->s_next = NULL;
				semdhash[hash]->s_key = key;
				p->p_semKey = key;
				insertProcQ(&semdhash[hash]->s_procQ,p);
				return 0;
			}
		} else if (semdhash[hash]->s_key == key){		// if key==s_key then insert pcb to same semaphore
			p->p_semKey = key;
			insertProcQ(&semdhash[hash]->s_procQ,p);
			return 0;
		} else {		// if key!=s_key then recursive check s_next
			semd_t * saved = semdhash[hash];
			semdhash[hash] = semdhash[hash]->s_next;	// go to next semaphore (node) of this hash index
			int ret = insertBlocked(key,p);
			if (saved->s_next == NULL) saved->s_next = semdhash[hash]; // link the node inserted (in recursion)
			semdhash[hash] = saved;		//restore hash node
			return ret;
		}
	}
}

pcb_t *headBlocked(int *key){
	int hash = ((int)key/2)%ASHDSIZE;
	if (semdhash[hash] == NULL) return NULL;
	else return headProcQ(semdhash[hash]->s_procQ);
}

pcb_t* removeBlocked(int *key){
	int hash = ((int)key/2)%ASHDSIZE;
	pcb_t * ret = (pcb_t *)NULL;
	semd_t * saved = (semd_t *)NULL;
	if (semdhash[hash] == NULL) ret = NULL;
	else if (semdhash[hash]->s_key == key) {	// if node has that key
		ret = removeProcQ(&semdhash[hash]->s_procQ);
		ret->p_semKey = NULL;
		if (semdhash[hash]->s_procQ == NULL){	// free the semaphore
			semdhash[hash]->s_key = NULL;
			saved = semdhash[hash]->s_next;	// save next node
			semdhash[hash]->s_next = semdFree_h;		//link to head of free list
			semdFree_h = semdhash[hash];				// free node is new head
			semdhash[hash] = saved;
		}
	} else {		// if node has't that key
		saved = semdhash[hash];
		semdhash[hash] = semdhash[hash]->s_next;	//move to next node
		ret = removeBlocked(key);
		// if next node is changed (in recursion) then link new node
		if (saved->s_next != semdhash[hash]) saved->s_next = semdhash[hash];
		semdhash[hash] = saved;	//restore hash node
	}
	return ret;
}

void forallBlocked(int *key, void (*fun)(pcb_t *pcb, void *), void *arg){
	int hash = ((int)key/2)%ASHDSIZE;
	if (semdhash[hash] != NULL) {
		if (semdhash[hash]->s_key == key) forallProcQ(semdhash[hash]->s_procQ, fun, arg);
		else {
			semd_t * saved = semdhash[hash];
			semdhash[hash] = semdhash[hash]->s_next;	// go to next node of this hash and...
			forallBlocked(key, fun, arg);				// ...check if it has this key
			semdhash[hash] = saved;
		}
	}
}

void outChildBlocked(pcb_t *p){
	if (p != NULL){
		int hash = ((int)p->p_semKey/2)%ASHDSIZE;
		if (semdhash[hash] != NULL) {
			if (semdhash[hash]->s_key == p->p_semKey) removeBlocked(p->p_semKey);
			else {
				semd_t * prev = semdhash[hash];
				semdhash[hash] = semdhash[hash]->s_next;	// go to next node of this hash and...
				outChildBlocked(p);							// ...check for pcb key
				semdhash[hash] = prev;
			}
		}
	}
}

void initASL(){
	semdFree_h = semdFree_h-1;	//index of array
	// if pcbfree_h point to last then follower is NULL otherwise is next
	semdFree_h->s_next = (semdFree_h < &semd_table[MAXSEMD-1]) ? semdFree_h+1 : NULL;
	if (semdFree_h > semd_table) initASL();
}
