/*
 * DA2CT3.c
 *
 * Created: 10/12/2019 1:35:55 PM
 * Author : Billy
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

// global constants and variables
const int COMP = 249;
const int ON1 = 250;
const int OFF1 = 375;
const int ON2 = 1333;
int clock1, clock2, led3;

// long flash on secondary LED
int light2 (void)
{
	// turn the light on
	PORTB &= ~(1 << 2);
	
	// set clock 2 to CTC operation
	TCCR2A = (1 << WGM21);
	// COMPA = 249
	OCR2A = COMP;
	
	// set the clock value
	clock2 = ON2;
		
	// start clock 2 with prescalar = 64
	TCCR2B |= (1 << CS22);
	
	return 0;
}

// external interrupt handler
ISR (INT0_vect)
{
	if (~PIND & (1 << 2)) light2();
}

// pin change interrupt handler
ISR (PCINT1_vect)
{
	if (~PINC & (1 << 3)) light2();
}

// timer0 OVF interrupt handler
ISR (TIMER0_COMPA_vect)
{
	// decrement the clock counter and reset the timer
	clock1--;
	TIFR0 |= (1 << OCF0A);
	
	if (clock1 == 0)
	{
		// if LED3 is on
		if (led3)
		{
			// turn LED3 off
			PORTB |= (1 << 3);
			led3 = 0;
			// set the off clock1 value
			clock1 = OFF1;
		}
		// if LED3 in off
		else
		{
			// turn LED3 on
			PORTB &= ~(1 << 3);
			led3 = 1;
			// set the on clock1 value
			clock1 = ON1;
		}
	}
}

// timer2 OVF interrupt handler
ISR (TIMER2_COMPA_vect)
{
	// decrement the clock counter and reset the timer
	clock2--;
	TIFR2 |= (1 << OCF2A);
	
	if (clock2 == 0)
	{
		// turn LED off
		PORTB |= (1 << 2);
		// stop timer 2
		TCCR2B &= ~(1 << CS22);
	}
}

int main(void)
{
  // Set DDR B,C,D to known initial value
  DDRB = 0x00;
  DDRC = 0x00;
  DDRD = 0x00;
  // set PORT B,C,D to know initial value
  PORTB = 0xFF;
  PORTC = 0xFF;
  PORTD = 0xFF;
  // set PORTB 2 and 3 to output
  DDRB |= (1 << 3);
  DDRB |= (1 << 2);
  // set PINC 3 to input
  DDRC &= ~(1 << 3);
  
  // configure interrupt handling
  // enable INT0, falling edge trigger
  EIMSK = (1 << INT0);
  EICRA = 0x02;
  // enable PCINT11 (PINC.3)
  PCMSK1 = 0x08;
  PCICR = (1 << PCIE1);
  // enable TIMER0 COMPA interrupt
  TIMSK0 |= (1 << OCIE0A);
  // enable TIMER2 COMPA interrupt
  TIMSK2 |= (1 << OCIE2A);
	
  // LED pulse
  // set clock 1 to CTC operation
  TCCR0A |= (1 << WGM01);
  // COMPA = 249
  OCR0A = COMP;
  
  // turn the light on
  PORTB &= ~(1 << 3);
  led3 = 1;
  // set the on clock1 value
  clock1 = ON1;
  
  // start clock 1 with prescalar = 64
  TCCR0B |= (1 << CS01) | (1 << CS00);

  // enable global interrupt
  sei();
  // loop forever
  while (1);
}
