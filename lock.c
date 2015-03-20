#include 3140_concur.h

#define LOCKED 1
#define UNLOCKED 0

/*Data structure for storing information
about a lock.*/
typedef struct lock_impl {
	int locked;
	process_t* queue;
} lock_t;

void l_init(lock_t *l){
	l->locked = UNLOCKED;
	l->queue = NULL;
}

/*Method to lock*/
void l_lock(lock_t *l){
	__disable_interrupts();
	while(true){
		if(l->locked == UNLOCKED){
			l->locked = LOCKED;
			__enable_interrupts();
			return;
		}
		current_process->blocked = 1;
		enqueueProcess(current_process, l->queue);
		process_blocked();
	}
}

void l_unlock(lock_t *l){
	process_t *current; 
	__disable_interrupts();
	l->locked = UNLOCKED;
	current = dequeueProcess(l->queue);
	while(current!=NULL){
		process_t *temp = current;
		current = current->next;
		enqueueProcess(temp, queue);
	}
}

/*
  Puts process at end of the queue.
  Pre-condition: process is not NULL
*/
void enqueueProcess(process_t * process, process_t* queue){
	process_t *iterator; /*Used to iterate over queue*/
  process->next = NULL; 
  if(queue==NULL){
    /*Queue is empty so we can just put this process 
    as the head of the queue*/
    queue = process;
    return;
  }
  iterator = queue;
  /*Iterator to last element in queue*/
  while(iterator->next!=NULL){
    iterator = iterator->next;
  }
  /*Set next pointer of last element to process*/
  iterator->next = process;
}

/*
  Remove the first process in the queue
  and set it's next element to NULL.
  Pre-condition: queue is not empty i.e. queue is not NULL
*/
process_t* dequeueProcess(process_t* queue){
  process_t *top = queue;
  queue = queue->next;
  top->next = NULL;
  return top;
}