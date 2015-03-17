#include "3140_concur.h"
/*Kavan Bhavin kab395@cornell.edu & Jun Wei Lam jl2576@cornell.edu*/
/*This test just runs one process to make sure 
process_select just continually runs this process*/
/*Credit to 3420 staff for code which has been reused from lab3_t0.c*/

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
