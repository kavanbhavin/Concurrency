#include "3140_concur.h"

lock_t w;
lock_t r;

unsigned int nr= 0;

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
		delay (8000);
		__disable_interrupt();
		P1OUT ^= 0x01;
		__enable_interrupt();
	}
}

void reader (void) {
        /* enter */
	l_lock(&r);
	nr++;
        if (nr == 1) {
	   l_lock (&w);
        }
	l_unlock (&r);
	
	/*start reading*/
	p1();
	/*end reading*/

        /* exit */
	l_lock(&r);
	nr--;
        if (nr == 0) {
	   l_unlock (&w);
        }
	l_unlock (&r);
}

void writer (void) {
	/* enter */
	l_lock(&w);

	/*start writing*/
	P1OUT ^= 0x02;
	delay(50000);
	P1OUT ^= 0x02;
	/*end writing*/

        /* exit */
	l_unlock(&w);
}

int main (void){
	WDTCTL = WDTPW + WDTHOLD;
	P1DIR = 0x03;
	P1OUT = 0x00;

	l_init (&w);
	l_init (&r);
 
	if (process_create (writer,10) < 0) {
	 	return -1;
	}
	if (process_create (reader,10) < 0) {
	 	return -1;
	}
	if (process_create (reader,10) < 0) {
	 	return -1;
	}
	if (process_create (reader,10) < 0) {
	 	return -1;
	}
	if (process_create (writer,10) < 0) {
	 	return -1;
	}

	process_start();
 

 
	while (1) ;

	return 0;
}
