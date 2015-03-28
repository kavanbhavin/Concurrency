#include "3140_concur.h"
#include "uart_test_utility.h"

lock_t l;
cond_t cr;
cond_t cw; 

unsigned int nr= 0;
unsigned int nw= 0;

void delay (unsigned int n){
	int i;
	for (i=0; i < n; i++) {
		__no_operation();
		__no_operation();
		__no_operation();
	}
}

void p1 (){
	int i;
	for (i=0; i < 10; i++) {
		delay (50000);
		__disable_interrupt();
		uart_puts("A reader is reading\n");
		P1OUT ^= 0x01;
		__enable_interrupt();
	}
}

void reader (void) {
	l_lock(&l);
	if(nw!=0){
		c_wait(&l,&cr);
	}
	nr++;
	if(c_waiting(&l,&cr)){
		c_signal(&l,&cr);
	}
	else{
		l_unlock(&l);
	}
	uart_puts("A reader has started reading\n");	
	/*start reading*/
	p1();
	/*end reading*/
	uart_puts("A reader has finished reading\n");
	l_lock(&l);
	nr--;
  	if(c_waiting(&l,&cw) && nr == 0){
  		c_signal(&l,&cw);
  	}
  	else if(c_waiting(&l,&cr)){
  		c_signal(&l,&cr);
  	}
  	else{
  		l_unlock(&l);	
  	}
}

void writer (void) {
	l_lock(&l);
	if(nw!=0 || nr!=0){
		c_wait(&l,&cw);
	}
	nw++;
	l_unlock(&l);
	/*start writing*/
	uart_puts("A writer has started writing\n");
	P1OUT ^= 0x02;
	delay(50000);
	P1OUT ^= 0x02;
	uart_puts("A writer has finished writing\n");
	/*end writing*/
	l_lock(&l);
	nw--;
  	if(c_waiting(&l,&cw) && nr == 0){
  		c_signal(&l,&cw);
  	}
  	else if(c_waiting(&l,&cr)){
  		c_signal(&l,&cr);
  	}
  	else{
  		l_unlock(&l);	
  	}
}

int main (void){
	WDTCTL = WDTPW + WDTHOLD;
	P1DIR = 0x03;
	P1OUT = 0x00;

	l_init (&l);
	c_init (&l,&cr);
	c_init (&l,&cw);
 	
 	init_uart();	/* Setup UART (Note: Sets clock to 1 MHz) */
 
 	uart_clear_screen();
 	uart_puts("Creating concurrent processes...\n");
 	
	if (process_create (writer,15) < 0) {
	 	return -1;
	}
	if (process_create (reader,15) < 0) {
	 	return -1;
	}
	if (process_create (reader,15) < 0) {
	 	return -1;
	}
	if (process_create (reader,15) < 0) {
	 	return -1;
	}
	if (process_create (writer,15) < 0) {
	 	return -1;
	}
	
	uart_puts("Starting concurrent processes...\n\n");
	process_start();
 
	uart_puts("\n\nAll concurrent processes terminated.\n\n");
 
	while (1) ;

	return 0;
}
