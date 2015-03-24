#include "3140_concur.h"

#define LOCKED 1
#define UNLOCKED 0

void l_init(lock_t *l){
	l->locked = UNLOCKED;
	l->queue = NULL;
}

/*Method to lock*/
void l_lock(lock_t *l){
	__disable_interrupt();
	while(1){
		if(l->locked == UNLOCKED){
			l->locked = LOCKED;
			current_process->blocked = NOT_BLOCKED;
			__enable_interrupt();
			return;
		}
		current_process->blocked = BLOCKED;
		enqueueProcess(current_process, &(l->queue));
		process_blocked();
	}
}
/*Do we need to worry about the case where this is already locked*/
void l_unlock(lock_t *l){
	process_t *current; 
	__disable_interrupt();
	l->locked = UNLOCKED;
	if(l->queue == NULL) return;
	current = dequeueProcess(&(l->queue));
	current->blocked = NOT_BLOCKED;
	enqueueProcess(current, &queue);
	__enable_interrupt();
}
