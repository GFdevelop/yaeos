#include <uARMconst.h>
#include "const.h"
#include "pcb.h"
#include "asl.h"

pcb_t pcbFree_table[MAXPROC];
pcb_t *pcbfree_h;
/*
void initPcbs(){
    pcb_t *temp;
    int i; 
    pcbfree_h -> p_next = pcbFree_table[0];
    temp = pcbFree_table[0];
    for(i = 1; i < MAXPROC; i++){
        temp -> p_next = pcbFree_table[i];
        temp = temp -> p_next;
    }  
}*/

void initPcbs(){
	pcbfree_h = &pcbFree_table[0];
	int size;
	if ((size = sizeof(pcbFree_table)/sizeof(pcbFree_table[0])) < 20) {
		initPcbs();
		(pcbFree_table[size]).p_next = &pcbFree_table[size+1];
		//*(pcbFree_table[size]).p_parent = NULL;
		//*(pcbFree_table[size]).p_first_child = NULL;
		//*(pcbFree_table[size]).p_sib = NULL;
	}
}
/*
void freePcb(pcb_t *p){
	int size = sizeof(pcbFree_table)/sizeof(pcbfree_h);
	if (size < 20) {
		if (*(pcbFree_table[size]).p_first_child != NULL) freePcb(p);
		else *(pcbFree_table[size]).p_first_child = p;
	}
}*/
/*
pcb_t *allocPcb(){
	
}
*/
void freePcb(pcb_t *p){
	
}

pcb_t *allocPcb(){
	int size = sizeof(pcbFree_table)/sizeof(pcbfree_h);
	if (size == 0) return NULL;
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
