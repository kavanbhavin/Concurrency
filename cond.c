#include "3140_concur.h"
/* Constant definitions for convenience */
#define TRUE 1
#define FALSE 0

/*ASSUME ONLY ONE LOCK PER CONDITION VARIABLE: MARTINEZ PIAZZA POST*/

/* Initializes struct variables for condition variable implementation*/
void c_init (lock_t *l, cond_t *c){
	c->waiting_queue = NULL;
	return;
}

/*I don't believe we have to use interrupts as the process has 
already obtained a lock*/
/* Method to wait for condition variable to be signalled*/
void c_wait (lock_t *l, cond_t *c){
	/* Add process to variable's waiting queue*/
	enqueueProcess(current_process, &(c->waiting_queue));
	/* Set current process' status to BLOCKED so it doesn't get put
	* on ready queue*/
	current_process->blocked = BLOCKED;
	l_unlock(l);
	process_blocked();
}

/* Method to signal condition variable. Should only be called after
 checking waiting to ensure that there is someone to signal. Since
 we just transfer the lock to the next waiting process, there is no
 need to actually unlock it. */ 
void c_signal (lock_t *l, cond_t *c){
	process_t *first;
	/*waiting_queue cannot be NULL as waiting must be checked*/
	first = dequeueProcess(&(c->waiting_queue));
	enqueueProcess(first, &ready_queue);
	return;
}
/* Method to check if there are any processes waiting for this
 condition. User must hold the lock when calling this function. */
int c_waiting (lock_t *l, cond_t *c){
	if(c->waiting_queue == NULL) return 0;
	return 1;
}
