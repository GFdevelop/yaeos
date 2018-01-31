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
		if (size < 19)	(pcbFree_table[size]).p_next = &pcbFree_table[size+1];
		else pcbFree_table[size].p_next = NULL;
	}
}

void freePcb(pcb_t *p){
	if (p != NULL){
		int size = pcbfree_h-pcbFree_table-1;
		if (size < 20){
			if (size < 19) p->p_next = pcbfree_h;
			//else pcbfree_h = NULL;
			pcbfree_h = p;
		}
	}
}

pcb_t *allocPcb(){
	int size = pcbfree_h-pcbFree_table;
	if (size > 20) return NULL;
	else {
		pcb_t * temp = pcbfree_h;
		if (pcbfree_h->p_next < &pcbFree_table[MAXPROC]) pcbfree_h = pcbfree_h->p_next;
		//else pcbfree_h = NULL;
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
		else {
			if (p->p_priority > (*head)->p_priority){
				p->p_next = *head;
				*head = p;
			}
			else if (p->p_priority < (*head)->p_priority){
				if ((*head)->p_next != NULL) p->p_next = (*head)->p_next;
				else p->p_next = NULL;
				(*head)->p_next = p;
			}
			else {
				if ((*head)->p_next == NULL){
					(*head)->p_next = p;
					p->p_next = NULL;
				}
				else insertProcQ(&(*head)->p_next, p);
			}
		}
	}
}

pcb_t *headProcQ(pcb_t *head){
	if (head == NULL) return NULL;
	else return head;
}

pcb_t* removeProcQ(pcb_t **head){
	if (*head == NULL) return NULL;
	else {
		pcb_t * tmp = *head;
		*head = (*head)->p_next;
		tmp->p_next = NULL;
		return tmp;
	}
}

pcb_t* outProcQ(pcb_t **head, pcb_t *p){
	if ((p == NULL) || (*head == NULL)) return NULL;
	else {
		if (*head == p){
			*head = (*head)->p_next;
			p->p_next = NULL;
			return p;
		}
		else if ((*head)->p_next == p){
			(*head)->p_next = p->p_next;
			p->p_next = NULL;
			return p;
		}
		else return outProcQ(&(*head)->p_next, p);
	}
}

void forallProcQ(pcb_t *head, void fun(pcb_t *pcb, void *), void *arg){
	if (head != NULL){
		fun(head,arg);
		forallProcQ(head->p_next, fun, arg);
	}
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
