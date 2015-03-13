#include "3140_concur.h"
#include <stdlib.h>	
#define PROCEESS_RUNTIME 400
/* Starts up the concurrent execution */
void process_start (void){
  __disable_interrupt();
  TACTL &= TACLR;
  TACCTL0 = CCIE;
  TACCTL0 |= CM0;     /* set to compare mode */
  TACTL |= TASSEL_2;  /* set clock to SMCLK */
  TACTL |= MC_1;    /* set mode to up mode*/
  TACTL |= ID_0;  
  TACCR0 = PROCEESS_RUNTIME;
  process_begin();
}

/*Bookkeping struct for keeping track of processes*/
struct process_state {
  /* Stack pointer for this process */ 
  unsigned int sp;
  struct process_state *next;
};

/* the currently running process */
process_t *current_process=NULL; 

/*Queue of processes*/
process_t *queue=NULL;

/*Put process at end of queue.*/
void pushProcess(process_t * process){
	process_t *iterator;
  process->next = NULL;
  if(queue==NULL){
    queue = process;
    return;
  }
  iterator = queue;
  while(iterator->next!=NULL){
    iterator = iterator->next;
  }
  iterator->next = process;
}

/*Take off the top process and set his next to Null.*/
process_t* popProcess(){
  process_t *top = queue;
  queue  =queue->next;
  top->next = NULL;
  return top;
}

int process_create (void (*f)(void), int n){
	process_t * new_process;
  /*Disable interrupts before calling process_init*/
  __disable_interrupt();
  /* Allocate space for new process' bookkeping struct */
  new_process =(process_t *) malloc(sizeof (process_t));
  /*If malloc failed, we must return -1*/
  if(new_process == NULL) {
    __enable_interrupt();
    return -1;
  }
  /*Initialize the process and store its stack pointer in the right place*/
  new_process->sp = process_init(f, n);
  /*If this failed, free struct memory and return -1*/
  if(new_process->sp == 0){
    free (new_process);
    __enable_interrupt();
    return -1;
  }
  /*Set next to NULL*/
  new_process->next = NULL;
  /*Add this process to process queue*/
  pushProcess(new_process);
  /*Enable interrupts again*/
  __enable_interrupt();
  return 0;
}

/* Called by the runtime system to select another process.
   "cursp" = the stack pointer for the currently running process
*/
unsigned int process_select (unsigned int cursp){
  if(current_process==NULL || cursp == 0){
  	if(queue == NULL) return 0;
    current_process = popProcess();
  }else{
    current_process->sp = cursp;
    pushProcess(current_process);
    current_process = popProcess();
  }
  return current_process->sp;
}



