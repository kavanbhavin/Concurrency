#ifndef UART_TEST_UTILITY_H_
#define UART_TEST_UTILITY_H_
/*All functions have been taken from test case provided by ECE 3140 course staff */
/* Initialize the UART for TX (9600, 8N1) */
/* Settings taken from TI UART demo */
void init_uart(void);

/* Transmit a single character over UART interface */
void uart_putc(char c);

/* Transmit a nul-terminated string over UART interface */
/* Transmit a nul-terminated string over UART interface */
void uart_puts(char *str);

/* Clear terminal screen using VT100 commands */
/* http://braun-home.net/michael/info/misc/VT100_commands.htm */
void uart_clear_screen(void);

char itoa(int i);

#endif /*UART_TEST_UTILITY_H_*/
