#include "3140_concur.h"
#include <stdlib.h>
/*Kavan Bhavin kab395@cornell.edu & Jun Wei Lam jl2576@cornell.edu*/

/*The duration of time we let a single process run for*/
#define PROCEESS_RUNTIME 10000

/* 
  Starts up the concurrent execution.
  Timer A is setup and process_begin() is called.
*/
void process_start (void){
/*Interrupts are disabled to satisfy pre-condition of process_begin()*/
  TACTL |= TACLR;     /*Clear timer*/
  TACCTL0 = CCIE;     /*Allow timer A to trigger interrupts*/
  TACCTL0 |= CM0;     /* set to compare mode */
  TACTL |= TASSEL_2;  /* set clock to SMCLK */
  TACTL |= MC_1;    /* set mode to up mode*/
  TACTL |= ID_0;    /* Divide by 1*/
  TACCR0 = PROCEESS_RUNTIME; /*Set duration of timer*/
  __disable_interrupt(); 
  process_begin();
}

/* the currently running process */
process_t *current_process=NULL; 

/*
  Pointer to head of queue of processes.
  Implementation of Process Control Block.
*/
process_t *ready_queue=NULL;

/* 
  Creates a new process. 
  f is a function pointer to this new process 
  n is the initial stack size for this process.
  Returns 0 if this process was successfully created and -1 if not.
*/
int process_create (void (*f)(void), int n){
	process_t * new_process;
  /*Disable interrupts before calling process_init to meet pre-condition*/
  __disable_interrupt();
  /* Allocate space for new process' bookkeping struct */
  new_process =(process_t *) malloc(sizeof (process_t));
  /*If malloc failed, we must return -1*/
  if(new_process == NULL) {
    __enable_interrupt();
    return -1;
  }
  /*Initialize the process and store its stack pointer in it's struct*/
  new_process->sp = process_init(f, n);
  /*If this failed, free struct memory and return -1*/
  if(new_process->sp == 0){
    free (new_process);
    __enable_interrupt();
    return -1;
  }
  /*Initialize block variable to 0*/
  new_process->blocked = NOT_BLOCKED;
  /*Add this process to ready process queue*/
  enqueueProcess(new_process, &ready_queue);
  /*Enable interrupts again*/
  __enable_interrupt();
  return 0;
}

/* Called by the runtime system to select another process.
   "cursp" = the stack pointer for the currently running process
   cursp is 0 if there is no currently running process.
   Returns the stack pointer for the next process to be run. 
   If there are no processes left to run, returns 0.
*/
unsigned int process_select (unsigned int cursp){
  /*
  If either current_process is NULL or cursp is 0
  we don't need to worry about saving cursp 
  for future executions of the process.
  */
  if(current_process == NULL || cursp == 0){
    /*If the queue is empty, there is nothing we can select so return 0*/
  	if(ready_queue == NULL) return 0;
    /*Set current process to first element in queue*/
    current_process = dequeueProcess(&ready_queue); 
  }else{
    /*Save stack pointer for future executions of process*/
    current_process->sp = cursp;
    /*If process has been blocked, we should not add it to our ready queue
    * because it has already been added to a waiting queue*/
    if(current_process->blocked == NOT_BLOCKED) enqueueProcess(current_process, &ready_queue);
    /*
      We do not have to check if queue is NULL before calling
      dequeueProcess because if current_process wasn't blocked we would have added it
      to the queue. If it was, there must be some other ready process (otherwise it 
      wouldn't block).
      Set current process to first element in queue.
    */
    current_process = dequeueProcess(&ready_queue);
  }
  /*Return stack pointer to current process to be exectued*/
  return current_process->sp;
}

