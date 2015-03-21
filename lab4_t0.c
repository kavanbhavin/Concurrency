#include "3140_concur.h"

lock_t l;

void delay (void){
	int i= 0;
	while (i < 8000) {
		__no_operation();
		__no_operation();
		__no_operation();
		i++;
	}
}

void p1(void){
	int i= 0;
	while(i < 11){
		/*NCS*/
		delay();
		i++;
		/*CS*/
		l_lock(&l);
		P1OUT^= 0x01;
		delay();
		P1OUT^= 0x01;
		delay();
		P1OUT^= 0x02;
		delay();
		P1OUT^= 0x02;
		delay();
		l_unlock(&l);
	}
}

int main(void){
	WDTCTL = WDTPW + WDTHOLD;
	P1DIR = 0x03;
	P1OUT = 0x00;

	l_init (&l);
	
	if (process_create (p1,10) < 0) {
	 	return -1;
	}
	if (process_create (p1,10) < 0) {
	 	return -1;
	}
	
	process_start();
	P1OUT= 0x02;
	while(1);
	return 0;	
}
