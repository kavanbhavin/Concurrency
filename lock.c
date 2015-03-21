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
			__enable_interrupt();
			return;
		}
		current_process->blocked = 1;
		enqueueProcess(current_process, &(l->queue));
		process_blocked();
	}
}

void l_unlock(lock_t *l){
	process_t *current; 
	__disable_interrupt();
	l->locked = UNLOCKED;
	current = dequeueProcess(&(l->queue));
	while(current!=NULL){
		process_t *temp = current;
		current = current->next;
		enqueueProcess(temp, &queue);
	}
	__enable_interrupt();
}
