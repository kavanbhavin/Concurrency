#include "3140_concur.h"
#include <time.h>

void delay (void)
{
	int i;
	for (i=0; i < 8000; i++) {
		__no_operation();
		__no_operation();
		__no_operation();
	}
}

void p1 (void)
{
	int i;
	for (i=0; i < 10; i++) {
		delay ();
		__disable_interrupt();
		P1OUT ^= 0x01;
		__enable_interrupt();
	}
}


int main (void)
{
 WDTCTL = WDTPW + WDTHOLD;
 P1DIR = 0x03;
 P1OUT = 0x00;
 
 if (process_create (p1,10) < 0) {
 	return -1;
 }
 process_start ();
 
 while (1) ;

 return 0;
}
