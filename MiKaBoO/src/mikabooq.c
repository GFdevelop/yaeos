#include <uARMconst.h>
#include <stdint.h>

#include "const.h"
#include "mikabooq.h"

#define ROOT free_proc_list
/* static array with MAXPROC elements */
struct pcb_t free_proc_list[MAXPROC];
/* static array with MAXTHREAD elements */
struct tcb_t free_thread_list[MAXTHREAD];
struct list_head th;
/* static array with MAXMSG elements */
struct msg_t free_msg_list[MAXMSG];
struct list_head mh;

/* initialize the data structure */
/* the return value is the address of the root process */
struct pcb_t *proc_init(void){
	int i;
	ROOT->p_parent = NULL;
	INIT_LIST_HEAD(&ROOT->p_threads);
	INIT_LIST_HEAD(&ROOT->p_children);
	INIT_LIST_HEAD(&ROOT->p_siblings);
	for(i = 0; i < MAXPROC; i++) list_add(&ROOT[i].p_siblings, &ROOT->p_siblings);
	return ROOT;
}

/* alloc a new empty pcb (as a child of p_parent) */
/* p_parent cannot be NULL */
struct pcb_t *proc_alloc(struct pcb_t *p_parent){
	if(p_parent == NULL || list_next(&ROOT->p_siblings) == NULL) return NULL;
	else{
		struct list_head *index = list_next(&ROOT->p_siblings);
		list_del(index);
		struct pcb_t *new = container_of(index, struct pcb_t, p_siblings);
		new->p_parent = p_parent;
		INIT_LIST_HEAD(&new->p_threads);
		INIT_LIST_HEAD(&new->p_children);
		list_add(index, &(p_parent->p_children));
		return new;
	}
}

/* delete a process (properly updating the process tree links) */
/* this function must fail if the process has threads or children. */
/* return value: 0 in case of success, -1 otherwise */
int proc_delete(struct pcb_t *oldproc){
	if(oldproc == NULL || !list_empty(&oldproc->p_children) || !list_empty(&oldproc->p_threads)) return -1;
	else {
		list_del(&oldproc->p_siblings);
		oldproc->p_parent = NULL;
		list_add(&oldproc->p_siblings, &ROOT->p_siblings);
		return 0;
	}
}

/* return the pointer to the first child (NULL if the process has no children) */
struct pcb_t *proc_firstchild(struct pcb_t *proc){
	if (proc == NULL || list_next(&proc->p_children) == NULL) return NULL; //qui funziona?!
	else return container_of(list_next(&proc->p_children), struct pcb_t, p_siblings);
}

/* return the pointer to the first thread (NULL if the process has no threads) */
struct tcb_t *proc_firstthread(struct pcb_t *proc){
	if (proc == NULL || list_next(&proc->p_threads) == NULL) return NULL; //qui funziona!?
	else return container_of(list_next(&proc->p_threads), struct tcb_t, t_next);
}

/****************************************** THREAD ALLOCATION ****************/

/* initialize the data structure */
void thread_init(void){
	int i;
	INIT_LIST_HEAD(&th);
	for(i = 0; i < MAXTHREAD; i++){
		INIT_LIST_HEAD(&free_thread_list[i].t_next);
		INIT_LIST_HEAD(&free_thread_list[i].t_sched);
		INIT_LIST_HEAD(&free_thread_list[i].t_msgq);
		list_add(&free_thread_list[i].t_next, &th);
	}
}

/* alloc a new tcb (as a thread of process) */
/* return -1 if process == NULL or mo more available tcb-s.
	 return 0 (success) otherwise */
struct tcb_t *thread_alloc(struct pcb_t *process){
	if(process == NULL || list_empty(&th)) return NULL;
	else{
		struct list_head *index = list_next(&th);
		struct tcb_t *new = container_of(index, struct tcb_t, t_next);
		list_del(index);
		new->t_pcb = process;
		list_add(index, &process->p_threads);
		return new;
	}
}

/* Deallocate a tcb (unregistering it from the list of threads of its process) */
/* it fails if the message queue is not empty (returning -1) */
int thread_free(struct tcb_t *oldthread){
	if(oldthread == NULL || !list_empty(&oldthread->t_msgq)) return -1;
	else {
		list_del(&oldthread->t_next);
		list_add(&oldthread->t_next, &th);
		return 0;
	}
}

/*************************** THREAD QUEUE ************************/

/* add a tcb to the scheduling queue */
void thread_enqueue(struct tcb_t *new, struct list_head *queue){
	if(queue != NULL) list_add_tail(&new->t_sched, queue->next); 
}

/* return the head element of a scheduling queue (this function does not dequeues the element) return NULL if the list is empty */
struct tcb_t *thread_qhead(struct list_head *queue){
	if(queue == NULL || list_empty(queue)) return NULL;
	else return container_of(list_prev(queue), struct tcb_t, t_sched);
}

/* get the first element of a scheduling queue return NULL if the list is empty */
struct tcb_t *thread_dequeue(struct list_head *queue){
	if(queue == NULL || list_prev(queue) == NULL) return NULL;
	else{
		struct list_head *prev = list_prev(queue);
		list_del(list_prev(queue));
		return container_of(prev, struct tcb_t, t_sched);
	}
}

/*************************** MSG QUEUE ************************/

/* initialize the data structure */
/* the return value is the address of the root process */
void msgq_init(void){
	int i;
	INIT_LIST_HEAD(&mh);
	for(i = 0; i < MAXMSG; i++){
		INIT_LIST_HEAD(&free_msg_list[i].m_next);
		list_add(&free_msg_list[i].m_next, &mh);
	}
}

/* add a message to a message queue. */
/* msgq_add fails (returning -1) if no more msgq elements are available */
int msgq_add(struct tcb_t *sender, struct tcb_t *destination, uintptr_t value){
	if (sender == NULL || destination == NULL || list_empty(&mh)) return -1;
	else{
		struct msg_t *new = container_of(list_next(&mh), struct msg_t, m_next);
		list_del(list_next(&mh));
		new->m_sender = sender;
		new->m_value = value;
		list_add_tail(&new->m_next, &destination->t_msgq);
		return 0;
	}
}

/* retrieve a message from a message queue */
/* -> if sender == NULL: get a message from any sender
	 -> if sender != NULL && *sender == NULL: get a message from any sender and store
	 the address of the sending tcb in *sender
	 -> if sender != NULL && *sender != NULL: get a message sent by *sender */
/* return -1 if there are no messages in the queue matching the request.
	 return 0 and store the message payload in *value otherwise. */
int msgq_get(struct tcb_t **sender, struct tcb_t *destination, uintptr_t *value){
	if (list_empty(&destination->t_msgq)) return -1;
	else {
		struct list_head *prec = (&destination->t_msgq)->next;
		struct msg_t * index = container_of(prec, struct msg_t, m_next);
		if (sender == NULL || (sender != NULL && *sender == NULL)){
			*value = index->m_value;
			if (sender != NULL && *sender == NULL) *sender = *(&index->m_sender);
			list_del(prec);
			list_add(prec, &mh);
			return 0;
		}else if (sender != NULL && *sender != NULL){
			struct list_head *end = prec;
			int boolean = 1;
			while (boolean){
				if (*sender == *(&index->m_sender)){
					*value = index->m_value;
					list_del(prec);
					list_add(prec, &mh);
					boolean = 0;
				}else if (list_is_last(prec, end)) boolean = 0;
					else {
						prec = list_next(prec);
						index = container_of(prec, struct msg_t, m_next);
					}
			}
			if (*sender == *(&index->m_sender)) return 0;
		}
		return -1;
	}
}
