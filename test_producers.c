#include "3140_concur.h"
#include "uart_test_utility.h"
#include <stdlib.h>
/*Kavan Bhavin kab395*/
/* Some of the code from sample test cases provided by ECE 3140 course staff
 * has been reused
 */
 /* Defines the capacity of the queue*/
#define CAPACITY 3
 
typedef struct queue_node {
	int val;
	struct queue_node *next;
} q_t;
 
int count =0;
lock_t l;
cond_t full;
cond_t empty;
q_t *queue=NULL;

q_t* create_node(int node_val){
	q_t *q = (q_t*)malloc(sizeof(q_t));
	q->val = node_val;
	q->next=NULL;
	return q;
}

void enqueueNode(q_t *node){
	q_t *iterator; /*Used to iterate over queue*/
  if(queue==NULL){
    /*Queue is empty so we can just put this process 
    as the head of the queue*/
    queue = node;
    return;
  }
  iterator = queue;
  /*Iterator to last element in queue*/
  while(iterator->next!=NULL){
    iterator = iterator->next;
  }
  /*Set next pointer of last element to process*/
  iterator->next = node;
}

q_t* dequeueNode(){
  q_t *top = queue;
  queue = (queue)->next;
  top->next = NULL;
  return top;
}

void qwrite(int val){
	q_t* node = create_node(val);
	enqueueNode(node);
	uart_puts("Wrote ");
	uart_putc(itoa(val));
	uart_puts(" to queue\n");
}

void qread(){
	int val;
	q_t *node = dequeueNode();
	val = node->val;
	free(node);
	uart_puts("Read ");
	uart_putc(itoa(val));
	uart_puts(" from queue\n");
}

/* Method that 'produces value'*/ 
void producer(lock_t *l, int val_to_write){
 	l_lock(l);
 	if(count==CAPACITY) c_wait(l, &full);
 	count++;
 	qwrite(val_to_write);
 	if(count==1 && c_waiting(l, &empty)) c_signal(l, &empty);
 	else l_unlock(l);
}

void consumer(lock_t *l){
	l_lock(l);
	if(count==0) c_wait(l, &empty);
	count--;
	qread();
	if((count==CAPACITY-1) && c_waiting(l, &full)) c_signal(l, &full);
	else l_unlock(l);
}

void p1(){
	producer(&l, 5);
}
void p2(){
	producer(&l, 3);	
}

void p3(){
	producer(&l, 9);	
}

void p4(){
	producer(&l, 4);	
}
void p5(){
	producer(&l, 7);
}
void pread(){
	consumer(&l);
}

int main (void){
	WDTCTL = WDTPW + WDTHOLD;
	l_init(&l);
	c_init(&l, &full);
	c_init(&l, &empty);
	
	init_uart();	/* Setup UART (Note: Sets clock to 1 MHz) */
 
 	uart_clear_screen();
 	uart_puts("Creating concurrent processes...\n");
 	if (process_create (pread,20) < 0) {
	 	return -1;
	}
 	if (process_create (p1,20) < 0) {
	 	return -1;
	}
	if (process_create (p2,20) < 0) {
	 	return -1;
	}
	if (process_create (p3,20) < 0) {
	 	return -1;
	}
	if (process_create (p4,20) < 0) {
	 	return -1;
	}
	if (process_create (p5,20) < 0) {
	 	return -1;
	}
	if (process_create (pread,20) < 0) {
	 	return -1;
	}
	uart_puts("Starting concurrent processes...\n\n");
	process_start();
	uart_puts("\n\nAll concurrent processes terminated.\n\n");
 
	while (1) ;

	return 0;
}
