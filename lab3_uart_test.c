#include "3140_concur.h"

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

/* Delay 'd' cycles */
void delay (unsigned int d) {
	int i;
	for (i=0; i < d; i++) {
		__no_operation();
	}
}

/* Blink red LED 5 times */
void p1 (void) {
	int i;
	for (i=0; i < 10; i++) {
		delay(10000);
		__disable_interrupt();
		P1OUT ^= 0x01;
		__enable_interrupt();
	}
}

/* Blink green LED 5 times */
void p2 (void) {
	int i;
	for (i=0; i < 10; i++) {
		delay(10000);
		__disable_interrupt();
		P1OUT ^= 0x02;
		__enable_interrupt();
	}
}

/* Print 'A' to UART 20 times */
void pA (void) {
	int i;
	delay(20000);
	for (i=0; i < 20; i++) {
		delay(500);
		uart_putc('A');
	}
}

/* Print 'B' to UART 25 times */
void pB (void) {
	int i;
	delay(20000);
	for (i=0; i < 25; i++) {
		delay(500);
		uart_putc('B');
	}
}

/* Print 'C' to UART 7 times */
void pC (void) {
	int i;
	delay(20000);
	for (i=0; i < 7; i++) {
		delay(500);
		uart_putc('C');
	}
}


/*------------------------------------------------------------------------
 * Set up and launch concurrent processes using our framework
 * 
 *   In addition to the red and green LEDs, this test has three
 *   concurrent processes sending 'A', 'B', and 'C' respectively over
 *   the MSP430 UART. The execution of the processes can be seen by
 *   watching the UART output in a terminal (e.g. TeraTerm).
 * 
 *   If you adjust the Timer A period, you should see different
 *   interleavings of 'ABC' in the terminal output.
 * 
 *   Note: The UART output does not work well with debugger stepping or
 *   breakpoints. It is best to run this test all the way through (play
 *   button).
 *----------------------------------------------------------------------*/

int main (void) {
	WDTCTL = WDTPW + WDTHOLD;
 	P1DIR = 0x03;
 	P1OUT = 0x00;
 	init_uart();	/* Setup UART (Note: Sets clock to 1 MHz) */
 
 	uart_clear_screen();
 	uart_puts("Creating concurrent processes...\n");
 	if (process_create (p1,10) < 0) { return -1; }
 	if (process_create (p2,10) < 0) { return -1; }
 	if (process_create (pA,10) < 0) { return -1; }
 	if (process_create (pB,10) < 0) { return -1; }
 	if (process_create (pC,10) < 0) { return -1; }
 
 	uart_puts("Starting concurrent processes...\n\n");
 	process_start ();
 
 	uart_puts("\n\nAll concurrent processes terminated.\n\n");
 	P1OUT = 0x02;
 	while (1) ;
 	return 0;
}
