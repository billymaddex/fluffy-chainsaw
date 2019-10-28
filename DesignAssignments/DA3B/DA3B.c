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

// terminal output function declarations
void uart_init(unsigned int ubrr);
void uart_send_string(char *data);

// ADC function declarations
void adc_init (void);

// TIMER1 CTC interrupt handler
ISR(TIMER1_COMPA_vect)
{
  // start ADC conversion
  ADCSRA |= (1 << ADSC);
}

ISR(ADC_vect)
{
  // collect and compute ADC value
  // value stored in ADC
  //uint16_t voltage = ADC;
  float cel = (5 * ADC / 1000.0) + 4;
  float fahr = 1.8 * cel + 32;
  
  // output string
  char report[80];
  
  // generate output string
  snprintf(report, sizeof(report), "Current temperature:\t%.1f° C\t%.1f° F\r\n", cel, fahr); 

  // output to terminal
  ATOMIC_BLOCK(ATOMIC_FORCEON)
  {
    uart_send_string(report); 
  }
}

int main(void)
{
	// configure terminal
	uart_init(UBRR_9600);

	// configure and start ADC
	adc_init();
	
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
  // set output mode
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

// configure ADC input
void adc_init (void)
{
  // set PORTC to input for ADC
  DDRC = 0x00;
  
  ADMUX =
    // Reference Selection Bits
    // AVcc with external capacitor at AREF
    (0 << REFS1) |
    (1 << REFS0) |
    // ADC Left Adjust Result
    (1 << ADLAR) |
    // Analog Channel Selection Bits
    // ADC4 (PC4 PIN27)
    (0 << MUX0) |
	(1 << MUX2) |
    (0 << MUX1) |
    (0 << MUX0);
  ADCSRA =
    // ADC ENable
    (1 << ADEN) |
    // ADC Start Conversion
    (0 << ADSC) |
    // ADC Auto Trigger Enable
    (0 << ADATE) |
    // ADC Interrupt Flag
    (0 << ADIF) |
    // ADC Interrupt Enable
    (1 << ADIE) |
    // ADC Prescaler Select Bits
    // prescalar = 128
    (1 << ADPS2) |
    (1 << ADPS1) |
    (1 << ADPS0);
}