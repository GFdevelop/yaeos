#ifndef ASL_H
#define ASL_H

#include "pcb.h"

typedef struct semd_t{
	struct semd_t *s_next;
	int *s_key;
	struct pcb_t *s_procQ;
}semd_t;

/**** ASHT - Active Semaphore Hash Table Management ****/

/*
Insert the pcb pointed by p in the queue of blocked processes associated with the semaphore with parameter "key" as key.
If the semaphore is not in the ASHT, it allocates a SEMD from semdFree list and insert into ASHT with all the field setted accordingly.
If it is not possible, it returns -1. 0 otherwise. 
*/

int insertBlocked(int *key, pcb_t *p);

/* 
Returns the pointer to the pcb of first blocked process on semaphore key without removing it. 
If semaphore key does not exist it returns NULL. 
*/
pcb_t *headBlocked(int *key);

/*
Returns the first pcb from the blocked queue associated with the SEMD of ASHT with "key" as key.
If it does not exist in ASHT, returns NULL. Returns the element otherwise.
If the blocked queue of the semaphore will be empty after removal, it removes the semaphore's identifier from ASHT and insert into semdFree list-
*/
pcb_t* removeBlocked(int *key);

/*
For each blocked process in the queue of semaphore having "key" as key, call the function fun(...).
*/
void forallBlocked(int *key, void (*fun)(pcb_t *pcb, void *), void *arg);

/*
Removes the pcb pointed by p from the blocked process queue of the semaphore it is blocked on.
ASHT must be updated accordingly. 
*/
void outChildBlocked(pcb_t *p);

/*
Initializes the semdFree list in order to contain all the elements of the semd_table. 
This method will be invoked only once in the initialization phase of the data structure.
*/
void initASL();

#endif
