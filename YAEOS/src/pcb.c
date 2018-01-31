#include <uARMconst.h>
#include "const.h"
#include "pcb.h"
#include "asl.h"

pcb_t pcbFree_table[MAXPROC];
pcb_t *pcbfree_h = &pcbFree_table[MAXPROC];

void initPcbs(){
	int size = pcbfree_h-pcbFree_table-1;
	if (size >= 0) {
		pcbfree_h = pcbfree_h-1;
		initPcbs();
		if (size != 19)	(pcbFree_table[size]).p_next = &pcbFree_table[size+1];
		//*(pcbFree_table[size]).p_parent = NULL;
		//*(pcbFree_table[size]).p_first_child = NULL;
		//*(pcbFree_table[size]).p_sib = NULL;
	}
}

void freePcb(pcb_t *p){
	if (p != NULL){
		int size = pcbfree_h-pcbFree_table;
		if ((size < 20) && (size > 0)){
			p->p_next = pcbfree_h;
			pcbfree_h = p;
		}
	}
}

pcb_t *allocPcb(){
	if (pcbfree_h == &pcbFree_table[MAXPROC]) return NULL;
	else {
		pcb_t * temp = pcbfree_h;
		pcbfree_h = pcbfree_h->p_next;
		temp->p_next = NULL;
		temp->p_parent = NULL;
		temp->p_first_child = NULL;
		temp->p_sib = NULL;
		return temp;
	}
}

void insertProcQ(pcb_t **head, pcb_t *t){
	
}

pcb_t *headProcQ(pcb_t *head){
	pcb_t * temp;
	return temp;
}

pcb_t* removeProcQ(pcb_t **head){
	pcb_t * temp;
	return temp;
}

pcb_t* outProcQ(pcb_t **head, pcb_t *p){
	pcb_t * temp;
	return temp;
}

void forallProcQ(pcb_t *head, void fun(pcb_t *pcb, void *), void *arg){
	
}

void insertChild(pcb_t *parent, pcb_t *p){
	
}

pcb_t *removeChild(pcb_t *p){
	pcb_t * temp;
	return temp;
}

pcb_t *outChild(pcb_t *p){
	pcb_t * temp;
	return temp;
}
