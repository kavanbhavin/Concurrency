#include "3140_concur.h"
/*Kavan Bhavin kab395*/
/*
	Implements enqueue and dequeue functions for a process queue.
*/
/*
  Puts a process at end of a queue.
  Pre-condition: process is not NULL
  Arguments:
  			process_t *process - process to add to the queue
  			process_t **queue - A pointer to the queue that we 
  								wish to add the process to
*/
void enqueueProcess(process_t * process, process_t** q){
	process_t *iterator; /*Used to iterate over queue*/
  process->next = NULL; 
  if(*q==NULL){
    /*Queue is empty so we can just put this process 
    as the head of the queue*/
    *q = process;
    return;
  }
  iterator = *q;
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
  Arguments:
  			process_t *queue - a pointer to the queue to 
  								remove the first element from
*/
process_t* dequeueProcess(process_t **q){
  process_t *top = *q;
  *q = (*q)->next;
  top->next = NULL;
  return top;
}
