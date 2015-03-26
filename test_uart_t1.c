#include "3140_concur.h"
/*Kavan Bhavin kab395*/
/* Some of the code from sample test cases provided by ECE 3140 course staff
 * has been reused
 */
 
/*------------------------------------------------------------------------
 * UART communication functions
 *----------------------------------------------------------------------*/

/* Initialize the UART for TX (9600, 8N1) */
/* Settings taken from TI UART demo */ 
void init_uart(void) {
	BCSCTL1 = CALBC1_1MHZ;        /* Set DCO for 1 MHz */
	DCOCTL  = CALDCO_1MHZ;
	P3SEL = 0x30;                 /* P3.4,5 = USCI_A0 TXD/RXD */
	UCA0CTL1 |= UCSSEL_2;         /* SMCLK */
	UCA0BR0 = 104;                /* 1MHz 9600 */
	UCA0BR1 = 0;                  /* 1MHz 9600 */
	UCA0MCTL = UCBRS0;            /* Modulation UCBRSx = 1 */
	UCA0CTL1 &= ~UCSWRST;         /* Initialize USCI state machine */
}

/* Transmit a single character over UART interface */
void uart_putc(char c) {
    while(!(IFG2 & UCA0TXIFG)); /* Wait for TX buffer to empty */
    UCA0TXBUF = c;				/* Transmit character */
}

/* Transmit a nul-terminated string over UART interface */
void uart_puts(char *str) {
	while (*str) {
		/* Replace newlines with \r\n carriage return */
		if(*str == '\n') { uart_putc('\r'); }
		uart_putc(*str++);
	}
}

/* Clear terminal screen using VT100 commands */
/* http://braun-home.net/michael/info/misc/VT100_commands.htm */
void uart_clear_screen(void) {
	uart_putc(0x1B);		/* Escape character */
 	uart_puts("[2J");		/* Clear screen */
 	uart_putc(0x1B);		/* Escape character */
 	uart_puts("[0;0H");		/* Move cursor to 0,0 */
}
/*------------------------------------------------------------------------
 * Concurrent process test functions
 *----------------------------------------------------------------------*/

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
		uart_puts("A reader is reading\n");
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
	uart_puts("A writer is writing\n");
	/*start writing*/
	P1OUT ^= 0x02;
	delay(50000);
	P1OUT ^= 0x02;
	/*end writing*/

        /* exit */
	l_unlock(&w);
}
/*
 * In addition to blinking the LEDs in an interleaving, this test case also
 * sends information over the MSP430 UART. These can be observed to ensure
 * that no illegal interleavings take place.
 */
  
int main (void) {
	WDTCTL = WDTPW + WDTHOLD;
	P1DIR = 0x03;
	P1OUT = 0x00;

	l_init (&w);
	l_init (&r);
 
 	init_uart();	/* Setup UART (Note: Sets clock to 1 MHz) */
 
 	uart_clear_screen();
 	uart_puts("Creating concurrent processes...\n");
	if (process_create (writer,10) < 0) {
	 	return -1;
	}
	if (process_create (reader,10) < 0) {
	 	return -1;
	}
	if (process_create (writer,10) < 0) {
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
	
	uart_puts("Starting concurrent processes...\n\n");
	process_start();
 
	uart_puts("\n\nAll concurrent processes terminated.\n\n");
 
	while (1) ;

	return 0;
}
