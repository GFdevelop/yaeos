#ifndef ASL_H
#define ASL_H

#include <uARMtypes.h>
#include <const.h>
#include <pcb.h>

typedef struct semd_t {
	struct semd_t *s_next;
	int *s_key;
	struct pcb_t *s_procQ;
} semd_t;

/**** ASHT - Active Semaphore Hash Table Management ****/

/* Insert PCB pointed by p in the queue of blocked process from semaphore with key as key.
   If the semaphore is not in the queue, it allocates a new one from the free list and insert into ASHD with all the field setted accordingly.
   If it is not possible, it returns -1. 0 otherwise. */

int insertBlocked(int *key, pcb_t *p);

/* Returns the pointer to the PCB of first blocked process on semaphore key. 
   If semaphore does not exist it returns NULL. */
pcb_t *headBlocked(int *key);

pcb_t* removeBlocked(int *key);

void forallBlocked(int *key, void (*fun)(pcb_t *pcb, void *), void *arg);

//outChildBlocked(pcb_t *p);

void initASL();
#endif
