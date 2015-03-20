#include "3140_concur.h"
#include <stdlib.h>	
/*Kavan Bhavin kab395@cornell.edu & Jun Wei Lam jl2576@cornell.edu*/

/*The duration of time we let a single process run for*/
#define PROCEESS_RUNTIME 10000
#define BLOCKED 1
#define NOT_BLOCKED 0

/*
  Bookkeping struct for keeping track of processes.
  Essentially our implementation of a one element of a
  Process Control Block.
*/
struct process_state {
  /* Stack pointer for this process */ 
  unsigned int sp;
  /*Pointer to next process. NULL if no next process*/
  struct process_state *next;
  /*Repesents whether a process has been blocked*/
  unsigned int blocked;
};

/* 
  Starts up the concurrent execution.
  Timer A is setup and process_begin() is called.
*/
void process_start (void){
/*Interrupts are disabled to satisfy pre-condition of process_begin()*/
  TACTL &= TACLR;     /*Clear timer*/
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
process_t *queue=NULL;

/*
  Puts process at end of the queue.
  Pre-condition: process is not NULL
*/
void enqueueProcess(process_t * process){
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
process_t* dequeueProcess(){
  process_t *top = queue;
  queue = queue->next;
  top->next = NULL;
  return top;
}

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
  /*Set block variable to 0*/
  new_process->blocked = NOT_BLOCKED;
  /*Add this process to process queue*/
  enqueueProcess(new_process);
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
  	if(queue == NULL) return 0;
    /*Set current process to first element in queue*/
    current_process = dequeueProcess(); 
  }else{
    /*Save stack pointer for future executions of process*/
    current_process->sp = cursp;
    /*Push process to back of queue*/
    if(current_process->blocked != BLOCKED) enqueueProcess(current_process);
    /*
      We do not have to check if queue is NULL before calling
      dequeueProcess because we have just added an element to the queue.
      Set current process to first element in queue.
    */
      //TODO do we need to check?
    current_process = dequeueProcess();
  }
  /*Return stack pointer to current process to be exectued*/
  return current_process->sp;
}



