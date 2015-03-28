#include "3140_concur.h"
/*Kavan Bhavin kab395*/
/* Constant definitions for convenience */
#define TRUE 1
#define FALSE 0

/*In this implementation we have assumed that each condition variable can
only be associated with one lock. According to a piazza post followup by
professor Martinez this was fine. In order to extend this to allow for multiple
locks we would just need to store a two dimensional data structure, like a Hash Map, 
that mapped each lock to a waiting queue. Then when we wait, we add to the corresponding
queue and when we signal we dequeue from the appropriate queue.*/

/* Initializes struct variables for condition variable implementation*/
void c_init (lock_t *l, cond_t *c){
	c->waiting_queue = NULL;
}

/* Method to wait for condition variable to be signalled*/
void c_wait (lock_t *l, cond_t *c){
	process_t *current;
	/* Disable interrupt for atomicity*/
	__disable_interrupt();
	/* Add process to variable's waiting queue*/
	enqueueProcess(current_process, &(c->waiting_queue));
	/* Set current process' status to BLOCKED so it doesn't get put
	* on ready queue*/
	current_process->blocked = BLOCKED;
	/* Set lock to unlocked state*/
	l->locked = 0;
	/* If there are processes blocked on this lock, we should unblock the
	first one on the queue. */
	if(l->queue != NULL){
		current = dequeueProcess(&(l->queue));
		current->blocked = NOT_BLOCKED; /*Just to be safe*/
		enqueueProcess(current, &ready_queue);
	}
	/* Call process_blocked() to wait until this process is signalled*/
	process_blocked();
	/* Re-enable interrupts*/
	__enable_interrupt();
}

/* Method to signal condition variable. Should only be called after
 checking waiting to ensure that there is a process to signal. Since
 we just transfer the lock to the next waiting process, there is no
 need to actually unlock it. */ 
void c_signal (lock_t *l, cond_t *c){
	process_t *first;
	/*waiting_queue cannot be NULL as waiting must be checked*/
	first = dequeueProcess(&(c->waiting_queue));
	/* Unblock process */
	first->blocked=NOT_BLOCKED;
	/*Add process to scheduler queue so that it is executed and 
	can return from c_wait*/
	enqueueProcess(first, &ready_queue);
}

/* Method to check if there are any processes waiting for this
 condition. User must hold the lock when calling this function. */
int c_waiting (lock_t *l, cond_t *c){
	if(c->waiting_queue == NULL) return FALSE;	
	return TRUE;
}
