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
   
#include "pcb.h"

pcb_t pcbFree_table[MAXPROC];
/*
Assign to the pointer pcbfree_h the memory address of the next memory location after the last element of pcbFree_table.
See point [1] of design_choises.txt.
*/
pcb_t *pcbfree_h = &pcbFree_table[MAXPROC];

void initPcbs(){
	pcbfree_h = pcbfree_h-1;	//index of array
	// if pcbfree_h is last of list then the follower is NULL otherwise is next
	pcbfree_h->p_next = (pcbfree_h < &pcbFree_table[MAXPROC-1]) ? pcbfree_h+1 : NULL;
	if (pcbfree_h > pcbFree_table) initPcbs();
}

void freePcb(pcb_t *p){
	if (p != NULL){
		p->p_next = pcbfree_h;
		pcbfree_h = p;
	}
}

pcb_t *allocPcb(){
	if (pcbfree_h == NULL) return NULL;
	else {
		pcb_t * ret = pcbfree_h;
		pcbfree_h = pcbfree_h->p_next;	//move head of free list
		ret->p_next = NULL;
		ret->p_parent = NULL;
		ret->p_first_child = NULL;
		ret->p_sib = NULL;
		ret->activation_time = 0;
		ret->kernel_time = 0;
		ret->user_time = 0;
		return ret;
	}
}

void insertProcQ(pcb_t **head, pcb_t *p){
	if (p != NULL){
		if (*head == NULL){		// if list is empty or is end of nodes (recursion) then insert
			*head = p;
			(*head)->p_next = NULL;
		} else if (p->p_priority > (*head)->p_priority){	// if p has major priority of this node then insert
			p->p_next = *head;
			*head = p;
		} else if ((*head)->p_next == NULL) {
			p->p_next = NULL;
			(*head)->p_next = p;
		} else {	// if p has priority <= than this node, try to insert before the next node
			insertProcQ(&(*head)->p_next, p);
			if ((*head)->p_next == p->p_next) (*head)->p_next = p;	// if node was inserted then link new node
		}
	}
}

pcb_t *headProcQ(pcb_t *head){ //NULL (empty list) case not needed: if list is empty head == NULL
	return head;
}

pcb_t* removeProcQ(pcb_t **head){
	if (*head == NULL) return NULL;
	else {
		pcb_t * ret = *head;
		*head = (*head)->p_next;
		ret->p_next = NULL;
		return ret;
	}
}

/*
For more information, see point [2] in design_choices.txt
*/
pcb_t* outProcQ(pcb_t **head, pcb_t *p){
	if ((p == NULL) || (*head == NULL)) return NULL;	// if end of list (p not found)
	else if (*head == p) return removeProcQ(head);		// if p is found remove pcb
	else return outProcQ(&(*head)->p_next, p);			// if p is not found check next node
}

void forallProcQ(pcb_t *head, void fun(pcb_t *pcb, void *), void *arg){
	if (head != NULL){
		forallProcQ(head->p_next, fun, arg);
		fun(head,arg);
	}
}

/*
See desing_choices.txt point [3]
*/
void insertChild(pcb_t *parent, pcb_t *p){
	if ((parent != NULL) && (p != NULL)){
		if (parent->p_first_child == NULL){		//if list is empty or is end of list
			p->p_sib = NULL;
			parent->p_first_child = p;
			p->p_parent = parent;
		} else {
			pcb_t *saved = parent->p_first_child;
			parent->p_first_child = saved->p_sib;	// move to next sibling
			insertChild(parent,p);
			parent->p_first_child = saved;	//restore old child
			if (saved->p_sib == NULL) saved->p_sib = p;		// if end of list then we link new node
		}
	}
}

pcb_t *removeChild(pcb_t *p){
	if ((p == NULL) || (p->p_first_child == NULL)) return NULL;
	else {
		pcb_t * ret = p->p_first_child;
		p->p_first_child = ret->p_sib;
		ret->p_sib = NULL;
		return ret;
	}
}

/*
Complete information at point [4] in design_choices.txt
*/
pcb_t *outChild(pcb_t *p){
	if ((p == NULL) || (p->p_parent == NULL) || (p->p_parent->p_first_child == NULL)) return NULL;	//not in list
	else if (p == p->p_parent->p_first_child) return removeChild(p->p_parent);	// (move first child to p->p_sib)
	else {
		pcb_t * saved = p->p_parent->p_first_child;
		p->p_parent->p_first_child = saved->p_sib;		// first child "is" next sibling
		pcb_t * ret = outChild(p);						// check next sibling
		// if node is removed then link follower node
		if (saved->p_sib != p->p_parent->p_first_child) saved->p_sib = p->p_parent->p_first_child;
		p->p_parent->p_first_child = saved;		// restore
		return ret;
	}
}
