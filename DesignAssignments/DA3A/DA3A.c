/*
 * DA3A.c
 *
 * Created: 10/19/2019 9:26:49 PM
 * Author : Billy
 */ 

// 16MHz CPU clock
#define F_CPU 16000000UL
// 9600 baud rate setting
#define UBRR_9600 103

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdlib.h>
#include <stdio.h>

// global constants and variables
// terminal output variables
int ival;
float fval;
char sendme[80];

// terminal output function declarations
void uart_init(unsigned int ubrr);
void uart_send_string(char *data);

// TIMER1 CTC interrupt handler
ISR (TIMER1_COMPA_vect	)
{
	// get a random integer
	ival = rand();
	fval = (float) ival / rand();
	
	// print to the terminal: a string, a random int, a floating point
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		snprintf(sendme, sizeof(sendme), "A string every second!\nA random integer: %d\nA floating point: %f\r\n\r\n", ival, fval);
		uart_send_string(sendme);
	}
}

int main(void)
{
	// configure terminal and test output
	uart_init(UBRR_9600);
	
	// configure interrupt handling	
	// enable TIMER1 COMPA interrupt
	TIMSK1 |= (1 << OCIE1A);
		
	// set TIMER1 to CTC operations
	TCCR1B |= (1 << WGM12);
	// TIMER1 COMPA = X
	OCR1A = 15620;
	
	// start TIMER1 with prescalar = 1024
	TCCR1B |= (1 << CS12) | (1 << CS10);

	// enable global interrupt
	sei();
	
	// loop forever
	while (1);
}

// configure terminal output
void uart_init(unsigned int ubrr)
{
  // set buad rate to 9600
  UBRR0H = (unsigned char)(ubrr>>8);
  UBRR0L = (unsigned char)ubrr;
  // set terminal to output
  UCSR0B = (1 << TXEN0);
  // set ouput mode
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_send_string(char *data)
{
  while((*data != '\0'))
    {
      while (!(UCSR0A & (1 <<UDRE0)));
      UDR0 = *data;
      data++;
    }
}
