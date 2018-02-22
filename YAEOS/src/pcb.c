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
/*
Assign to the pointer pcbfree_h the memory address of the next memory location after the last element of pcbFree_table.
See point [1] of design_choises.txt.
*/
pcb_t *pcbfree_h = &pcbFree_table[MAXPROC];

void initPcbs(){
	pcbfree_h = pcbfree_h-1;	//index of array
	pcbfree_h->p_next = (pcbfree_h < &pcbFree_table[MAXPROC-1]) ? pcbfree_h+1 : NULL;
	if (pcbfree_h > pcbFree_table) initPcbs();
}

void freePcb(pcb_t *p){
	if (p != NULL){
		//if (pcbfree_h-pcbFree_table-1 < MAXPROC){		//FIXME: questo if è inutile, sostituirlo con uno valido
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
	if (p != NULL){	//The insertion can be divided into three cases...
		if (*head == NULL){	//...list pointed by head is empty 
			*head = p;
			(*head)->p_next = NULL;
		}else if (p->p_priority > (*head)->p_priority){	//...priority of element p > priority of the current one
			p->p_next = *head;
			*head = p;
		}else{	//...priority of the current element > priority of the element p
			insertProcQ(&(*head)->p_next, p);
			if ((*head)->p_next == p->p_next) (*head)->p_next = p;
		}
	}
}

pcb_t *headProcQ(pcb_t *head){ //NULL (empty list) case not needed: if list is empty head == NULL
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

/*
For more information, see point [2] in design_choices.txt
*/
pcb_t* outProcQ(pcb_t **head, pcb_t *p){	//Four possible scenarios...
	if ((p == NULL) || (*head == NULL)) return NULL;	//...p is NULL or list is empty/p is not found
	else if (*head == p) return removeProcQ(head); //...p is the element pointed by head
	else return outProcQ(&(*head)->p_next, p);	//...p not found but list isn't finished yet
}

void forallProcQ(pcb_t *head, void fun(pcb_t *pcb, void *), void *arg){
	if (head != NULL){
		fun(head,arg);
		forallProcQ(head->p_next, fun, arg);
	}
}

/*
See desing_choices.txt point [3]
*/
void insertChild(pcb_t *parent, pcb_t *p){
	if ((parent != NULL) && (p != NULL)){
		if (parent->p_first_child == NULL){	//parent has no child
			p->p_sib = NULL;
			parent->p_first_child = p;
			p->p_parent = parent;
		}else{	//parent has already one or more children
			pcb_t *son = parent->p_first_child;
			parent->p_first_child = son->p_sib;
			insertChild(parent,p);
			parent->p_first_child = son;
			if (son->p_sib == NULL) son->p_sib = p;
		}
	}
}

pcb_t *removeChild(pcb_t *p){
	if ((p == NULL) || (p->p_first_child == NULL)) return NULL;
	else{
		pcb_t * ret = p->p_first_child;
		p->p_first_child = ret->p_sib;
		return ret;
	}
}

/*
Complete information at point [4] in design_choices.txt
*/
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
