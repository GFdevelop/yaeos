#ifndef PCB_H
#define PCB_H

#include <uARMtypes.h>
#include <uARMconst.h>
#include <const.h>

typedef struct pcb_t {
	struct pcb_t *p_next;
	struct pcb_t *p_parent;
	struct pcb_t *p_first_child;
	struct pcb_t *p_sib;
	state_t p_s;
	int p_priority;
	int *p_semKey;	

	unsigned int activation_time;
	unsigned int kernel_time;
	unsigned int user_time;
}pcb_t;

/**** PCB queue management ****/

/* 
Initializes the pcbFree list in order to contain all the elements of the pcbFree_table. 
This method will be invoked only once in the initialization phase of the data structure.
*/
void initPcbs();

/* 
Insert the PCB pointed by p in the pcbFree list. 
*/
void freePcb(pcb_t *p);

/*
Remove an element from pcbFree list, initialize and return it.
Return NULL if the pcbFree list is empty. 
*/
pcb_t *allocPcb();

/* 
Insert the element pointed by p in the process queue pointed by head.
Insertion must respect the priority of each pcb.
Process queue must be descending sorted according to pcb's priority. 
*/
void insertProcQ(pcb_t **head, pcb_t *p);

/* 
Return the first element of the queue pointed by head, without removing it.
Return NULL if the queue is empty. 
*/
pcb_t *headProcQ(pcb_t *head);

/* 
Remove the first element in the queue pointed by head.
Return a poiter to the removed element or NULL if the queue is empty. 
*/
pcb_t* removeProcQ(pcb_t **head);

/* 
Remove the element pointed by p from the queue pointed by head.
Return a pointer to p if it is present, NULL otherwise. 
*/
pcb_t* outProcQ(pcb_t **head, pcb_t *p);

/*
For each element in the queue pointed by head, call the function fun(...).
*/
void forallProcQ(pcb_t *head, void fun(pcb_t *pcb, void *), void *arg);


/**** PCB tree management ****/

/*
Insert the pcb pointed by p as child of the pcb pointed by parent. 
*/
void insertChild(pcb_t *parent, pcb_t *p);

/*
Remove the first child of the pcb pointed by p and return its pointer. 
Return NULL if p has no child 
*/
pcb_t *removeChild(pcb_t *p);

/*
Remove the pcb pointed by p from the child list of his father.
If p has no father, it returns NULL.
Return a pointer to the removed element otherwise. 
*/
pcb_t *outChild(pcb_t *p);

#endif
