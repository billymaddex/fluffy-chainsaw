/*
 * DA2CT2.c
 *
 * Created: 10/12/2019 11:18:51 AM
 * Author : Billy
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

// global constants and variables
const int TICKS = 0x06;
const int ON1 = 250;
const int OFF1 = 375;
const int ON2 = 1333;
int clock1, clock2;

// long flash on secondary LED
int light2 (void)
{
	// turn the light on
	PORTB &= ~(1 << 2);
	
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
ISR (TIMER0_OVF_vect)
{
	// decrement the clock counter and reset the timer
	clock1--;
	TCNT0 = TICKS;
	
	if (clock1 == 0)
	{
		// if LED3 is on
		if ((~PORTB & 0x08) == 0x08)
		{
			// turn LED3 off
			PORTB |= (1 << 3);
			// set the off clock1 value
			clock1 = OFF1;
		}
		// if LED3 in off
		else
		{
			// turn LED3 on
			PORTB &= ~(1 << 3);
			// set the on clock1 value
			clock1 = ON1;
		}
	}
}

// timer2 OVF interrupt handler
ISR (TIMER2_OVF_vect)
{
	// decrement the clock counter and reset the timer
	clock2--;
	TCNT2 = TICKS;
	
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
  // enable TIMER0 OVF interrupt
  TIMSK0 |= (1 << TOIE0);
  // enable TIMER2 OVF interrupt
  TIMSK2 |= (1 << TOIE2);
	
  
  // set clock 1 to normal operation
  TCCR0A = 0x00;
  // 250 ticks
  TCNT0 = TICKS;
  // set clock 2 to normal operation
  TCCR2A = 0x00;
  // 250 ticks
  TCNT2 = TICKS;
	  
  // LED pulse
  // turn the light on
  PORTB &= ~(1 << 3);
  // set the on clock1 value
  clock1 = ON1;
  // start clock 1 with prescalar = 64
  TCCR0B |= (1 << CS01) | (1 << CS00);

  // enable global interrupt
  sei();
  // loop forever
  while (1);
}
