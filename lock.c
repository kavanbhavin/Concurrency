#include "3140_concur.h"

/*Constants to represent the state of a lock*/
#define LOCKED 1
#define UNLOCKED 0

/* Method to initialize a lock struct*/
void l_init(lock_t *l){
	l->locked = UNLOCKED;
	l->queue = NULL;
}

/* Method to lock. Lock must be initialized before calling this method.
 If the lock is currently busy, the process is blocked and added to 
 a block_queue represented in the lock's struct. */
void l_lock(lock_t *l){
	/* Disable interrupts to ensure atomicity*/
	__disable_interrupt();
	while(1){
		/* Check state of lock */
		if(l->locked == UNLOCKED){
			/* Since lock is free, mark it as busy*/
			l->locked = LOCKED;
			/* Ensure that current_process is added to ready queue after
			time slice */
			current_process->blocked = NOT_BLOCKED;
			/* Re-enable interrupts again to ensure concurrency*/
			__enable_interrupt();
			return;
		}
		/* If lock is busy, block current process and add to block queue*/
		current_process->blocked = BLOCKED;
		enqueueProcess(current_process, &(l->queue));
		process_blocked();
	}
}
/* Method to unlock. The state is set to the UNLOCKED state and 
 the first process on the waiting queue, if there is one, will be 
 added to the ready queue. */
void l_unlock(lock_t *l){
	process_t *current; 
	/* Disable interrupts to ensure atomicity */
	__disable_interrupt();
	l->locked = UNLOCKED;
	/* If there are no processes blocked on this lock, we can return */
	if(l->queue == NULL){
		__enable_interrupt();
	 	return;
	}
	/* Otherwise we add the first waiting process to the ready queue */
	current = dequeueProcess(&(l->queue));
	current->blocked = NOT_BLOCKED; /*Just to be safe*/
	enqueueProcess(current, &ready_queue);
	__enable_interrupt();
}
