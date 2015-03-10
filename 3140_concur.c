/*************************************************************************
 *
 *  Copyright (c) 2011 Cornell University
 *  Computer Systems Laboratory
 *  Cornell University, Ithaca, NY 14853
 *  All Rights Reserved
 *
 *  $Id$
 *
 **************************************************************************
 */
#include "3140_concur.h"
#include <stdlib.h>	
#define PROCEESS_RUNTIME 2000
/*
  State layout:

  .-----------------.
  |       retpc     |
  |-----------------|
  |       pc        |
  |-----------------|
  |       sr        |
  |-----------------|
  |       R4        |
  |-----------------|
  |      ....       |
  |                 |
  |-----------------|
  |      R15        | <---- sp
  '-----------------'

  State requires 15 slots on the stack.

  The rest of the registers are automatically saved when
  process_switch() is called, since they are caller-saved registers!
*/


/*------------------------------------------------------------------------
 *
 *  process_init --
 *
 *   Initialize process state
 *
 *------------------------------------------------------------------------
 */
unsigned int 
process_init (void (*f)(void), int n)
{
  int *sp;	/* Pointer to process stack (allocated in heap) */ 
  int i;

  /* in reality, there are 14 more slots needed */
  n += 15;

  /* Allocate space for the process's stack */
  sp = (int *) malloc(n*sizeof(int));
  if (sp == NULL) { return 0; }	/* Allocation failed */
  
  /* Initialize the stack to all zeros */ 
  /* Note: Could just use calloc instead */ 
  for (i=0; i < n; i++) {
  	sp[i] = 0;
  }
  
  /* Highest stack address holds the address we should jump to
   * after the process terminates. process_terminated is
   * a pointer-to-a-function, so we cast it to int first. */
  sp[n-1] = (unsigned int) process_terminated;
  
  /* Put the function entry point and an initial status register
   * at the next highest stack address. This emulates an
   * interrupt, and allows us to start executing the function
   * by doing a reti */
  sp[n-2] = (unsigned int) f;
  sp[n-3] = GIE;	/* Initialize SR to only have GIE bit set */
  
  return (unsigned int)&(sp[n-15]);
}

/* Starts up the concurrent execution */
void process_start (void){
  TACTL &= TACLR;
  TACCTL0=CCIE;
  TACCTL0 |= CM0;     /* set to compare mode */
  TACTL |= TASSEL_2;  /* set clock to SMCLK */
  TACTL |= MC_1;    /* set mode to up mode*/
  TACTL |= ID_0;  
  TACCR0 = PROCEESS_RUNTIME;
  __disable_interrupt();
  process_begin();
  /*should I enable interrupts again*/
}

/* the currently running process */
process_t *current_process=NULL; 


int process_create (void (*f)(void), int n){
  /*Disable interrupts before calling process_init*/
  __disable_interrupt();
  /* Allocate space for new process' bookkeping struct */
  process_t * new_process = (process_t *) malloc(sizeof process_t);
  /*If malloc failed, we must return -1*/
  if(new_process == NULL) {
    __enable_interrupt();
    return -1;
  }
  /*Initialize the process and store its stack pointer in the right place*/
  new_process->sp = process_init(f, n);
  /*If this failed, free struct memory and return -1*/
  if(new_process->sp == 0){
    free new_process;
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
  if(current_process==NULL){
    /*this really shouldn't happen*/
    return 0;
  }
  if(cursp == 0 && current_process!= NULL){
    /*no currently running process. This means current process may have terminated*/
    process_t *toFree=popProcess();
    free toFree;
  }else if(current_process!=NULL){
    current_process->sp = cursp;
    pushProcess(popProcess());
  }
  if(current_process== NULL) return 0;
  return current_process->sp;
}


/*Bookkeping struct for keeping track of processes*/
typedef struct process_state {
  /* Stack pointer for this process */ 
  unsigned int sp;
  struct process_state *next;
} process_t;

void pushProcess(process_t * process){
  if(current_process==NULL){
    current_process = process;
    return;
  }
  process_t *iterator = current_process;
  while(iterator->next!=NULL){
    iterator = iterator->next;
  }
  iterator->next = process;
}

process_t* popProcess(){
  process_t *top = current_process;
  current_process  = current_process->next;
  top->next = NULL;
  return top;
}