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

pcb_t pcbFree_table[MAXPROC];
pcb_t *pcbfree_h = &pcbFree_table[MAXPROC];

void initPcbs(){
	pcbfree_h = pcbfree_h-1;	//index of array
	pcbfree_h->p_next = (pcbfree_h < &pcbFree_table[MAXPROC-1]) ? pcbfree_h+1 : NULL;
	if (pcbfree_h > pcbFree_table) initPcbs();
}

void freePcb(pcb_t *p){
	if (p != NULL){
		//if (pcbfree_h-pcbFree_table-1 < MAXPROC){		//questo if è inutile, sostituirlo con uno valido
			p->p_next = pcbfree_h;
			pcbfree_h = p;
		//}
	}
}

pcb_t *allocPcb(){
	if (pcbfree_h == NULL) return NULL;
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

void insertProcQ(pcb_t **head, pcb_t *p){
	if (p != NULL){
		if (*head == NULL){
			*head = p;
			(*head)->p_next = NULL;
		}
		else if (p->p_priority > (*head)->p_priority){
			p->p_next = *head;
			*head = p;
		}
		else {
			insertProcQ(&(*head)->p_next, p);
			if ((*head)->p_next == p->p_next) (*head)->p_next = p;
		}
	}
}

pcb_t *headProcQ(pcb_t *head){
	return head;
}

pcb_t* removeProcQ(pcb_t **head){
	if (*head == NULL) return NULL;
	else {
		pcb_t * tmp = *head;
		*head = (*head)->p_next;
		return tmp;
	}
}

pcb_t* outProcQ(pcb_t **head, pcb_t *p){	//vai a scelte progettuali 2
	if ((p == NULL) || (*head == NULL)) return NULL;
	else if (*head == p) return removeProcQ(&(*head));
	else return outProcQ(&(*head)->p_next, p);
}

void forallProcQ(pcb_t *head, void fun(pcb_t *pcb, void *), void *arg){
	if (head != NULL){
		fun(head,arg);
		forallProcQ(head->p_next, fun, arg);
	}
}

void insertChild(pcb_t *parent, pcb_t *p){
	if ((parent != NULL) && (p != NULL)){
		if (parent->p_first_child == NULL) {
			p->p_sib = NULL;
			parent->p_first_child = p;
			p->p_parent = parent;
		}
		else {
			pcb_t * son = parent->p_first_child;
			parent->p_first_child = son->p_sib;
			insertChild(parent,p);
			parent->p_first_child = son;
			if (son->p_sib == NULL) son->p_sib = p;
		}
	}
}

pcb_t *removeChild(pcb_t *p){
	if ((p == NULL) || (p->p_first_child == NULL)) return NULL;
	else  {
		pcb_t * ret = p->p_first_child;
		p->p_first_child = ret->p_sib;
		return ret;
	}
}

pcb_t *outChild(pcb_t *p){
	if ((p == NULL) || (p->p_parent == NULL) || (p->p_parent->p_first_child == NULL)) return NULL;
	else if (p == p->p_parent->p_first_child) return removeChild(p->p_parent);
	else {
		pcb_t * son = p->p_parent->p_first_child;
		p->p_parent->p_first_child = son->p_sib;
		pcb_t * ret = outChild(p);
		if (p->p_parent->p_first_child == p->p_sib) son->p_sib = p->p_parent->p_first_child;
		p->p_parent->p_first_child = son;
		return ret;
	}
}
