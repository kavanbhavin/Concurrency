#include "3140_concur.h"
#define TRUE 1
#define FALSE 0


void c_init (lock_t *l, cond_t *c){
	c->lock = l;
	c->waiting_queue = NULL;
	return;
}

void c_wait (lock_t *l, cond_t *c){
	__disable_interrupt();
	enqueueProcess(current_process, &(c->waiting_queue));
	current_process->blocked = BLOCKED;
	l_unlock(l);
	process_blocked();
}

void c_signal (lock_t *l, cond_t *c){
	process_t *first;
	__disable_interrupt();
	/*Should not be called when queue is null (must check waiting)*/
	first = dequeueProcess(&(c->waiting_queue));
	enqueueProcess(first, &queue);
	__enable_interrupt();
	return;
}
/* YOU MUST HOLD LOCK BEFORE CALLING*/
int c_waiting (lock_t *l, cond_t *c){
	if(c->waiting_queue == NULL) return 0;
	return 1;
}
