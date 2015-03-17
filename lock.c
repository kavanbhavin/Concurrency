#include 3140_concur.h

#define LOCKED 1
#define UNLOCKED 0

/*Data structure for storing information
about a lock.*/
typedef struct lock_impl {
	int locked;
} lock_t;

void l_init(lock_t *l){
	l->locked = UNLOCKED;
}

/*Method to lock*/
void l_lock(lock_t *l){
	__disable_interrupts();
	if(l->locked == UNLOCKED){
		l->locked = LOCKED;
		__enable_interrupts();
		return;
	}
	current_process->blocked = 1;
	process_blocked();
}

void l_unlock(lock_t *l){
	
}